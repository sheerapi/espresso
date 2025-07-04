#pragma once

#include <algorithm>
#include <cmath>
namespace math
{
	struct Vector2
	{
	public:
		float x;
		float y;

		Vector2() : x(0.0F), y(0.0F) {};
		Vector2(float x, float y) : x(x), y(y) {};

		auto operator+(const Vector2& other) const -> Vector2
		{
			return {x + other.x, y + other.y};
		}

		auto operator-(const Vector2& other) const -> Vector2
		{
			return {x - other.x, y - other.y};
		}

		auto operator*(const Vector2& other) const -> Vector2
		{
			return {x * other.x, y * other.y};
		}

		auto operator/(const Vector2& other) const -> Vector2
		{
			return {x / other.x, y / other.y};
		}

		auto operator+(float scalar) const -> Vector2
		{
			return {x + scalar, y + scalar};
		}

		auto operator-(float scalar) const -> Vector2
		{
			return {x - scalar, y - scalar};
		}

		auto operator*(float scalar) const -> Vector2
		{
			return {x * scalar, y * scalar};
		}

		auto operator/(float scalar) const -> Vector2
		{
			return {x / scalar, y / scalar};
		}

		auto operator==(const Vector2& other) const -> bool
		{
			return x == other.x && y == other.y;
		}

		auto operator!=(const Vector2& other) const -> bool
		{
			return !(*this == other);
		}

		auto operator<=(const Vector2& other) const -> bool
		{
			return x <= other.x && y <= other.y;
		}

		auto operator>=(const Vector2& other) const -> bool
		{
			return x >= other.x && y >= other.y;
		}

		[[nodiscard]] auto Negate() const -> Vector2
		{
			return {-x, -y};
		}

		[[nodiscard]] auto Min(const Vector2& b) const -> Vector2
		{
			return {std::min(x, b.x), std::min(y, b.y)};
		}

		[[nodiscard]] auto Max(const Vector2& b) const -> Vector2
		{
			return {std::max(x, b.x), std::max(y, b.y)};
		}

		[[nodiscard]] auto Absolute() const -> Vector2
		{
			return {std::abs(x), std::abs(y)};
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt(LengthSquared());
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (x * x) + (y * y);
		}

		[[nodiscard]] auto Normalized() const -> Vector2
		{
			float length = Length();
			return {x / length, y / length};
		}

		void Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
		}

		[[nodiscard]] auto Dot(const Vector2& other) const -> float
		{
			return (x * other.x) + (y * other.y);
		}

		[[nodiscard]] auto Angle(const Vector2& other) const -> float
		{
			return std::atan2(other.y, other.x) - std::atan2(y, x);
		}

		[[nodiscard]] auto Lerp(const Vector2& other, float t) const -> Vector2
		{
			return {x + ((other.x - x) * t), y + ((other.y - y) * t)};
		}

		[[nodiscard]] auto Distance(const Vector2& other) const -> float
		{
			return std::sqrt(DistanceSquared(other));
		}

		[[nodiscard]] auto DistanceSquared(const Vector2& other) const -> float
		{
			return ((x - other.x) * (x - other.x)) + ((y - other.y) * (y - other.y));
		}

		[[nodiscard]] auto Reflect(const Vector2& normal) const -> Vector2
		{
			return *this - (normal * (2.0F * Dot(normal)));
		}

		[[nodiscard]] auto Refract(const Vector2& other, float eta) const -> Vector2
		{
			float k = 1.0F - ((eta * eta) * (1.0F - (Dot(other) * Dot(other))));
			if (k < 0.0F)
			{
				return {0.0F, 0.0F};
			}
			return (*this * eta) - (other * ((eta * Dot(other)) + std::sqrt(k)));
		}

        static auto Zero() -> Vector2
        {
            return {0.0F, 0.0F};
        }

        static auto One() -> Vector2
        {
            return {1.0F, 1.0F};
        }

        static auto Up() -> Vector2
        {
            return {0.0F, 1.0F};
        }

        static auto Down() -> Vector2
        {
            return {0.0F, -1.0F};
        }

        static auto Left() -> Vector2
        {
            return {-1.0F, 0.0F};
        }

        static auto Right() -> Vector2
        {
            return {1.0F, 0.0F};
        }
	};
}