#include "graphics/ogl/GLGraphicDevice.h"
#include "SDL_video.h"
#include "core/log.h"
#include "glad/gl.h"

namespace graphics::gl
{
	void GraphicDeviceGL::init()
    {
		_context = SDL_GL_CreateContext((SDL_Window*)_window->getWindowHandle());

        if (_context == nullptr)
        {
            log_error("failed to get opengl context: %s", SDL_GetError());
            return;
        }

		gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

        int major;
        int minor;

        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        log_trace("using opengl %d.%d", major, minor);
	}

	void GraphicDeviceGL::beginFrame()
	{
        
    }

	void GraphicDeviceGL::endFrame()
	{

    }

	void GraphicDeviceGL::submit()
	{
		glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
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

    void GraphicDeviceGL::setup(void* window)
    {
        _window = (platform::Window*)window;
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	}
}