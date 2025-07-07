#pragma once
#include "graphics/GraphicResource.h"

namespace graphics
{
    class CommandList : public GraphicResource
    {
    public:
        ~CommandList() override = default;
    };
}