#pragma once
#include "graphics/Swapchain.h"

namespace graphics::vk
{
    class VkSwapchain : public Swapchain
    {
    public:
        ~VkSwapchain();
        
		void init(platform::Window* window) override;
		void recreate() override;

		static auto create(platform::Window* window) -> std::unique_ptr<Swapchain>;
	};
}