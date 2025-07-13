#pragma once
#include "graphics/GraphicDevice.h"
#include "platform/Window.h"
#include <cassert>
#include <cstdint>
#include <memory>

namespace graphics
{
	class GraphicContext
	{
	public:
		inline static GraphicContext* current{nullptr};
		
		virtual ~GraphicContext() = default;

		virtual void init(platform::Window* window) = 0;
		virtual void makeCurrent() 
		{
			current = this;
		};
		virtual auto getBackend() -> uint32_t = 0;
		virtual void setVsync(bool vsync) {};

		virtual void beginFrame() {};
		virtual void endFrame() {};
		virtual void swap() {};

		auto getDevice() -> GraphicDevice*
		{
			return device.get();
		}

		static auto getGraphicContext() -> GraphicContext*;

	protected:
		std::unique_ptr<GraphicDevice> device;
		platform::Window* window;
	};
}