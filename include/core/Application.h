#pragma once
#include "core/jobs/FrameMemoryPool.h"
#ifdef EDITOR
#	include "editor/core/EditorContext.h"
#endif
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

		auto getName() -> std::string;

		auto getWindow() -> platform::Window*;

		[[nodiscard]] auto hasInit() const -> bool;

	protected:
		std::unique_ptr<jobs::FrameMemoryPool> frameMemoryPool;
		std::string appName = "Game";
		std::unique_ptr<platform::Window> window;

#ifdef EDITOR
		std::unique_ptr<editor::internals::EditorContext> editor;
#endif

	private:
		bool _init{false};
	};
}