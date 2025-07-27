#pragma once
#include "core/jobs/FrameMemoryPool.h"
#include "platform/EnvironmentInfo.h"
#include "platform/Thread.h"
#include "platform/Window.h"
#include <chrono>
#include <memory>
#include <string>

namespace core
{
	namespace internals
	{
		inline static std::chrono::milliseconds startTime{
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch())};
	}

	inline bool verbose{false};
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
		auto getEnvironmentInfo() -> platform::EnvironmentInfo*
		{
			return envInfo.get();
		}

		auto getFrameMemoryPool() -> jobs::FrameMemoryPool*
		{
			return frameMemoryPool.get();
		}

		[[nodiscard]] auto hasInit() const -> bool;

	protected:
		std::unique_ptr<jobs::FrameMemoryPool> frameMemoryPool;
		std::unique_ptr<platform::Window> window;
		platform::ApplicationInfo appInfo;

		std::unique_ptr<platform::EnvironmentInfo> envInfo;

		// this is way too heavy for it to be plainly on the App definition
		// it irks me to see that its around ~700 bytes, and i mean, it still is
		// but at least the memory is stored somewhere else, not here

	private:
		bool _init{false};
	};
}