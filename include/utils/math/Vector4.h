#pragma once
#include "utils/math/Vector3.h"
namespace math
{
	struct Vector4
	{
	public:
		float X;
		float Y;
		float Z;
		float W;

		Vector4() : X(0.0F), Y(0.0F), Z(0.0F), W(1.0F) {};
		Vector4(float x, float y, float z, float w = 0.0F) : X(x), Y(y), Z(z), W(w) {};
		Vector4(const Vector3& vec3, float w = 1.0F) : X(vec3.X), Y(vec3.Y), Z(vec3.Z), W(w) {};
		Vector4(const Vector2& vec2, float z = 0.0F, float w = 1.0F) : X(vec2.X), Y(vec2.Y), Z(z), W(w) {};

		auto operator+(const Vector4& other) const -> Vector4
		{
			return {X + other.X, Y + other.Y, Z + other.Z, W + other.W};
		}

		auto operator-(const Vector4& other) const -> Vector4
		{
			return {X - other.X, Y - other.Y, Z - other.Z, W - other.W};
		}

		auto operator*(const Vector4& other) const -> Vector4
		{
			return {X * other.X, Y * other.Y, Z * other.Z, W * other.W};
		}

		auto operator/(const Vector4& other) const -> Vector4
		{
			return {X / other.X, Y / other.Y, Z / other.Z, W / other.W};
		}

		auto operator+(float scalar) const -> Vector4
		{
			return {X + scalar, Y + scalar, Z + scalar, W + scalar};
		}

		auto operator-(float scalar) const -> Vector4
		{
			return {X - scalar, Y - scalar, Z - scalar, W - scalar};
		}

		auto operator*(float scalar) const -> Vector4
		{
			return {X * scalar, Y * scalar, Z * scalar, W * scalar};
		}

		auto operator/(float scalar) const -> Vector4
		{
			return {X / scalar, Y / scalar, Z / scalar, W / scalar};
		}

		auto operator==(const Vector4& other) const -> bool
		{
			return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
		}

		auto operator!=(const Vector4& other) const -> bool
		{
			return !(*this == other);
		}

		auto operator<=(const Vector4& other) const -> bool
		{
			return X <= other.X && Y <= other.Y && Z <= other.Z && W <= other.W;
		}

		auto operator>=(const Vector4& other) const -> bool
		{
			return X >= other.X && Y >= other.Y && Z >= other.Z && W >= other.W;
		}

		[[nodiscard]] auto Negate() const -> Vector4
		{
			return {-X, -Y, -Z, -W};
		}

		[[nodiscard]] auto Min(const Vector4& b) const -> Vector4
		{
			return {std::min(X, b.X), std::min(Y, b.Y), std::min(Z, b.Z), std::min(W, b.W)};
		}

		[[nodiscard]] auto Max(const Vector4& b) const -> Vector4
		{
			return {std::max(X, b.X), std::max(Y, b.Y), std::max(Z, b.Z), std::max(W, b.W)};
		}

		[[nodiscard]] auto Absolute() const -> Vector4
		{
			return {std::abs(X), std::abs(Y), std::abs(Z), std::abs(W)};
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt(LengthSquared());
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (X * X) + (Y * Y) + (Z * Z) + (W * W);
		}

		[[nodiscard]] auto Normalized() const -> Vector4
		{
			float length = Length();
			return {X / length, Y / length, Z / length, W / length};
		}

		void Normalize()
		{
			float length = Length();
			X /= length;
			Y /= length;
			Z /= length;
			W /= length;
		}

		[[nodiscard]] auto Dot(const Vector4& other) const -> float
		{
			return (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W);
		}

		[[nodiscard]] auto Cross(const Vector4& other) const -> Vector4
		{
			return {(Y * other.Z) - (Z * other.Y), (Z * other.X) - (X * other.Z),
					(X * other.Y) - (Y * other.X), 0.0F};
		}

		[[nodiscard]] auto Angle(const Vector4& other) const -> float
		{
			return std::acos(Dot(other) / (Length() * other.Length()));
		}

		[[nodiscard]] auto Lerp(const Vector4& other, float t) const -> Vector4
		{
			return {X + ((other.X - X) * t), Y + ((other.Y - Y) * t),
					Z + ((other.Z - Z) * t), W + ((other.W - W) * t)};
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
			return ((X - other.X) * (X - other.X)) +
				   ((Y - other.Y) * (Y - other.Y)) +
				   ((Z - other.Z) * (Z - other.Z)) +
				   ((W - other.W) * (W - other.W));
		}

		[[nodiscard]] auto Reflect(const Vector4& normal) const -> Vector4
		{
			return *this - (normal * (2.0F * Dot(normal)));
		}

		[[nodiscard]] auto Refract(const Vector4& other, float eta) const -> Vector4
		{
			return {X * eta, Y * eta, Z * eta, W * eta};
		}

		[[nodiscard]] auto Rcp() const -> Vector4
		{
			return {1.0F / X, 1.0F / Y, 1.0F / Z, 1.0F / W};
		}

		[[nodiscard]] auto LengthInverse() const -> Vector4
		{
			return {1.0F / Length(), 1.0F / Length(), 1.0F / Length(), 1.0F / Length()};
		}

		[[nodiscard]] auto ToVector3() const -> Vector3
		{
			return {X, Y, Z};
		}

		[[nodiscard]] auto ToVector2() const -> Vector2
		{
			return {X, Y};
		}

		static auto Zero() -> Vector4
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