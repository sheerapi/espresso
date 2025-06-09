#pragma once

namespace graphics
{
    class GraphicDevice
    {
    public:
        virtual ~GraphicDevice() = default;

        virtual void init(void* window) = 0;
        virtual void beginFrame() {};
        virtual void endFrame() {};
    };
}