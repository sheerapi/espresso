#include "graphics/RenderThread.h"
#include "core/Application.h"
#include "core/Scene.h"
#include "utils/Demangle.h"
#include "utils/PerformanceTimer.h"

namespace graphics
{
	void RenderThread::init()
	{
		core::threadName = "render";

		_device = GraphicDevice::getGraphicDevice();
		_device->setup(core::Application::main->getWindow());
		{
			es_stopwatchNamed(es_type(*_device));
			_device->init();
		}
	}

	void RenderThread::update()
	{
		_device->makeCurrent();
		_device->beginFrame();

		for (auto* camera : core::Scene::currentScene->getCameras())
		{
			camera->setViewportSize(core::Application::main->getWindow()->getSize());
			camera->render();
		}

		_device->endFrame();
		_device->submit();
	}

	void RenderThread::shutdown()
	{
		delete _device;
	}
}