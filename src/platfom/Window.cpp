#include "platform/Window.h"
#include "core/log.h"

namespace core
{
	Window::Window(const std::string& title, int width, int height)
	{
		_title = title;
		_width = width;
		_height = height;
	}

	void Window::create()
	{
		_window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_UNDEFINED,
								   SDL_WINDOWPOS_UNDEFINED, _width, _height,
								   SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        if (_window == nullptr)
        {
            log_error("failed to create window %s", _title.c_str());
            return;
        }

        _running = true;
	}

    Window::~Window()
    {
        SDL_DestroyWindow(_window);
    }

    auto Window::isRunning() const -> bool
    {
        return _running;
    }
}