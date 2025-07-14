#pragma once
#include "graphics/RenderPass.h"
#include <memory>

namespace graphics
{
	class GraphicDevice
	{
	public:
		virtual ~GraphicDevice() = default;

		virtual void init() {};
		virtual auto createPassDescriptor(const std::string& name, RenderPass* pass) -> std::unique_ptr<RenderPassDescriptor>
		{
			return nullptr;
		}
	};
}