#pragma once
#include "../GraphicDevice.h"
#include "glad/volk.h"
#include <optional>
#include <vector>

namespace graphics::vk
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		[[nodiscard]] auto isComplete() const -> bool
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class VkGraphicDevice : public graphics::GraphicDevice
	{
	public:
		~VkGraphicDevice() override;

		VkGraphicDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkGraphicDevice() = default;

		static auto isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
			-> bool;
		static auto findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
			-> QueueFamilyIndices;

		static auto checkDeviceExtensionSupport(
			VkPhysicalDevice device, const std::vector<const char*>& extensions) -> bool;

		void beginFrame() override;
		void endFrame() override;
		void submit() override;

		auto getPhysicalDevice() -> VkPhysicalDevice
		{
			return physicalDevice;
		}

		auto getDevice() -> VkDevice
		{
			return device;
		}

		auto getFamilyIndices() -> QueueFamilyIndices
		{
			return indices;
		}

		auto getGraphicQueue() -> VkQueue
		{
			return graphicsQueue;
		}

		auto getPresentQueue() -> VkQueue
		{
			return presentQueue;
		}

	protected:
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		QueueFamilyIndices indices;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

	private:
		auto _getQueueCreateInfos() -> std::vector<VkDeviceQueueCreateInfo>;
	};
}