#include "graphics/RenderThread.h"
#include "core/Application.h"

namespace graphics
{
    void RenderThread::init()
    {
		_device = GraphicDevice::getGraphicDevice();
		_device->setup(core::Application::main->getWindow());
        _device->init();
	}

    void RenderThread::update()
    {
		_device->makeCurrent();
		_device->beginFrame();

		_device->endFrame();
		_device->submit();
	}

    void RenderThread::shutdown()
    {
        delete _device;
    }
}