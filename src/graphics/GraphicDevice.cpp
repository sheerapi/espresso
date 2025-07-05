#include "graphics/GraphicContext.h"
#include "graphics/vulkan/VkGraphicContext.h"

namespace graphics
{
	auto GraphicContext::getGraphicContext() -> GraphicContext*
    {
        return new vk::VkGraphicContext();
    }
}