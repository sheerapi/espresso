#include "graphics/GraphicContext.h"
#include "graphics/noop/NopGraphicContext.h"

namespace graphics
{
	auto GraphicContext::getGraphicContext() -> GraphicContext*
    {
        return new nop::NopGraphicContext();
    }
}