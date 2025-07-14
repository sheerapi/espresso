#pragma once
#include "RenderPass.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace graphics
{
	class RenderGraph
	{
	public:
		struct PassAccess
		{
			RenderPassDescriptor* pass;
			std::unordered_set<std::string> reads;
			std::unordered_set<std::string> writes;
		};

		virtual ~RenderGraph() = default;

		virtual void setup() {};

		void addPass(const std::string& name, RenderPass* pass);
		void removePass(const std::string& name);
		void setOutput(std::string& output);

		void compile();
		void execute();

	protected:
		std::vector<std::unique_ptr<RenderPassDescriptor>> passes;
		std::vector<RenderPassDescriptor*> execOrder;
		std::unordered_set<std::string> requiredOutputs;

	private:
		auto _collectPassAccess() -> std::vector<RenderGraph::PassAccess>;

		static auto performTopologicalSort(
			const std::vector<PassAccess>& passAccessList,
			const std::unordered_map<RenderPassDescriptor*,
									 std::unordered_set<RenderPassDescriptor*>>&
				successors,
			std::unordered_map<RenderPassDescriptor*, int>& inDegree)
			-> std::vector<RenderPassDescriptor*>;

		static auto buildDependencyGraph(const std::vector<PassAccess>& passAccessList)
			-> std::tuple<std::unordered_map<RenderPassDescriptor*,
											 std::unordered_set<RenderPassDescriptor*>>,
						  std::unordered_map<RenderPassDescriptor*,
											 std::unordered_set<RenderPassDescriptor*>>,
						  std::unordered_map<RenderPassDescriptor*, int>>;

		static auto buildPassAccessMaps(const std::vector<PassAccess>& passAccessList)
			-> std::pair<std::unordered_map<RenderPassDescriptor*,
											std::unordered_set<std::string>>,
						 std::unordered_map<RenderPassDescriptor*,
											std::unordered_set<std::string>>>;

		void _markLivePasses(
			std::unordered_set<RenderPassDescriptor*>& livePasses,
			std::unordered_set<std::string>& requiredResources,
			const std::unordered_map<RenderPassDescriptor*,
									 std::unordered_set<std::string>>& passReads,
			const std::unordered_map<RenderPassDescriptor*,
									 std::unordered_set<std::string>>& passWrites);

		static auto isPassLive(
			RenderPassDescriptor* pass,
			const std::unordered_set<std::string>& requiredResources,
			const std::unordered_map<RenderPassDescriptor*,
									 std::unordered_set<std::string>>& passWrites)
			-> bool;

		static void updateRequiredResources(
			RenderPassDescriptor* pass,
			std::unordered_set<std::string>& requiredResources,
			const std::unordered_map<RenderPassDescriptor*,
									 std::unordered_set<std::string>>& passReads,
			const std::unordered_map<RenderPassDescriptor*,
									 std::unordered_set<std::string>>& passWrites);

		static void checkUnresolvedResources(
			const std::unordered_set<std::string>& requiredResources);

		auto _cullPasses(const std::vector<struct PassAccess>& passAccessList) -> int;
		auto _rebuildExecutionOrder(
			const std::unordered_set<RenderPassDescriptor*>& livePasses) -> int;
	};
}