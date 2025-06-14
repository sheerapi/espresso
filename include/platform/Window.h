#pragma once
#include "SDL2/SDL_video.h"
#include <string>

namespace platform
{
	class Window
	{
	public:
		Window(const std::string& title, int width = 960, int height = 540);
		~Window();
		
		void create();

		auto isRunning() const -> bool;

		void setTitle(const std::string& title);
		void setSize(int width, int height);
		void setMinSize(int width, int height);
		void setMaxSize(int width, int height);

		void toggleFullscreen();
		void toggleVsync();

		void close();

		void handleEvent(void* event);

		auto getWindowHandle() -> void*;

	private:
		std::string _title;
		int _width;
		int _height;
		int _xPos;
		int _yPos;
		bool _vsync{true};
		bool _fullscreen{false};
		bool _running;

		SDL_Window* _window;
	};
}