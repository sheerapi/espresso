#include "graphics/RenderThread.h"
#include "core/Application.h"
#include "core/Scene.h"
#include "graphics/GraphicContext.h"
#include "utils/Demangle.h"
#include "utils/PerformanceTimer.h"

namespace graphics
{
	void RenderThread::init()
	{
		core::threadName = "render";

		_context = GraphicContext::getGraphicContext();
		{
			es_stopwatchNamed(es_type(*_context));
			_context->init(core::Application::main->getWindow());
		}
	}

	void RenderThread::update()
	{
		_context->makeCurrent();
		_context->getDevice()->beginFrame();

		for (auto* camera : core::Scene::currentScene->getCameras())
		{
			camera->setViewportSize(core::Application::main->getWindow()->getSize());
			camera->render();
		}

		_context->getDevice()->endFrame();
		_context->getDevice()->submit();
	}

	void RenderThread::shutdown()
	{
		delete _context;
	}
}