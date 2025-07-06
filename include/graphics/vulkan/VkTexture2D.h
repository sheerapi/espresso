#pragma once
#include "glad/volk.h"
#include "graphics/Texture2D.h"

namespace graphics::vk
{
	class VkTexture2D : public Texture2D
	{
	public:
		VkTexture2D(VkDevice device, VkImageViewCreateInfo viewInfo, VkImage image);
		~VkTexture2D() override;

	protected:
		VkImage image;
		VkDevice device;
		VkImageView view;
		bool owned{true};
	};
}