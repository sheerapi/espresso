#include "graphics/vulkan/VkSwapchain.h"
#include "SDL_vulkan.h"
#include "graphics/vulkan/VkGraphicContext.h"
#include "graphics/vulkan/VkGraphicDevice.h"
#include <algorithm>

namespace graphics::vk
{
	void VkSwapchain::init(platform::Window* window)
	{
		_createSwapchain();
	}

	VkSwapchain::~VkSwapchain()
	{
		vkDestroySwapchainKHR(((VkGraphicDevice*)context->getDevice())->getDevice(),
							  swapchain, nullptr);
	}

	void VkSwapchain::recreate()
	{
	}

    void VkSwapchain::_createSwapchain()
    {
		uint32_t imageCount = capabilities.minImageCount;
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = context->getSurface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;
		createInfo.imageExtent = size;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        _setupQueueFamilies(createInfo);

		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = swapchain;

		es_vkCall(
			vkCreateSwapchainKHR(((VkGraphicDevice*)context->getDevice())->getDevice(),
								 &createInfo, nullptr, &swapchain));
	}

	void VkSwapchain::_setupQueueFamilies(VkSwapchainCreateInfoKHR info)
    {
		QueueFamilyIndices indices = VkGraphicDevice::findQueueFamilies(
			((VkGraphicDevice*)context->getDevice())->getPhysicalDevice(),
			context->getSurface());
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
										 indices.presentFamily.value()};

		if (indices.graphicsFamily != indices.presentFamily)
		{
			info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount = 2;
			info.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = 0;			  // Optional
			info.pQueueFamilyIndices = nullptr; // Optional
		}
	}

	auto VkSwapchain::create(platform::Window* window, VkGraphicContext* context)
		-> std::unique_ptr<Swapchain>
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(
			((VkGraphicDevice*)context->getDevice())->getPhysicalDevice(),
			context->getSurface());

		VkSurfaceFormatKHR surfaceFormat =
			chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode =
			chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

		auto swapchain =
			std::make_unique<VkSwapchain>(surfaceFormat, presentMode, extent);
        swapchain->capabilities = swapChainSupport.capabilities;
        swapchain->context = context;
		swapchain->init(window);

		return swapchain;
	}

	auto VkSwapchain::chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	auto VkSwapchain::chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	auto VkSwapchain::chooseSwapExtent(platform::Window* window,
									   const VkSurfaceCapabilitiesKHR& capabilities)
		-> VkExtent2D
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		int width;
		int height;
		SDL_Vulkan_GetDrawableSize((SDL_Window*)window->getWindowHandle(), &width,
								   &height);

		VkExtent2D actualExtent = {static_cast<uint32_t>(width),
								   static_cast<uint32_t>(height)};

		actualExtent.width =
			std::clamp(actualExtent.width, capabilities.minImageExtent.width,
					   capabilities.maxImageExtent.width);
		actualExtent.height =
			std::clamp(actualExtent.height, capabilities.minImageExtent.height,
					   capabilities.maxImageExtent.height);

		return actualExtent;
	}

	auto VkSwapchain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
		-> SwapChainSupportDetails
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
												 details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
												  nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
													  details.presentModes.data());
		}

		return details;
	}
}