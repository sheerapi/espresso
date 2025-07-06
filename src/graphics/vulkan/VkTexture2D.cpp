#include "graphics/vulkan/VkTexture2D.h"
#include "graphics/vulkan/VkGraphicContext.h"

namespace graphics::vk
{
	VkTexture2D::VkTexture2D(VkDevice device, VkImageViewCreateInfo viewInfo, VkImage image)
	{
		es_vkCall(
			vkCreateImageView(device, &viewInfo, nullptr, &this->view));
		this->image = image;
        this->device = device;
		owned = false;
	}

	VkTexture2D::~VkTexture2D()
    {
        vkDestroyImageView(device, view, nullptr);
    }
}