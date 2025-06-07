#include "core/Application.h"
#include "core/log.h"

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
}