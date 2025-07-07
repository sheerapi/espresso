#pragma once
#include "../Shader.h"
#include "glad/volk.h"

namespace graphics::vk
{
	class VkShaderSource : public ShaderSource
	{
	public:
		static auto create(const char* data, ShaderType type)
			-> std::unique_ptr<VkShaderSource>;

    protected:
		VkShaderModule module;
	};

	class VkShader : public Shader
	{
	public:
		void compile() override;
		void attach(std::unique_ptr<ShaderSource> source) override;
	};
}