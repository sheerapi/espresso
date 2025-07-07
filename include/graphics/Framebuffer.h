#pragma once
#include "graphics/Texture2D.h"

namespace graphics
{
    class Framebuffer : public Texture2D
    {
    public:
        ~Framebuffer() override;
    };
}