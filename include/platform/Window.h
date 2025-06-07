#pragma once
#include <string>
#include "SDL2/SDL_video.h"

namespace core
{
    class Window
    {
    public:
        Window(const std::string& title, int width = 1600, int height = 900);
        void create();

        ~Window();

        auto isRunning() const -> bool;

        void setTitle(const std::string& title);
        void setSize(int width, int height);
        void setMinSize(int width, int height);
        void setMaxSize(int width, int height);

        void toggleFullscreen();
        void toggleVsync();

        void close();

    private:
        std::string _title;
        int _width;
        int _height;
        bool _vsync{true};
        bool _fullscreen{false};
        bool _running;

        SDL_Window* _window;
    };
}