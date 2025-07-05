#pragma once
#include "../GraphicDevice.h"

namespace graphics::vk
{
    class VkGraphicDevice : public graphics::GraphicDevice
    {
    public:
        ~VkGraphicDevice() override;

		void init() override;
		void beginFrame() override;
		void endFrame() override;
        void submit() override;

    private:
        bool _vsync{false};
	};
}