#pragma once
#include <cstdint>

namespace graphics
{
    class GraphicDevice
    {
    public:
        virtual ~GraphicDevice() = default;

        virtual void init(void* window) = 0;
        virtual void beginFrame() {};
        virtual void endFrame() {};
        virtual void submit() {};

        virtual void makeCurrent() {};

        virtual auto getBackend() -> uint32_t = 0;

        static auto getGraphicDevice() -> GraphicDevice*;
    };
}