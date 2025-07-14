#include "graphics/noop/NopGraphicDevice.h"
#include <memory>

namespace graphics::nop
{
    void NopGraphicDevice::init()
    {
        
    }

	auto NopGraphicDevice::createPassDescriptor(const std::string& name, RenderPass* pass)
		-> std::unique_ptr<RenderPassDescriptor>
	{
        auto desc = std::make_unique<RenderPassDescriptor>();
        desc->name = name;
        desc->pass = std::unique_ptr<RenderPass>(pass);
        desc->build();

        return desc;
    }
}