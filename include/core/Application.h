#pragma once
#include "core/jobs/FrameMemoryPool.h"
#include "platform/EnvironmentInfo.h"
#include "platform/Thread.h"
#include "platform/Window.h"
#include <memory>
#include <string>

namespace core
{
	inline bool verbose{false};
	inline thread_local std::string threadName{"unknown"};
	inline thread_local platform::ThreadTime time;

	class Application
	{
	public:
		static Application* main;

		Application();
		virtual ~Application();

		void setup();

		auto run() -> bool;

		// the main difference between init and start is that
		// init runs before window creation and starts runs after everything is setup

		virtual void init() {};
		virtual void start() {};
		virtual void update() {};
		virtual void shutdown() {};

		virtual auto getAppInfo() -> platform::ApplicationInfo
		{
			return {};
		}

		auto getName() const -> std::string
		{
			return appInfo.name;
		}

		auto getId() const -> std::string
		{
			return appInfo.appId;
		}

		auto getWindow() -> platform::Window*;
		auto getEnvironmentInfo() -> platform::EnvironmentInfo&
		{
			return envInfo;
		}

		[[nodiscard]] auto hasInit() const -> bool;

	protected:
		std::unique_ptr<jobs::FrameMemoryPool> frameMemoryPool;
		std::unique_ptr<platform::Window> window;
		platform::EnvironmentInfo envInfo;
		platform::ApplicationInfo appInfo;

	private:
		bool _init{false};
	};
}