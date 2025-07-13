#include "graphics/noop/NopGraphicContext.h"

namespace graphics::nop
{
	void NopGraphicContext::init(platform::Window* window)
	{
	}

	auto NopGraphicContext::getBackend() -> uint32_t
	{
		return 0x00000002; // use SDL_WINDOW_OPENGL but don't really do much
	}

	void NopGraphicContext::beginFrame()
	{
	}

	void NopGraphicContext::endFrame()
	{
	}
}