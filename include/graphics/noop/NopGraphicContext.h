#pragma once
#include "graphics/GraphicContext.h"

namespace graphics::nop
{
    class NopGraphicContext : public GraphicContext
    {
    public:
        void init(platform::Window *window) override;
        auto getBackend() -> uint32_t override;
        void beginFrame() override;
        void endFrame() override;
    };
}