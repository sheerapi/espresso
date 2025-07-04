#pragma once
#include "utils/math/Vector2.h"
#include <algorithm>
#include <cmath>

namespace math
{
	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

		Vector3() : x(0.0F), y(0.0F), z(0.0F) {};
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
		Vector3(const Vector2& vec2, float z = 0.0F) : x(vec2.x), y(vec2.y), z(z) {};

		auto operator+(const Vector3& other) const -> Vector3
		{
			return {x + other.x, y + other.y, z + other.z};
		}

		auto operator-(const Vector3& other) const -> Vector3
		{
			return {x - other.x, y - other.y, z - other.z};
		}

		auto operator*(const Vector3& other) const -> Vector3
		{
			return {x * other.x, y * other.y, z * other.z};
		}

		auto operator/(const Vector3& other) const -> Vector3
		{
			return {x / other.x, y / other.y, z / other.z};
		}

		auto operator+(float scalar) const -> Vector3
		{
			return {x + scalar, y + scalar, z + scalar};
		}

		auto operator-(float scalar) const -> Vector3
		{
			return {x - scalar, y - scalar, z - scalar};
		}

		auto operator*(float scalar) const -> Vector3
		{
			return {x * scalar, y * scalar, z * scalar};
		}

		auto operator/(float scalar) const -> Vector3
		{
			return {x / scalar, y / scalar, z / scalar};
		}

		auto operator==(const Vector3& other) const -> bool
		{
			return x == other.x && y == other.y && z == other.z;
		}

		auto operator!=(const Vector3& other) const -> bool
		{
			return !(*this == other);
		}

		auto operator<=(const Vector3& other) const -> bool
		{
			return x <= other.x && y <= other.y && z <= other.z;
		}

		auto operator>=(const Vector3& other) const -> bool
		{
			return x >= other.x && y >= other.y && z >= other.z;
		}

		[[nodiscard]] auto Negate() const -> Vector3
		{
			return {-x, -y, -z};
		}

		[[nodiscard]] auto Min(const Vector3& b) const -> Vector3
		{
			return {std::min(x, b.x), std::min(y, b.y), std::min(z, b.z)};
		}

		[[nodiscard]] auto Max(const Vector3& b) const -> Vector3
		{
			return {std::max(x, b.x), std::max(y, b.y), std::max(z, b.z)};
		}

		[[nodiscard]] auto Absolute() const -> Vector3
		{
			return {std::abs(x), std::abs(y), std::abs(z)};
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt(LengthSquared());
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (x * x) + (y * y) + (z * z);
		}

		[[nodiscard]] auto Normalized() const -> Vector3
		{
			float length = Length();
			return {x / length, y / length, z / length};
		}

		void Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
			z /= length;
		}

		[[nodiscard]] auto Dot(const Vector3& other) const -> float
		{
			return (x * other.x) + (y * other.y) + (z * other.z);
		}

		[[nodiscard]] auto Cross(const Vector3& other) const -> Vector3
		{
			return {(y * other.z) - (z * other.y), (z * other.x) - (x * other.z),
					(x * other.y) - (y * other.x)};
		}

		[[nodiscard]] auto Angle(const Vector3& other) const -> float
		{
			return std::atan2(other.z, other.x) - std::atan2(z, x);
		}

		[[nodiscard]] auto Lerp(const Vector3& other, float t) const -> Vector3
		{
			return {x + ((other.x - x) * t), y + ((other.y - y) * t),
					z + ((other.z - z) * t)};
		}

		[[nodiscard]] auto Slerp(const Vector3& other, float t) const -> Vector3
		{
			return Lerp(other, t).Normalized();
		}

		[[nodiscard]] auto Distance(const Vector3& other) const -> float
		{
			return std::sqrt(DistanceSquared(other));
		}

		[[nodiscard]] auto DistanceSquared(const Vector3& other) const -> float
		{
			return ((x - other.x) * (x - other.x)) +
				   ((y - other.y) * (y - other.y)) +
				   ((z - other.z) * (z - other.z));
		}

		[[nodiscard]] auto Reflect(const Vector3& normal) const -> Vector3
		{
			return *this - (normal * (2.0F * Dot(normal)));
		}

		[[nodiscard]] auto Refract(const Vector3& other, float eta) const -> Vector3
		{
			return {x * eta, y * eta, z * eta};
		}

		[[nodiscard]] auto Rcp() const -> Vector3
		{
			return {1.0F / x, 1.0F / y, 1.0F / z};
		}

		[[nodiscard]] auto LengthInverse() const -> Vector3
		{
			return {1.0F / Length(), 1.0F / Length(), 1.0F / Length()};
		}

		[[nodiscard]] auto LengthInverseSquared() const -> Vector3
		{
			return {1.0F / LengthSquared(), 1.0F / LengthSquared(),
					1.0F / LengthSquared()};
		}

		static auto zero() -> Vector3
		{
			return {0.0F, 0.0F, 0.0F};
		}

		static auto One() -> Vector3
		{
			return {1.0F, 1.0F, 1.0F};
		}

		static auto Up() -> Vector3
		{
			return {0.0F, 1.0F, 0.0F};
		}

		static auto Down() -> Vector3
		{
			return {0.0F, -1.0F, 0.0F};
		}

		static auto Left() -> Vector3
		{
			return {-1.0F, 0.0F, 0.0F};
		}

		static auto Right() -> Vector3
		{
			return {1.0F, 0.0F, 0.0F};
		}

		static auto Forward() -> Vector3
		{
			return {0.0F, 0.0F, 1.0F};
		}

		static auto Backward() -> Vector3
		{
			return {0.0F, 0.0F, -1.0F};
		}
	};
}