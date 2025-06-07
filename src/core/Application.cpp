#include "core/Application.h"
#include "SDL2/SDL_events.h"
#include "core/log.h"
#include "SDL2/SDL.h"

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
    }

    auto Application::run() -> bool
    {
        window->create();

        SDL_Event e;
        while (window->isRunning())
        {
            SDL_PollEvent(&e);
        }

        return true;
    }

    Application::~Application()
    {
        shutdown();
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
			window = std::unique_ptr<Window>(new Window(appName));
		}
	}
}