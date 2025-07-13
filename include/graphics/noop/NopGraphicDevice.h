#pragma once
#include "graphics/GraphicDevice.h"

namespace graphics::nop
{
    class NopGraphicDevice : public GraphicDevice
    {
    public:
        void init() override;
    };
}