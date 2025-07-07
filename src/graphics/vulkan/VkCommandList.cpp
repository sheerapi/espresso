#include "graphics/vulkan/VkCommandList.h"
#include "graphics/vulkan/VkGraphicContext.h"

namespace graphics::vk
{
	VkCommandList::VkCommandList(VkGraphicDevice* device)
	{
		this->device = device;

		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex =
			device->getFamilyIndices().graphicsFamily.value();

		es_vkCall(vkCreateCommandPool(device->getDevice(), &commandPoolInfo, nullptr,
									  &cmdPool));

		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.pNext = nullptr;
		cmdAllocInfo.commandPool = cmdPool;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		es_vkCall(vkAllocateCommandBuffers(device->getDevice(), &cmdAllocInfo, &buffer));
	}

	VkCommandList::~VkCommandList()
	{
		vkDestroyCommandPool(device->getDevice(), cmdPool, nullptr);
	}
}