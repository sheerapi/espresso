#pragma once
#include "graphics/GraphicResource.h"
#include "graphics/Shader.h"
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

		virtual auto genShaderSource(const char* source, ShaderType type)
			-> std::unique_ptr<ShaderSource>;
            
        virtual auto genShader() -> std::shared_ptr<Shader>;

	protected:
		std::unordered_map<unsigned int, std::shared_ptr<GraphicResource>> resources;
	};
}