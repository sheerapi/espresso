#include "graphics/GraphicDevice.h"
#include "graphics/ogl/GLGraphicDevice.h"

namespace graphics
{
	auto GraphicDevice::getGraphicDevice() -> GraphicDevice*
    {
        return new gl::GraphicDeviceGL();
    }
}