#pragma once
#include <cmath>
#include "utils/math/Vector3.h"
#include "utils/math/Vector4.h"

namespace math
{
	struct Quaternion
	{
	public:
		float X;
		float Y;
		float Z;
		float W;

		Quaternion() : X(0.0F), Y(0.0F), Z(0.0F), W(1.0F) {};
		Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {};
		Quaternion(const Vector3& axis, float angle)
		{
			float halfAngle = angle * 0.5F;
			float s = std::sin(halfAngle);
			W = std::cos(halfAngle);
			X = axis.X * s;
			Y = axis.Y * s;
			Z = axis.Z * s;
		}

		Quaternion(const Vector4& vec4) : X(vec4.X), Y(vec4.Y), Z(vec4.Z), W(vec4.W) {};

		auto operator+(const Quaternion& other) const -> Quaternion
		{
			return {X + other.X, Y + other.Y, Z + other.Z, W + other.W};
		}

		auto operator-(const Quaternion& other) const -> Quaternion
		{
			return {X - other.X, Y - other.Y, Z - other.Z, W - other.W};
		}

		auto operator*(const Quaternion& other) const -> Quaternion
		{
			return {(W * other.X) + (X * other.W) + (Y * other.Z) - (Z * other.Y),
					(W * other.Y) + (Y * other.W) + (Z * other.X) - (X * other.Z),
					(W * other.Z) + (Z * other.W) + (X * other.Y) - (Y * other.X),
					(W * other.W) - (X * other.X) - (Y * other.Y) - (Z * other.Z)};
		}

		auto operator*(const Vector3& vec) const -> Vector3
		{
			return {(X * 2.0F * (Y * vec.Z - Z * vec.Y)) +
						(vec.X * (W * W - X * X - Y * Y + Z * Z)),
					(Y * 2.0F * (Z * vec.X - X * vec.Z)) +
						(vec.Y * (W * W + X * X - Y * Y - Z * Z)),
					(Z * 2.0F * (X * vec.Y - Y * vec.X)) +
						(vec.Z * (W * W + X * X + Y * Y - Z * Z))};
		}

		auto operator*(float scalar) const -> Quaternion
		{
			return {X * scalar, Y * scalar, Z * scalar, W * scalar};
		}

		auto operator/(float scalar) const -> Quaternion
		{
			return {X / scalar, Y / scalar, Z / scalar, W / scalar};
		}

		auto operator==(const Quaternion& other) const -> bool
		{
			return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
		}

		auto operator!=(const Quaternion& other) const -> bool
		{
			return !(*this == other);
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt((X * X) + (Y * Y) + (Z * Z) + (W * W));
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (X * X) + (Y * Y) + (Z * Z) + (W * W);
		}

		[[nodiscard]] auto Normalized() const -> Quaternion
		{
			float len = Length();
			if (len == 0.0F)
			{
				return {0.0F, 0.0F, 0.0F, 1.0F};
			}
			return {X / len, Y / len, Z / len, W / len};
		}
		[[nodiscard]] auto Conjugate() const -> Quaternion
		{
			return {-X, -Y, -Z, W};
		}

		[[nodiscard]] auto Inverse() const -> Quaternion
		{
			float len = LengthSquared();
			if (len == 0.0F)
			{
				return {0.0F, 0.0F, 0.0F, 1.0F};
			}
			return {-X / len, -Y / len, -Z / len, W / len};
		}

		[[nodiscard]] auto ToVector4() const -> Vector4
		{
			return {X, Y, Z, W};
		}

		[[nodiscard]] auto Dot(const Quaternion& other) const -> float
		{
			return (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W);
		}

		[[nodiscard]] auto Angle() const -> float
		{
			return std::atan2(std::sqrt((X * X) + (Y * Y) + (Z * Z)), W);
		}

		[[nodiscard]] auto Axis() const -> Vector3
		{
			float sinHalfAngle = std::sqrt((X * X) + (Y * Y) + (Z * Z));
			if (sinHalfAngle == 0.0F)
			{
				return {1.0F, 0.0F, 0.0F};
			}
			return {X / sinHalfAngle, Y / sinHalfAngle, Z / sinHalfAngle};
		}

		[[nodiscard]] auto AngleAxis() const -> std::pair<float, Vector3>
		{
			float angle = Angle();
			Vector3 axis = Axis();
			return {angle, axis};
		}

		[[nodiscard]] auto ToEuler() const -> Vector3
		{
			constexpr float kRadToDeg = 180.0F / static_cast<float>(M_PI);

			float sinr_cosp = 2.0F * (W * X + Y * Z);
			float cosr_cosp = 1.0F - (2.0F * (X * X + Y * Y));
			float roll = std::atan2(sinr_cosp, cosr_cosp) * kRadToDeg;

			float sinp = 2.0F * (W * Y - Z * X);
			float pitch = 0.0F;
			if (std::abs(sinp) >= 1.0F)
			{
				pitch = std::copysign(90.0F, sinp);
			}
			else
			{
				pitch = std::asin(sinp) * kRadToDeg;
			}

			float siny_cosp = 2.0F * (W * Z + X * Y);
			float cosy_cosp = 1.0F - (2.0F * (Y * Y + Z * Z));
			float yaw = std::atan2(siny_cosp, cosy_cosp) * kRadToDeg;
			return Vector3{roll, pitch, yaw};
			return {roll, pitch, yaw};
		}

		// Converts Euler angles (in degrees) to a Quaternion.
		// The input Vector3 represents rotations in the order: X (roll), Y (pitch), Z (yaw).
		static auto FromEuler(const Vector3& euler) -> Quaternion
		{
			constexpr float kDegToRad = static_cast<float>(M_PI / 180.0F);

			float halfRoll = euler.X * 0.5F * kDegToRad;
			float halfPitch = euler.Y * 0.5F * kDegToRad;
			float halfYaw = euler.Z * 0.5F * kDegToRad;

			float cosRoll = std::cos(halfRoll);
			float sinRoll = std::sin(halfRoll);
			float cosPitch = std::cos(halfPitch);
			float sinPitch = std::sin(halfPitch);
			float sinYaw = std::sin(halfYaw);
			float cosYaw = std::cos(halfYaw);
			float x = (sinRoll * cosPitch * cosYaw) - (cosRoll * sinPitch * sinYaw);
			float y = (cosRoll * sinPitch * cosYaw) + (sinRoll * cosPitch * sinYaw);
			return {x, y,
					(cosRoll * cosPitch * sinYaw) - (sinRoll * sinPitch * cosYaw),
					(cosRoll * cosPitch * cosYaw) + (sinRoll * sinPitch * sinYaw)};
		}

		static auto Identity() -> Quaternion
		{
			return {0.0F, 0.0F, 0.0F, 1.0F};
		}

		static auto RotationX(float angle) -> Quaternion
		{
			return {std::sin(angle * 0.5F), 0.0F, 0.0F, std::cos(angle * 0.5F)};
		}

		static auto RotationY(float angle) -> Quaternion
		{
			return {0.0F, std::sin(angle * 0.5F), 0.0F, std::cos(angle * 0.5F)};
		}

		static auto RotationZ(float angle) -> Quaternion
		{
			return {0.0F, 0.0F, std::sin(angle * 0.5F), std::cos(angle * 0.5F)};
		}
	};
}