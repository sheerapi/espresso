#pragma once
#include "graphics/GraphicResource.h"

namespace graphics
{
	enum class TextureFormat : char
	{
		Rgba8Unorm,
		Rgba8sRGB,
		R8Unorm,
		Bgra8Unorm,
		Bgra8sRGB
	};

	struct Texture2D : public GraphicResource
	{
	public:
		~Texture2D() override = default;

		[[nodiscard]] auto getWidth() const -> int
		{
			return width;
		}

		[[nodiscard]] auto getHeight() const -> int
		{
			return height;
		}

		[[nodiscard]] auto getFormat() const -> TextureFormat
		{
			return format;
		}

	protected:
		unsigned short width;
		unsigned short height;
		TextureFormat format;
	};
}