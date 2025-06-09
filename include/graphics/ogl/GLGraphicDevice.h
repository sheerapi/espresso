#pragma once
#include "../GraphicDevice.h"

namespace graphics::gl
{
    class GraphicDeviceGL : public graphics::GraphicDevice
    {
    public:
        ~GraphicDeviceGL() override{};
		void init(void* window) override{};
		void beginFrame() override{};
		void endFrame() override{};
        void submit() override{};
	};
}