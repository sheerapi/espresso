#include "graphics/RenderGraph.h"
#include "core/log.h"
#include "graphics/GraphicContext.h"
#include <algorithm>
#include <queue>
#include <unordered_map>

namespace graphics
{
	void RenderGraph::addPass(const std::string& name, RenderPass* pass)
	{
		passes.push_back(
			GraphicContext::current->getDevice()->createPassDescriptor(name, pass));
		compile();
	}

	void RenderGraph::removePass(const std::string& name)
	{
		auto pass =
			std::remove_if(passes.begin(), passes.end(), [=](auto& desc)
						   { return desc->name == name; });

		log_trace("destroyed pass %s", (*pass)->name.c_str());
		compile();
	}

	void RenderGraph::setOutput(std::string& output)
	{
		requiredOutputs.insert(std::move(output));
	}

	void RenderGraph::compile()
	{
		std::vector<PassAccess> passAccessList = _collectPassAccess();
		auto [dependencies, successors, inDegree] = buildDependencyGraph(passAccessList);

		execOrder = performTopologicalSort(passAccessList, successors, inDegree);

		if (execOrder.size() != passes.size())
		{
			log_fatal("render graph contains cyclic dependencies");
			return;
		}

		auto culled = _cullPasses(passAccessList);

		log_trace("compiled render graph with %d passes (%d culled)", execOrder.size(),
				  culled);
	}

	void RenderGraph::execute()
	{
		for (auto& pass : execOrder)
		{
			pass->pass->execute();
		}
	}

	auto RenderGraph::_collectPassAccess() -> std::vector<RenderGraph::PassAccess>
	{
		std::vector<PassAccess> passAccessList;

		for (auto& passDesc : passes)
		{
			auto* pass = passDesc->pass.get();
			RenderGraphBuilder builder(this, pass);
			pass->setup(builder);
			passAccessList.push_back(
				{passDesc.get(), builder.getReads(), builder.getWrites()});
		}

		return passAccessList;
	}

	auto RenderGraph::buildDependencyGraph(const std::vector<PassAccess>& passAccessList)
		-> std::tuple<std::unordered_map<RenderPassDescriptor*,
										 std::unordered_set<RenderPassDescriptor*>>,
					  std::unordered_map<RenderPassDescriptor*,
										 std::unordered_set<RenderPassDescriptor*>>,
					  std::unordered_map<RenderPassDescriptor*, int>>
	{
		std::unordered_map<std::string, RenderPassDescriptor*> lastWriter;
		std::unordered_map<std::string, std::unordered_set<RenderPassDescriptor*>>
			readersSinceLastWrite;
		std::unordered_map<RenderPassDescriptor*,
						   std::unordered_set<RenderPassDescriptor*>>
			dependencies;

		for (const auto& access : passAccessList)
		{
			auto* current = access.pass;
			dependencies[current] = {};

			for (const auto& resource : access.reads)
			{
				if (lastWriter.count(resource) != 0U)
				{
					dependencies[current].insert(lastWriter[resource]);
				}
				readersSinceLastWrite[resource].insert(current);
			}

			for (const auto& resource : access.writes)
			{
				if (lastWriter.count(resource) != 0U)
				{
					dependencies[current].insert(lastWriter[resource]);
				}
				for (auto* reader : readersSinceLastWrite[resource])
				{
					if (reader != current)
					{
						dependencies[current].insert(reader);
					}
				}
				lastWriter[resource] = current;
				readersSinceLastWrite[resource].clear();
			}
		}

		std::unordered_map<RenderPassDescriptor*,
						   std::unordered_set<RenderPassDescriptor*>>
			successors;
		std::unordered_map<RenderPassDescriptor*, int> inDegree;

		for (const auto& access : passAccessList)
		{
			inDegree[access.pass] = 0;
		}

		for (const auto& [pass, deps] : dependencies)
		{
			for (auto* dependency : deps)
			{
				successors[dependency].insert(pass);
				inDegree[pass]++;
			}
		}

		return {dependencies, successors, inDegree};
	}

