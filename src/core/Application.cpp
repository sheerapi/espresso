#include "core/Application.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "core/TickThread.h"
#include "core/log.h"
#include "graphics/RenderThread.h"
#include "platform/AssetManager.h"
#include "platform/EventHandler.h"
#include "platform/ThreadManager.h"
#include "utils/PerformanceTimer.h"
#include "platform/JobScheduler.h"
#include <memory>

namespace core
{
	Application* Application::main{nullptr};

	Application::Application()
	{
		es_stopwatch();

		if (main != nullptr)
		{
			log_error("an application instance already exists");
		}

		main = this;

		::internals::JobScheduler::init();
		AssetManager::init();

		platform::ThreadManager::addThread<graphics::RenderThread>();
		platform::ThreadManager::addThread<core::TickThread>();
	}

	auto Application::run() -> bool
	{
		SDL_Event e;
		while (window->isRunning())
		{
			core::time.startMeasure();

			SDL_PollEvent(&e);
			::internals::handleEvent(e);

			core::time.endMeasure();
		}

		return true;
	}

	Application::~Application()
	{
		shutdown();
		platform::ThreadManager::shutdown();
		SDL_DestroyWindow((SDL_Window*)window->getWindowHandle());
		SDL_Quit();
		::internals::JobScheduler::shutdown();
		log_warn("bye bye!");
	}

	auto Application::getName() -> std::string
	{
		return appName;
	}

	void Application::setup()
	{
		es_stopwatchNamed("presentation setup");

		{
			es_stopwatchNamed("platform backend init");

			if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER |
						 SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK) <
				0)
			{
				log_error("failed to initialize platform libraries");
			}
		}

		if (window == nullptr)
		{
			window = std::make_unique<platform::Window>(appName);
		}

		window->create();
		platform::ThreadManager::run();

		_init = true;
	}

	auto Application::getWindow() -> platform::Window*
	{
		return window.get();
	}

	auto Application::hasInit() const -> bool
	{
		return _init;
	}
}