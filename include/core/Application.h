#pragma once
#include <string>

namespace core
{
    class Application
    {
    public:
        static Application* main;

        Application();
        virtual ~Application();

        auto run() -> bool;

        virtual void init() {};
        virtual void update() {};
        virtual void shutdown() {};

        auto getName() -> std::string;

    protected:
        std::string appName = "Game";
    };
}