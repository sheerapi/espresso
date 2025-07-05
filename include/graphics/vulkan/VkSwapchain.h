#pragma once
#include "glad/volk.h"
#include "graphics/Swapchain.h"
#include "graphics/vulkan/VkGraphicContext.h"
#include <vector>

namespace graphics::vk
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VkSwapchain : public Swapchain
	{
	public:
		VkSwapchain(VkSurfaceFormatKHR format, VkPresentModeKHR presentMode,
					VkExtent2D size)
			: format(format), presentMode(presentMode), size(size) {};

		~VkSwapchain() override;

		void init(platform::Window* window) override;
		void recreate() override;

		static auto create(platform::Window* window, VkGraphicContext* context)
			-> std::unique_ptr<Swapchain>;

		static auto querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
			-> SwapChainSupportDetails;

	protected:
		VkSurfaceFormatKHR format;
		VkPresentModeKHR presentMode;
		VkExtent2D size;
		VkSurfaceCapabilitiesKHR capabilities;
		VkGraphicContext* context;

		VkSwapchainKHR swapchain{nullptr};

	private:
		void _createSwapchain();
		void _setupQueueFamilies(VkSwapchainCreateInfoKHR info);

		static auto chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats)
			-> VkSurfaceFormatKHR;

		static auto chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes)
			-> VkPresentModeKHR;

		static auto chooseSwapExtent(platform::Window* window,
									 const VkSurfaceCapabilitiesKHR& capabilities)
			-> VkExtent2D;
	};
}