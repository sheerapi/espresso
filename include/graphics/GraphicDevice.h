#pragma once
#include "graphics/GraphicResource.h"
#include <memory>
#include <unordered_map>

namespace graphics
{
    class GraphicDevice
    {
    public:
        virtual ~GraphicDevice() = default;

        virtual void init() {};
        virtual void beginFrame() {};
        virtual void endFrame() {};
        virtual void submit() {};

    protected:
		std::unordered_map<unsigned int, std::shared_ptr<GraphicResource>> resources;
	};
}