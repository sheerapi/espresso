#include "graphics/RenderThread.h"
#include "core/Application.h"
#include "graphics/Renderer.h"

namespace graphics
{
	void RenderThread::init()
	{
		core::threadName = "render";
		Renderer::init();
	}

	void RenderThread::update()
	{
		Renderer::update();
	}

	void RenderThread::shutdown()
	{
		Renderer::shutdown();
	}
}