	auto RenderGraph::performTopologicalSort(
		const std::vector<PassAccess>& passAccessList,
		const std::unordered_map<RenderPassDescriptor*,
								 std::unordered_set<RenderPassDescriptor*>>& successors,
		std::unordered_map<RenderPassDescriptor*, int>& inDegree)
		-> std::vector<RenderPassDescriptor*>
	{
		std::queue<RenderPassDescriptor*> queue;
		std::vector<RenderPassDescriptor*> sortedOrder;

		for (const auto& access : passAccessList)
		{
			if (inDegree[access.pass] == 0)
			{
				queue.push(access.pass);
			}
		}

		while (!queue.empty())
		{
			auto* pass = queue.front();
			queue.pop();
			sortedOrder.push_back(pass);

			for (auto* successor : successors.at(pass))
			{
				if (--inDegree[successor] == 0)
				{
					queue.push(successor);
				}
			}
		}

		return sortedOrder;
	}

	auto RenderGraph::_cullPasses(const std::vector<struct PassAccess>& passAccessList)
		-> int
	{
		if (passAccessList.empty())
		{
			return 0;
		}

		std::unordered_set<RenderPassDescriptor*> livePasses;
		std::unordered_set<std::string> requiredResources = requiredOutputs;

		// Build quick-access maps for pass reads/writes
		auto [passReads, passWrites] = buildPassAccessMaps(passAccessList);

		// Mark live passes and update required resources
		_markLivePasses(livePasses, requiredResources, passReads, passWrites);

		// Rebuild execution order with only live passes
		int purged = _rebuildExecutionOrder(livePasses);

		// Check for unresolved resource dependencies
		checkUnresolvedResources(requiredResources);

		return purged;
	}

	auto RenderGraph::buildPassAccessMaps(const std::vector<PassAccess>& passAccessList)
		-> std::pair<
			std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>,
			std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>>
	{
		std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>
			passReads;
		std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>
			passWrites;

		for (const auto& access : passAccessList)
		{
			passReads[access.pass] = access.reads;
			passWrites[access.pass] = access.writes;
		}

		return {passReads, passWrites};
	}

	void RenderGraph::_markLivePasses(
		std::unordered_set<RenderPassDescriptor*>& livePasses,
		std::unordered_set<std::string>& requiredResources,
		const std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>&
			passReads,
		const std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>&
			passWrites)
	{
		for (auto it = execOrder.rbegin(); it != execOrder.rend(); ++it)
		{
			RenderPassDescriptor* pass = *it;

			if (isPassLive(pass, requiredResources, passWrites))
			{
				livePasses.insert(pass);
				updateRequiredResources(pass, requiredResources, passReads, passWrites);
			}
		}
	}

	auto RenderGraph::isPassLive(
		RenderPassDescriptor* pass,
		const std::unordered_set<std::string>& requiredResources,
		const std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>&
			passWrites) -> bool
	{
		if (pass->pass->isRequired())
		{
			return true;
		}

		if (std::any_of(
				passWrites.at(pass).begin(), passWrites.at(pass).end(),
				[&requiredResources](const std::string& resource)
				{ return requiredResources.find(resource) != requiredResources.end(); }))
		{
			return true;
		}

		return false;
	}

	void RenderGraph::updateRequiredResources(
		RenderPassDescriptor* pass, std::unordered_set<std::string>& requiredResources,
		const std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>&
			passReads,
		const std::unordered_map<RenderPassDescriptor*, std::unordered_set<std::string>>&
			passWrites)
	{
		for (const auto& resource : passReads.at(pass))
		{
			requiredResources.insert(resource);
		}

		for (const auto& resource : passWrites.at(pass))
		{
			requiredResources.erase(resource);
		}
	}

	auto RenderGraph::_rebuildExecutionOrder(
		const std::unordered_set<RenderPassDescriptor*>& livePasses) -> int
	{
		std::vector<RenderPassDescriptor*> newOrder;

		for (auto* pass : execOrder)
		{
			if (livePasses.find(pass) != livePasses.end())
			{
				newOrder.push_back(pass);
			}
		}

		int purged = static_cast<int>(execOrder.size() - newOrder.size());
		execOrder = std::move(newOrder);

		return purged;
	}

	void RenderGraph::checkUnresolvedResources(
		const std::unordered_set<std::string>& requiredResources)
	{
		if (!requiredResources.empty())
		{
			std::string error;
			for (const auto& resource : requiredResources)
			{
				error += " " + resource;
			}

			log_error("unproduced resources detected:%s", error.c_str());
		}
	}
}