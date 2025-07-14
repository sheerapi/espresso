#pragma once
#include <memory>
#include <string>
#include <unordered_set>

namespace graphics
{
    class RenderPass;
    class RenderGraph;

    class RenderGraphBuilder
    {
    public:
		RenderGraphBuilder(RenderGraph* graph, RenderPass* pass)
			: graph(graph), currentPass(pass)
		{
		}

		void read(const std::string& res);
        void write(const std::string& res);

		auto getReads() const -> const std::unordered_set<std::string>&
		{
			return reads;
		}

		auto getWrites() const -> const std::unordered_set<std::string>&
		{
			return writes;
		}

		auto getCurrentPass() const -> RenderPass*
		{
			return currentPass;
		}

	protected:
        RenderGraph* graph;
        RenderPass* currentPass;
        std::unordered_set<std::string> reads;
        std::unordered_set<std::string> writes;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        virtual void setup(RenderGraphBuilder& builder) = 0;
        virtual void execute() = 0;
        [[nodiscard]] virtual auto isRequired() const -> bool {
            return false;
        }
    };

    struct RenderPassDescriptor
    {
    public:
        std::string name;
        std::unique_ptr<RenderPass> pass;

        virtual void build() {};
    };
}