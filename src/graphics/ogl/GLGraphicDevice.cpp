#include "graphics/ogl/GLGraphicDevice.h"
#include "SDL_video.h"

namespace graphics::gl
{
	void GraphicDeviceGL::init(void* window)
    {
        _window = (platform::Window*)window;
		_context = SDL_GL_CreateContext((SDL_Window*)_window->getWindowHandle());
	}

	void GraphicDeviceGL::beginFrame()
	{
        
    }

	void GraphicDeviceGL::endFrame()
	{

    }

	void GraphicDeviceGL::submit()
	{
        SDL_GL_SwapWindow((SDL_Window*)_window->getWindowHandle());
    }

	void GraphicDeviceGL::makeCurrent()
	{
		SDL_GL_MakeCurrent((SDL_Window*)_window->getWindowHandle(), _context);
	}

	auto GraphicDeviceGL::getBackend() -> uint32_t
	{
		return 0x00000002; // SDL_WINDOW_OPENGL
	}

    GraphicDeviceGL::~GraphicDeviceGL()
    {
        SDL_GL_DeleteContext(_context);
    }
}