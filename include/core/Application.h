#pragma once
#include "platform/Window.h"
#include <memory>
#include <string>

namespace core
{
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

        auto getWindow() -> Window*;

    protected:
        std::string appName = "Game";
        std::unique_ptr<Window> window;
    };
}