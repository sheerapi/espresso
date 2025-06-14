#include "core/Application.h"
#include "SDL2/SDL_events.h"
#include "core/log.h"
#include "SDL2/SDL.h"
#include "graphics/RenderThread.h"
#include "platform/EventHandler.h"
#include "platform/ThreadManager.h"

namespace core
{
	Application* Application::main{nullptr};

	Application::Application()
    {
        if (main != nullptr)
        {
            log_error("an application instance already exists");
        }

        main = this;

        platform::ThreadManager::addThread<graphics::RenderThread>();
    }

    auto Application::run() -> bool
    {
        window->create();
		platform::ThreadManager::run();

		SDL_Event e;
        while (window->isRunning())
        {
            SDL_PollEvent(&e);
            internals::handleEvent(e);
        }

        return true;
    }

    Application::~Application()
    {
        shutdown();
		platform::ThreadManager::shutdown();
		SDL_DestroyWindow((SDL_Window*)window->getWindowHandle());
        SDL_Quit();
		log_warn("bye bye!");
	}

	auto Application::getName() -> std::string
    {
        return appName;
    }

    void Application::setup()
    {
		if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER |
					 SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK) < 0)
		{
			log_error("failed to initialize platform libraries");
		}

		if (window == nullptr)
		{
			window = std::unique_ptr<platform::Window>(new platform::Window(appName));
		}
	}

    auto Application::getWindow() -> platform::Window*
    {
        return window.get();
    }
}