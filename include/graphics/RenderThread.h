#pragma once
#include "graphics/GraphicContext.h"
#include "platform/Thread.h"

namespace graphics
{
    class RenderThread : public platform::Thread
    {
    public:
        void init() override;
        void shutdown() override;
        void update() override;

    private:
        GraphicContext* _context;
    };
}