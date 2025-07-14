#include "graphics/noop/NopGraphicContext.h"
#include "core/log.h"
#include "glad/gl.h"

namespace graphics::nop
{
	void NopGraphicContext::init(platform::Window* window)
	{
		this->window = window;
		_context = SDL_GL_CreateContext((SDL_Window*)window->getWindowHandle());

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

	auto NopGraphicContext::getBackend() -> uint32_t
	{
		return 0x00000002; // use SDL_WINDOW_OPENGL but don't really do much
	}

	void NopGraphicContext::beginFrame()
	{
		SDL_GL_MakeCurrent((SDL_Window*)window->getWindowHandle(), _context);
	}

	void NopGraphicContext::endFrame()
	{
		glFlush();
	}

	void NopGraphicContext::swap()
	{
		SDL_GL_SwapWindow((SDL_Window*)window->getWindowHandle());
	}
}