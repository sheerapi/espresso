#pragma once

#include <algorithm>
#include <cmath>
namespace math
{
	struct Vector2
	{
	public:
		float X;
		float Y;

		Vector2() : X(0.0F), Y(0.0F) {};
		Vector2(float x, float y) : X(x), Y(y) {};

		auto operator+(const Vector2& other) const -> Vector2
		{
			return {X + other.X, Y + other.Y};
		}

		auto operator-(const Vector2& other) const -> Vector2
		{
			return {X - other.X, Y - other.Y};
		}

		auto operator*(const Vector2& other) const -> Vector2
		{
			return {X * other.X, Y * other.Y};
		}

		auto operator/(const Vector2& other) const -> Vector2
		{
			return {X / other.X, Y / other.Y};
		}

		auto operator+(float scalar) const -> Vector2
		{
			return {X + scalar, Y + scalar};
		}

		auto operator-(float scalar) const -> Vector2
		{
			return {X - scalar, Y - scalar};
		}

		auto operator*(float scalar) const -> Vector2
		{
			return {X * scalar, Y * scalar};
		}

		auto operator/(float scalar) const -> Vector2
		{
			return {X / scalar, Y / scalar};
		}

		auto operator==(const Vector2& other) const -> bool
		{
			return X == other.X && Y == other.Y;
		}

		auto operator!=(const Vector2& other) const -> bool
		{
			return !(*this == other);
		}

		auto operator<=(const Vector2& other) const -> bool
		{
			return X <= other.X && Y <= other.Y;
		}

		auto operator>=(const Vector2& other) const -> bool
		{
			return X >= other.X && Y >= other.Y;
		}

		[[nodiscard]] auto Negate() const -> Vector2
		{
			return {-X, -Y};
		}

		[[nodiscard]] auto Min(const Vector2& b) const -> Vector2
		{
			return {std::min(X, b.X), std::min(Y, b.Y)};
		}

		[[nodiscard]] auto Max(const Vector2& b) const -> Vector2
		{
			return {std::max(X, b.X), std::max(Y, b.Y)};
		}

		[[nodiscard]] auto Absolute() const -> Vector2
		{
			return {std::abs(X), std::abs(Y)};
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt(LengthSquared());
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (X * X) + (Y * Y);
		}

		[[nodiscard]] auto Normalized() const -> Vector2
		{
			float length = Length();
			return {X / length, Y / length};
		}

		void Normalize()
		{
			float length = Length();
			X /= length;
			Y /= length;
		}

		[[nodiscard]] auto Dot(const Vector2& other) const -> float
		{
			return (X * other.X) + (Y * other.Y);
		}

		[[nodiscard]] auto Angle(const Vector2& other) const -> float
		{
			return std::atan2(other.Y, other.X) - std::atan2(Y, X);
		}

		[[nodiscard]] auto Lerp(const Vector2& other, float t) const -> Vector2
		{
			return {X + ((other.X - X) * t), Y + ((other.Y - Y) * t)};
		}

		[[nodiscard]] auto Distance(const Vector2& other) const -> float
		{
			return std::sqrt(DistanceSquared(other));
		}

		[[nodiscard]] auto DistanceSquared(const Vector2& other) const -> float
		{
			return ((X - other.X) * (X - other.X)) + ((Y - other.Y) * (Y - other.Y));
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