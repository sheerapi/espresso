#pragma once

namespace graphics
{
    class GraphicDevice
    {
    public:
        virtual ~GraphicDevice() = default;

        virtual void init() {};
        virtual void beginFrame() {};
        virtual void endFrame() {};
        virtual void submit() {};
    };
}