#pragma once
#include "platform/Window.h"
#include <memory>

namespace graphics
{
	class GraphicContext;

	class Swapchain
	{
	public:
		virtual ~Swapchain() = default;

		virtual void init(platform::Window* window) {};
		virtual void recreate() {};
		virtual auto getFrameCount() -> int
		{
			return 0;
		}

		static auto create(platform::Window* window, GraphicContext* context)
			-> std::unique_ptr<Swapchain>;
	};
}