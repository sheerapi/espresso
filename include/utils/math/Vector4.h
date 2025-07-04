#pragma once
#include "utils/math/Vector3.h"
namespace math
{
	struct Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4() : x(0.0F), y(0.0F), z(0.0F), w(1.0F) {};
		Vector4(float x, float y, float z, float w = 0.0F) : x(x), y(y), z(z), w(w) {};
		Vector4(const Vector3& vec3, float w = 1.0F) : x(vec3.x), y(vec3.y), z(vec3.z), w(w) {};
		Vector4(const Vector2& vec2, float z = 0.0F, float w = 1.0F) : x(vec2.x), y(vec2.y), z(z), w(w) {};

		auto operator+(const Vector4& other) const -> Vector4
		{
			return {x + other.x, y + other.y, z + other.z, w + other.w};
		}

		auto operator-(const Vector4& other) const -> Vector4
		{
			return {x - other.x, y - other.y, z - other.z, w - other.w};
		}

		auto operator*(const Vector4& other) const -> Vector4
		{
			return {x * other.x, y * other.y, z * other.z, w * other.w};
		}

		auto operator/(const Vector4& other) const -> Vector4
		{
			return {x / other.x, y / other.y, z / other.z, w / other.w};
		}

		auto operator+(float scalar) const -> Vector4
		{
			return {x + scalar, y + scalar, z + scalar, w + scalar};
		}

		auto operator-(float scalar) const -> Vector4
		{
			return {x - scalar, y - scalar, z - scalar, w - scalar};
		}

		auto operator*(float scalar) const -> Vector4
		{
			return {x * scalar, y * scalar, z * scalar, w * scalar};
		}

		auto operator/(float scalar) const -> Vector4
		{
			return {x / scalar, y / scalar, z / scalar, w / scalar};
		}

		auto operator==(const Vector4& other) const -> bool
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		auto operator!=(const Vector4& other) const -> bool
		{
			return !(*this == other);
		}

		auto operator<=(const Vector4& other) const -> bool
		{
			return x <= other.x && y <= other.y && z <= other.z && w <= other.w;
		}

		auto operator>=(const Vector4& other) const -> bool
		{
			return x >= other.x && y >= other.y && z >= other.z && w >= other.w;
		}

		[[nodiscard]] auto Negate() const -> Vector4
		{
			return {-x, -y, -z, -w};
		}

		[[nodiscard]] auto Min(const Vector4& b) const -> Vector4
		{
			return {std::min(x, b.x), std::min(y, b.y), std::min(z, b.z), std::min(w, b.w)};
		}

		[[nodiscard]] auto Max(const Vector4& b) const -> Vector4
		{
			return {std::max(x, b.x), std::max(y, b.y), std::max(z, b.z), std::max(w, b.w)};
		}

		[[nodiscard]] auto Absolute() const -> Vector4
		{
			return {std::abs(x), std::abs(y), std::abs(z), std::abs(w)};
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt(LengthSquared());
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (x * x) + (y * y) + (z * z) + (w * w);
		}

		[[nodiscard]] auto Normalized() const -> Vector4
		{
			float length = Length();
			return {x / length, y / length, z / length, w / length};
		}

		void Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}

		[[nodiscard]] auto Dot(const Vector4& other) const -> float
		{
			return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
		}

		[[nodiscard]] auto Cross(const Vector4& other) const -> Vector4
		{
			return {(y * other.z) - (z * other.y), (z * other.x) - (x * other.z),
					(x * other.y) - (y * other.x), 0.0F};
		}

		[[nodiscard]] auto Angle(const Vector4& other) const -> float
		{
			return std::acos(Dot(other) / (Length() * other.Length()));
		}

		[[nodiscard]] auto Lerp(const Vector4& other, float t) const -> Vector4
		{
			return {x + ((other.x - x) * t), y + ((other.y - y) * t),
					z + ((other.z - z) * t), w + ((other.w - w) * t)};
		}

		[[nodiscard]] auto Slerp(const Vector4& other, float t) const -> Vector4
		{
			return Lerp(other, t).Normalized();
		}

		[[nodiscard]] auto Distance(const Vector4& other) const -> float
		{
			return std::sqrt(DistanceSquared(other));
		}

		[[nodiscard]] auto DistanceSquared(const Vector4& other) const -> float
		{
			return ((x - other.x) * (x - other.x)) +
				   ((y - other.y) * (y - other.y)) +
				   ((z - other.z) * (z - other.z)) +
				   ((w - other.w) * (w - other.w));
		}

		[[nodiscard]] auto Reflect(const Vector4& normal) const -> Vector4
		{
			return *this - (normal * (2.0F * Dot(normal)));
		}

		[[nodiscard]] auto Refract(const Vector4& other, float eta) const -> Vector4
		{
			return {x * eta, y * eta, z * eta, w * eta};
		}

		[[nodiscard]] auto Rcp() const -> Vector4
		{
			return {1.0F / x, 1.0F / y, 1.0F / z, 1.0F / w};
		}

		[[nodiscard]] auto LengthInverse() const -> Vector4
		{
			return {1.0F / Length(), 1.0F / Length(), 1.0F / Length(), 1.0F / Length()};
		}

		[[nodiscard]] auto ToVector3() const -> Vector3
		{
			return {x, y, z};
		}

		[[nodiscard]] auto ToVector2() const -> Vector2
		{
			return {x, y};
		}

		static auto zero() -> Vector4
		{
			return {0.0F, 0.0F, 0.0F};
		}

		static auto One() -> Vector4
		{
			return {1.0F, 1.0F, 1.0F};
		}

		static auto Up() -> Vector4
		{
			return {0.0F, 1.0F, 0.0F};
		}

		static auto Down() -> Vector4
		{
			return {0.0F, -1.0F, 0.0F};
		}

		static auto Left() -> Vector4
		{
			return {-1.0F, 0.0F, 0.0F};
		}

		static auto Right() -> Vector4
		{
			return {1.0F, 0.0F, 0.0F};
		}

		static auto Forward() -> Vector4
		{
			return {0.0F, 0.0F, 1.0F};
		}

		static auto Backward() -> Vector4
		{
			return {0.0F, 0.0F, -1.0F};
		}
	};
}