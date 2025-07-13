#pragma once
#include "graphics/GraphicDevice.h"
#include "platform/Window.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

namespace graphics
{
	class GraphicContext
	{
	public:
		virtual ~GraphicContext() = default;

		virtual void init(platform::Window* window) = 0;
		virtual void makeCurrent() {};
		virtual auto getBackend() -> uint32_t = 0;
		virtual void setVsync(bool vsync) {};

		auto getDevice() -> GraphicDevice*
		{
			return device.get();
		}

		static auto getGraphicContext() -> GraphicContext*;

	protected:
		std::unique_ptr<GraphicDevice> device;
	};
}