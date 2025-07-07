#pragma once
#include "graphics/CommandList.h"
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

		virtual auto createCmdList() -> std::shared_ptr<CommandList>
		{
			return nullptr;
		}

	protected:
		std::unordered_map<unsigned int, std::shared_ptr<GraphicResource>> resources;
	};
}