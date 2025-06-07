#pragma once
#include <string>

namespace core
{
    class Application
    {
    public:
        Application(const std::string& appName = "Game");
        virtual ~Application();

        auto run() -> bool;

        virtual void init() {};
        virtual void update() {};
        virtual void shutdown() {};

    private:
        std::string _appName;
    };
}