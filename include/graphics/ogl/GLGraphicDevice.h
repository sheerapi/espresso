#pragma once
#include "../GraphicDevice.h"
#include "platform/Window.h"

namespace graphics::gl
{
    class GraphicDeviceGL : public graphics::GraphicDevice
    {
    public:
        ~GraphicDeviceGL() override;

        void setup(void* window) override;
		void init() override;
		void beginFrame() override;
		void endFrame() override;
        void submit() override;

		void makeCurrent() override;

		auto getBackend() -> uint32_t override;

    private:
        platform::Window* _window{nullptr};
        void* _context{nullptr};
	};
}