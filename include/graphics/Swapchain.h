#pragma once
#include "platform/Window.h"
#include <memory>

namespace graphics
{
    class Swapchain
    {
    public:
        virtual void init(platform::Window* window) {};
        virtual void recreate() {};

        static auto create(platform::Window* window) -> std::unique_ptr<Swapchain>;
    };
}