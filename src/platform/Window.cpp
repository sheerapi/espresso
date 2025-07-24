#include "platform/Window.h"
#include "SDL_events.h"
#include "SDL_video.h"
#include "graphics/GraphicContext.h"
#include "core/EventManager.h"
#include "core/log.h"
#include "utils/PerformanceTimer.h"

namespace platform
{
	Window::Window(const std::string& title, int width, int height)
	{
		_title = title;
		_width = width;
		_height = height;
	}

	void Window::create()
	{
		es_stopwatchNamed("window creation");
		
		auto* testContext = graphics::GraphicContext::getGraphicContext();

		_window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_UNDEFINED,
								   SDL_WINDOWPOS_UNDEFINED, _width, _height,
								   SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI |
									   testContext->getBackend());

		delete testContext;

		SDL_SetWindowData(_window, "handle", this);
		
		if (_fullscreen)
		{
			SDL_SetWindowFullscreen(_window, _fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		}

		if (_window == nullptr)
		{
			log_error("failed to create window %s", _title.c_str());
			return;
		}

		_running = true;
	}

	Window::~Window()
	{
		close();
		SDL_DestroyWindow(_window);
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
	}

	void Window::handleEvent(void* event)
	{
		auto* sdlEvent = (SDL_Event*)event;

		switch ((SDL_WindowEventID)sdlEvent->window.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
			close();
			break;

		case SDL_WINDOWEVENT_SHOWN:
			core::EventManager::triggerEvent("window.shown");
			break;

		case SDL_WINDOWEVENT_HIDDEN:
			core::EventManager::triggerEvent("window.hidden");
			break;

		case SDL_WINDOWEVENT_MOVED:
			_xPos = sdlEvent->window.data1;
			_yPos = sdlEvent->window.data2;
			break;

		case SDL_WINDOWEVENT_RESIZED:
			_width = sdlEvent->window.data1;
			_height = sdlEvent->window.data2;
			core::EventManager::triggerEvent("window.resized");
			break;

		case SDL_WINDOWEVENT_SIZE_CHANGED:
			_width = sdlEvent->window.data1;
			_height = sdlEvent->window.data2;
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			core::EventManager::triggerEvent("window.minimized");
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
			core::EventManager::triggerEvent("window.maximized");
			break;

		case SDL_WINDOWEVENT_RESTORED:
			core::EventManager::triggerEvent("window.restored");
			break;

		case SDL_WINDOWEVENT_ENTER:
			core::EventManager::triggerEvent("mouse.enter");
			break;

		case SDL_WINDOWEVENT_LEAVE:
			core::EventManager::triggerEvent("mouse.leave");
			break;

		case SDL_WINDOWEVENT_FOCUS_GAINED:
			core::EventManager::triggerEvent("window.focused");
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			core::EventManager::triggerEvent("window.unfocused");
			break;

		case SDL_WINDOWEVENT_TAKE_FOCUS:
		default:
			break;
		}
	}

	auto Window::getWindowHandle() -> void*
	{
		return (void*)_window;
	}

	auto Window::getVsync() const -> bool
	{
		return _vsync;
	}

	auto Window::getSize() const -> math::Vector2
	{
		return {(float)_width, (float)_height};
	}

	auto Window::getPosition() const -> math::Vector2
	{
		return {(float)_xPos, (float)_yPos};
	}
}