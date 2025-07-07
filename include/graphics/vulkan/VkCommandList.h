#pragma once
#include "graphics/CommandList.h"
#include "glad/volk.h"
#include "graphics/vulkan/VkGraphicDevice.h"

namespace graphics::vk
{
    class VkCommandList : public CommandList
    {
    public:
        VkCommandList(VkGraphicDevice* device);
        ~VkCommandList() override;

    protected:
        VkCommandPool cmdPool;
        VkCommandBuffer buffer;
        VkGraphicDevice* device;
    };
}