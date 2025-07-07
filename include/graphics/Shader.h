#pragma once
#include "graphics/GraphicResource.h"
#include <memory>
#include <string>
#include <vector>

namespace graphics
{
	enum class ShaderType : char
	{
		Vertex,
		Fragment,
		Geometry,
		TessellationControl,
		TessellationEvaluation,
		Compute
	};

	class ShaderSource
	{
	public:
	protected:
		ShaderType type;
		std::string name;
	};

	class Shader : public GraphicResource
	{
	public:
		virtual void attach(std::unique_ptr<ShaderSource> source);
		virtual void compile() = 0;
		auto getTypeMask() -> ShaderType;

	protected:
		std::vector<std::unique_ptr<ShaderSource>> sources;
	};
}