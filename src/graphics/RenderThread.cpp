#include "graphics/RenderThread.h"
#include "core/Application.h"

namespace graphics
{
    void RenderThread::init()
    {
		_device = core::Application::main->getWindow()->getGraphicDevice();
        _device->makeCurrent();
	}

    void RenderThread::update()
    {
        _device->beginFrame();

        _device->endFrame();
        _device->submit();
    }

    void RenderThread::shutdown()
    {

    }
}