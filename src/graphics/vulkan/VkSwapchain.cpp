#include "graphics/vulkan/VkSwapchain.h"
#include "SDL_vulkan.h"
#include "graphics/vulkan/VkGraphicContext.h"
#include "graphics/vulkan/VkGraphicDevice.h"
#include <algorithm>

namespace graphics::vk
{
	void VkSwapchain::init(platform::Window* window)
	{
		device = (VkGraphicDevice*)context->getDevice();
		_createSwapchain();
		_createRenderTargets();
	}

	VkSwapchain::~VkSwapchain()
	{
		vkDestroySwapchainKHR(device->getDevice(), swapchain, nullptr);
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
			vkCreateSwapchainKHR(device->getDevice(),
								 &createInfo, nullptr, &swapchain));
	}

	void VkSwapchain::_createRenderTargets()
	{
		uint32_t imageCount;

		std::vector<VkImage> swapChainImages;
		vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, swapChainImages.data());

		std::vector<VkImageView> swapChainImageViews(swapChainImages.size());
		for (size_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = format.format;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			renderTargets.push_back(device->createTexture(createInfo, swapChainImages[i]));
		}

		log_info("created swapchain (%dx%d in %d render targets)", size.width, size.height, imageCount, renderTargets.size());
	}

	void VkSwapchain::_setupQueueFamilies(VkSwapchainCreateInfoKHR& info)
	{
		device->getPhysicalDevice();
		context->getSurface();

		QueueFamilyIndices indices = VkGraphicDevice::findQueueFamilies(
			device->getPhysicalDevice(),
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
			info.queueFamilyIndexCount = 0;		// Optional
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

	auto VkSwapchain::getFrameCount() -> int
	{
		return (int)renderTargets.size();
	}
}