#pragma once
#include "graphics/GraphicContext.h"
#include "graphics/RenderGraph.h"
#include <memory>
#include <vector>

namespace graphics
{
    class Renderer
    {
    public:
    protected:
        static void init();
        static void shutdown();
        static void update();

    private:
        inline static std::unique_ptr<GraphicContext> context;
        inline static std::vector<std::unique_ptr<RenderGraph>> graphs;

        friend class RenderThread;
    };
}