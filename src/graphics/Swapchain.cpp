#include "graphics/Swapchain.h"
#include "graphics/vulkan/VkSwapchain.h"

namespace graphics
{
	auto Swapchain::create(platform::Window* window) -> std::unique_ptr<Swapchain>
	{
        return vk::VkSwapchain::create(window);
    }
}