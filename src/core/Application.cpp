#include "core/Application.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "components/core/LuaScriptEngine.h"
#include "core/TickThread.h"
#include "core/jobs/JobManager.h"
#include "core/log.h"
#include "graphics/RenderThread.h"
#include "platform/AssetManager.h"
#include "platform/EventHandler.h"
#include "platform/JobScheduler.h"
#include "platform/ThreadManager.h"
#include "platform/assets/LuaScript.h"
#include "utils/PerformanceTimer.h"
#include <ctime>
#include <filesystem>
#include <fstream>
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

		envInfo = std::make_unique<platform::EnvironmentInfo>();
		frameMemoryPool = std::make_unique<jobs::FrameMemoryPool>();
		jobs::JobManager::initialize();

		AssetManager::registerProcessor<platform::LuaScriptProcessor>();
		LuaScriptEngine::init();

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
			frameMemoryPool->reset();
			jobs::JobManager::beginFrame();

			if (SDL_PollEvent(&e) == 1)
			{
				::internals::handleEvent(e);
			}

			jobs::JobManager::endFrame();
			core::time.endMeasure();
		}

		return true;
	}

	Application::~Application()
	{
		shutdown();
		platform::ThreadManager::shutdown();
		::internals::JobScheduler::shutdown();
		jobs::JobManager::shutdown();
		LuaScriptEngine::shutdown();
		window.reset();
		SDL_Quit();
		log_warn("bye bye!");
	}

	void Application::setup()
	{
		es_stopwatchNamed("presentation setup");

		appInfo = getAppInfo();

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
			window = std::make_unique<platform::Window>(getName());
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

	void Application::generateCrashReport(const std::string& signalType, int code)
	{
		auto timestamp = std::time(nullptr);
		auto* date = localtime(&timestamp);

		char name[24];
		std::strftime(name, sizeof(name), "%d-%m-%y-%H.%M.txt", date);

		std::ofstream crash(std::filesystem::path(envInfo->paths.logPath) / name);
	}
}