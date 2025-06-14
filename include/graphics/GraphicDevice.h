#pragma once
#include <cstdint>

namespace graphics
{
    class GraphicDevice
    {
    public:
        virtual ~GraphicDevice() = default;

        virtual void setup(void* window) {};
        virtual void init() = 0;
        virtual void beginFrame() {};
        virtual void endFrame() {};
        virtual void submit() {};
        virtual void setVsync(bool vsync) {};

        virtual void makeCurrent() {};

        virtual auto getBackend() -> uint32_t = 0;

        static auto getGraphicDevice() -> GraphicDevice*;
    };
}