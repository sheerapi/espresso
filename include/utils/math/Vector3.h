#pragma once
#include "utils/math/Vector2.h"
#include <algorithm>
#include <cmath>

namespace math
{
	struct Vector3
	{
	public:
		float X;
		float Y;
		float Z;

		Vector3() : X(0.0F), Y(0.0F), Z(0.0F) {};
		Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {};
		Vector3(const Vector2& vec2, float z = 0.0F) : X(vec2.X), Y(vec2.Y), Z(z) {};

		auto operator+(const Vector3& other) const -> Vector3
		{
			return {X + other.X, Y + other.Y, Z + other.Z};
		}

		auto operator-(const Vector3& other) const -> Vector3
		{
			return {X - other.X, Y - other.Y, Z - other.Z};
		}

		auto operator*(const Vector3& other) const -> Vector3
		{
			return {X * other.X, Y * other.Y, Z * other.Z};
		}

		auto operator/(const Vector3& other) const -> Vector3
		{
			return {X / other.X, Y / other.Y, Z / other.Z};
		}

		auto operator+(float scalar) const -> Vector3
		{
			return {X + scalar, Y + scalar, Z + scalar};
		}

		auto operator-(float scalar) const -> Vector3
		{
			return {X - scalar, Y - scalar, Z - scalar};
		}

		auto operator*(float scalar) const -> Vector3
		{
			return {X * scalar, Y * scalar, Z * scalar};
		}

		auto operator/(float scalar) const -> Vector3
		{
			return {X / scalar, Y / scalar, Z / scalar};
		}

		auto operator==(const Vector3& other) const -> bool
		{
			return X == other.X && Y == other.Y && Z == other.Z;
		}

		auto operator!=(const Vector3& other) const -> bool
		{
			return !(*this == other);
		}

		auto operator<=(const Vector3& other) const -> bool
		{
			return X <= other.X && Y <= other.Y && Z <= other.Z;
		}

		auto operator>=(const Vector3& other) const -> bool
		{
			return X >= other.X && Y >= other.Y && Z >= other.Z;
		}

		[[nodiscard]] auto Negate() const -> Vector3
		{
			return {-X, -Y, -Z};
		}

		[[nodiscard]] auto Min(const Vector3& b) const -> Vector3
		{
			return {std::min(X, b.X), std::min(Y, b.Y), std::min(Z, b.Z)};
		}

		[[nodiscard]] auto Max(const Vector3& b) const -> Vector3
		{
			return {std::max(X, b.X), std::max(Y, b.Y), std::max(Z, b.Z)};
		}

		[[nodiscard]] auto Absolute() const -> Vector3
		{
			return {std::abs(X), std::abs(Y), std::abs(Z)};
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt(LengthSquared());
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (X * X) + (Y * Y) + (Z * Z);
		}

		[[nodiscard]] auto Normalized() const -> Vector3
		{
			float length = Length();
			return {X / length, Y / length, Z / length};
		}

		void Normalize()
		{
			float length = Length();
			X /= length;
			Y /= length;
			Z /= length;
		}

		[[nodiscard]] auto Dot(const Vector3& other) const -> float
		{
			return (X * other.X) + (Y * other.Y) + (Z * other.Z);
		}

		[[nodiscard]] auto Cross(const Vector3& other) const -> Vector3
		{
			return {(Y * other.Z) - (Z * other.Y), (Z * other.X) - (X * other.Z),
					(X * other.Y) - (Y * other.X)};
		}

		[[nodiscard]] auto Angle(const Vector3& other) const -> float
		{
			return std::atan2(other.Z, other.X) - std::atan2(Z, X);
		}

		[[nodiscard]] auto Lerp(const Vector3& other, float t) const -> Vector3
		{
			return {X + ((other.X - X) * t), Y + ((other.Y - Y) * t),
					Z + ((other.Z - Z) * t)};
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
			return ((X - other.X) * (X - other.X)) +
				   ((Y - other.Y) * (Y - other.Y)) +
				   ((Z - other.Z) * (Z - other.Z));
		}

		[[nodiscard]] auto Reflect(const Vector3& normal) const -> Vector3
		{
			return *this - (normal * (2.0F * Dot(normal)));
		}

		[[nodiscard]] auto Refract(const Vector3& other, float eta) const -> Vector3
		{
			return {X * eta, Y * eta, Z * eta};
		}

		[[nodiscard]] auto Rcp() const -> Vector3
		{
			return {1.0F / X, 1.0F / Y, 1.0F / Z};
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

		static auto Zero() -> Vector3
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