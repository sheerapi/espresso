#include "graphics/Renderer.h"
#include "utils/Demangle.h"
#include "utils/PerformanceTimer.h"
#include "core/Application.h"
#include "core/Scene.h"

namespace graphics
{
    void Renderer::init()
    {
		context = std::unique_ptr<GraphicContext>(GraphicContext::getGraphicContext());

		GraphicContext::current = context.get();

		{
			es_stopwatchNamed(es_type(*context.get()));
			context->init(core::Application::main->getWindow());
		}
	}

    void Renderer::update()
    {
		context->makeCurrent();
		context->beginFrame();

		for (auto* camera : core::Scene::currentScene->getCameras())
		{
			camera->setViewportSize(core::Application::main->getWindow()->getSize());
			camera->render();
		}

		context->endFrame();
		context->swap();
	}

    void Renderer::shutdown()
    {
        context.reset();
    }
}