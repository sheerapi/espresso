#include "graphics/vulkan/VkGraphicDevice.h"
#include "graphics/vulkan/VkGraphicContext.h"
#include <set>
#include <vector>

namespace graphics::vk
{
	VkGraphicDevice::VkGraphicDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		physicalDevice = device;
		indices = findQueueFamilies(physicalDevice, surface);

		auto queueCreateInfos = _getQueueCreateInfos();

		VkPhysicalDeviceFeatures deviceFeatures{};

		{
			// create device
			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.queueCreateInfoCount = queueCreateInfos.size();

			createInfo.pEnabledFeatures = &deviceFeatures;

			std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			es_vkCall(
				vkCreateDevice(physicalDevice, &createInfo, nullptr, &this->device));
		}

		// get vkQueues
		vkGetDeviceQueue(this->device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(this->device, indices.presentFamily.value(), 0, &presentQueue);

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		volkLoadDevice(this->device);

		log_info("initialized device %s (vulkan version %d.%d)",
				 deviceProperties.deviceName,
				 VK_VERSION_MAJOR(deviceProperties.apiVersion),
				 VK_VERSION_MINOR(deviceProperties.apiVersion));
	}

	VkGraphicDevice::~VkGraphicDevice()
	{
		if (device == nullptr)
		{
			return;
		}

		vkDeviceWaitIdle(device);
		vkDestroyDevice(device, nullptr);
	}

	auto VkGraphicDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
		-> bool
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		QueueFamilyIndices indices = findQueueFamilies(device, surface);

		bool extensionsSupported =
			checkDeviceExtensionSupport(device, {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

		bool swapChainAdequate = true;
		/* if (extensionsSupported)
		{
			auto swapChainSupport = VkSwapchain::querySwapChainSupport(device, surface);
			swapChainAdequate = !swapChainSupport.formats.empty() &&
								!swapChainSupport.presentModes.empty();
		} */

		return indices.isComplete() && extensionsSupported && swapChainAdequate;
	}

	auto VkGraphicDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
		-> QueueFamilyIndices
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
												 queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0U)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = 0U;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport != 0U)
			{
				indices.presentFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	auto VkGraphicDevice::_getQueueCreateInfos() -> std::vector<VkDeviceQueueCreateInfo>
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
												  indices.presentFamily.value()};

		float queuePriority = 1.0F;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		return queueCreateInfos;
	}

	auto VkGraphicDevice::checkDeviceExtensionSupport(
		VkPhysicalDevice device, const std::vector<const char*>& extensions) -> bool
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
											 availableExtensions.data());

		std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}