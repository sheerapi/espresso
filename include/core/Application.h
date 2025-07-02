#pragma once
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

		virtual void init() {};
		virtual void update() {};
		virtual void shutdown() {};

		auto getName() -> std::string;

		auto getWindow() -> platform::Window*;

		[[nodiscard]] auto hasInit() const -> bool;

	protected:
		std::string appName = "Game";
		std::unique_ptr<platform::Window> window;

	private:
		bool _init{false};
	};
}