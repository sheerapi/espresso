#pragma once
#include <cmath>
#include "utils/math/Vector3.h"
#include "utils/math/Vector4.h"

namespace math
{
	struct Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion() : x(0.0F), y(0.0F), z(0.0F), w(1.0F) {};
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
		Quaternion(const Vector3& axis, float angle)
		{
			float halfAngle = angle * 0.5F;
			float s = std::sin(halfAngle);
			w = std::cos(halfAngle);
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
		}

		Quaternion(const Vector4& vec4) : x(vec4.x), y(vec4.y), z(vec4.z), w(vec4.w) {};

		auto operator+(const Quaternion& other) const -> Quaternion
		{
			return {x + other.x, y + other.y, z + other.z, w + other.w};
		}

		auto operator-(const Quaternion& other) const -> Quaternion
		{
			return {x - other.x, y - other.y, z - other.z, w - other.w};
		}

		auto operator*(const Quaternion& other) const -> Quaternion
		{
			return {(w * other.x) + (x * other.w) + (y * other.z) - (z * other.y),
					(w * other.y) + (y * other.w) + (z * other.x) - (x * other.z),
					(w * other.z) + (z * other.w) + (x * other.y) - (y * other.x),
					(w * other.w) - (x * other.x) - (y * other.y) - (z * other.z)};
		}

		auto operator*(const Vector3& vec) const -> Vector3
		{
			return {(x * 2.0F * (y * vec.z - z * vec.y)) +
						(vec.x * (w * w - x * x - y * y + z * z)),
					(y * 2.0F * (z * vec.x - x * vec.z)) +
						(vec.y * (w * w + x * x - y * y - z * z)),
					(z * 2.0F * (x * vec.y - y * vec.x)) +
						(vec.z * (w * w + x * x + y * y - z * z))};
		}

		auto operator*(float scalar) const -> Quaternion
		{
			return {x * scalar, y * scalar, z * scalar, w * scalar};
		}

		auto operator/(float scalar) const -> Quaternion
		{
			return {x / scalar, y / scalar, z / scalar, w / scalar};
		}

		auto operator==(const Quaternion& other) const -> bool
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		auto operator!=(const Quaternion& other) const -> bool
		{
			return !(*this == other);
		}

		[[nodiscard]] auto Length() const -> float
		{
			return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
		}

		[[nodiscard]] auto LengthSquared() const -> float
		{
			return (x * x) + (y * y) + (z * z) + (w * w);
		}

		[[nodiscard]] auto Normalized() const -> Quaternion
		{
			float len = Length();
			if (len == 0.0F)
			{
				return {0.0F, 0.0F, 0.0F, 1.0F};
			}
			return {x / len, y / len, z / len, w / len};
		}
		[[nodiscard]] auto Conjugate() const -> Quaternion
		{
			return {-x, -y, -z, w};
		}

		[[nodiscard]] auto Inverse() const -> Quaternion
		{
			float len = LengthSquared();
			if (len == 0.0F)
			{
				return {0.0F, 0.0F, 0.0F, 1.0F};
			}
			return {-x / len, -y / len, -z / len, w / len};
		}

		[[nodiscard]] auto ToVector4() const -> Vector4
		{
			return {x, y, z, w};
		}

		[[nodiscard]] auto Dot(const Quaternion& other) const -> float
		{
			return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
		}

		[[nodiscard]] auto Angle() const -> float
		{
			return std::atan2(std::sqrt((x * x) + (y * y) + (z * z)), w);
		}

		[[nodiscard]] auto Axis() const -> Vector3
		{
			float sinHalfAngle = std::sqrt((x * x) + (y * y) + (z * z));
			if (sinHalfAngle == 0.0F)
			{
				return {1.0F, 0.0F, 0.0F};
			}
			return {x / sinHalfAngle, y / sinHalfAngle, z / sinHalfAngle};
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

			float sinr_cosp = 2.0F * (w * x + y * z);
			float cosr_cosp = 1.0F - (2.0F * (x * x + y * y));
			float roll = std::atan2(sinr_cosp, cosr_cosp) * kRadToDeg;

			float sinp = 2.0F * (w * y - z * x);
			float pitch = 0.0F;
			if (std::abs(sinp) >= 1.0F)
			{
				pitch = std::copysign(90.0F, sinp);
			}
			else
			{
				pitch = std::asin(sinp) * kRadToDeg;
			}

			float siny_cosp = 2.0F * (w * z + x * y);
			float cosy_cosp = 1.0F - (2.0F * (y * y + z * z));
			float yaw = std::atan2(siny_cosp, cosy_cosp) * kRadToDeg;
			return Vector3{roll, pitch, yaw};
			return {roll, pitch, yaw};
		}

		// Converts Euler angles (in degrees) to a Quaternion.
		// The input Vector3 represents rotations in the order: x (roll), y (pitch), z (yaw).
		static auto FromEuler(const Vector3& euler) -> Quaternion
		{
			constexpr float kDegToRad = static_cast<float>(M_PI / 180.0F);

			float halfRoll = euler.x * 0.5F * kDegToRad;
			float halfPitch = euler.y * 0.5F * kDegToRad;
			float halfyaw = euler.z * 0.5F * kDegToRad;

			float cosRoll = std::cos(halfRoll);
			float sinRoll = std::sin(halfRoll);
			float cosPitch = std::cos(halfPitch);
			float sinPitch = std::sin(halfPitch);
			float sinyaw = std::sin(halfyaw);
			float cosyaw = std::cos(halfyaw);
			float x = (sinRoll * cosPitch * cosyaw) - (cosRoll * sinPitch * sinyaw);
			float y = (cosRoll * sinPitch * cosyaw) + (sinRoll * cosPitch * sinyaw);
			return {x, y,
					(cosRoll * cosPitch * sinyaw) - (sinRoll * sinPitch * cosyaw),
					(cosRoll * cosPitch * cosyaw) + (sinRoll * sinPitch * sinyaw)};
		}

		static auto Identity() -> Quaternion
		{
			return {0.0F, 0.0F, 0.0F, 1.0F};
		}

		static auto Rotationx(float angle) -> Quaternion
		{
			return {std::sin(angle * 0.5F), 0.0F, 0.0F, std::cos(angle * 0.5F)};
		}

		static auto Rotationy(float angle) -> Quaternion
		{
			return {0.0F, std::sin(angle * 0.5F), 0.0F, std::cos(angle * 0.5F)};
		}

		static auto Rotationz(float angle) -> Quaternion
		{
			return {0.0F, 0.0F, std::sin(angle * 0.5F), std::cos(angle * 0.5F)};
		}
	};
}