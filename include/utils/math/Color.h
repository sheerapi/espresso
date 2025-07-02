#pragma once
#include <algorithm>
#include <iomanip>
#include <string>

namespace math
{
	struct Color
	{
	public:
		float R{0};
		float G{0};
		float B{0};
		float A{255};

		Color() = default;
		Color(float red, float green, float blue, float alpha = 255)
			: R(std::clamp(red, 0.F, 255.F)), G(std::clamp(green, 0.F, 255.F)),
			  B(std::clamp(blue, 0.F, 255.F)), A(std::clamp(alpha, 0.F, 255.F)) {};

		auto operator==(const Color& other) const -> bool
		{
			return R == other.R && G == other.G && B == other.B && A == other.A;
		}

		auto operator^(float alpha) const -> Color*
		{
			return new Color(R, G, B, alpha * 255);
		}

		auto operator*(float scalar) const -> Color*
		{
			return new Color(R * scalar, G * scalar, B * scalar, A * scalar);
		}

		auto operator!=(const Color& other) const -> bool
		{
			return !(*this == other);
		}

		[[nodiscard]] auto ToHex() -> unsigned int
		{
			R = std::clamp(R, 0.F, 255.F);
			G = std::clamp(G, 0.F, 255.F);
			B = std::clamp(B, 0.F, 255.F);
			A = std::clamp(A, 0.F, 255.F);
			return ((int)A << 24) | ((int)R << 16) | ((int)G << 8) | (int)B;
		}

		[[nodiscard]] auto ToFloatArray() const -> float*
		{
			return new float[4]{(float)R / 255.0F, (float)G / 255.0F, (float)B / 255.0F,
								(float)A / 255.0F};
		}

		[[nodiscard]] auto ToIntArray() const -> float*
		{
			return new float[4]{R, G, B, A};
		}

		[[nodiscard]] static auto RandomColor() -> Color
		{
			return {static_cast<float>(static_cast<float>(rand()) /
									   static_cast<float>(RAND_MAX) * 255),
					static_cast<float>(static_cast<float>(rand()) /
									   static_cast<float>(RAND_MAX) * 255),
					static_cast<float>(static_cast<float>(rand()) /
									   static_cast<float>(RAND_MAX) * 255)};
		}

		[[nodiscard]] static auto FromHex(unsigned int hex) -> Color
		{
			return Color(
				static_cast<float>((hex >> 16) & 0xFF), // Red
				static_cast<float>((hex >> 8) & 0xFF),	// Green
				static_cast<float>(hex & 0xFF),			// Blue
				static_cast<float>(((hex & 0xFF000000) != 0U) ? ((hex >> 24) & 0xFF)
															  : 255) // Alpha
			);
		}

		[[nodiscard]] static auto FromFloatArray(const float* arr) -> Color
		{
			return {(float)(arr[0] * 255), (float)(arr[1] * 255), (float)(arr[2] * 255),
					(float)(arr[3] * 255)};
		}

		[[nodiscard]] static auto FromIntArray(float* arr) -> Color
		{
			return {arr[0], arr[1], arr[2], arr[3]};
		}

		[[nodiscard]] static auto FromString(const std::string& str) -> Color
		{
			if (str[0] == '#')
			{
				return FromHex(std::stoul("0x" + str.substr(1), nullptr, 16));
			}

			return {0, 0, 0, 0};
		}

		[[nodiscard]] auto ToString(bool includeAlpha = false) const -> std::string
		{
			std::stringstream ss;
			ss << "#" << std::hex << std::setw(2) << std::setfill('0') << ((int)R & 0xFF)
			   << std::hex << std::setw(2) << std::setfill('0') << ((int)G & 0xFF)
			   << std::hex << std::setw(2) << std::setfill('0') << ((int)B & 0xFF);

			if (includeAlpha)
			{
				ss << std::hex << std::setw(2) << std::setfill('0') << ((int)A & 0xFF);
			}
			return ss.str();
		}

		static auto White() -> Color
		{
			return {255, 255, 255};
		}
	};
}