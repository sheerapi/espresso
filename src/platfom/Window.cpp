#include "platform/Window.h"
#include "SDL_video.h"
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

        SDL_SetWindowData(_window, "handle", this);
        SDL_SetWindowFullscreen(_window, _fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

        if (_window == nullptr)
        {
            log_error("failed to create window %s", _title.c_str());
            return;
        }

        _running = true;
	}

    Window::~Window()
    {
        if (_window != nullptr)
        {
			close();
		}
    }

    auto Window::isRunning() const -> bool
    {
        return _running;
    }

	void Window::setTitle(const std::string& title)
    {
        _title = title;
        SDL_SetWindowTitle(_window, _title.c_str());
    }

	void Window::setSize(int width, int height)
    {
        SDL_SetWindowSize(_window, width, height);
        SDL_GetWindowSize(_window, &_width, &_height);
    }

	void Window::setMinSize(int width, int height)
    {
		SDL_SetWindowMinimumSize(_window, width, height);
	}

	void Window::setMaxSize(int width, int height)
    {
        SDL_SetWindowMaximumSize(_window, width, height);
    }

	void Window::toggleFullscreen()
    {
        _fullscreen = !_fullscreen;
		SDL_SetWindowFullscreen(_window, _fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
	}

	void Window::toggleVsync()
    {
        _vsync = !_vsync; 
    }

    void Window::close()
    {
        _running = false;
		SDL_DestroyWindow(_window);
        _window = nullptr;
	}
}