#pragma once
#include "graphics/GraphicDevice.h"

namespace graphics::nop
{
    class NopGraphicDevice : public GraphicDevice
    {
    public:
        void init() override;
		auto createPassDescriptor(const std::string& name, RenderPass* pass)
			-> std::unique_ptr<RenderPassDescriptor> override;
	};
}