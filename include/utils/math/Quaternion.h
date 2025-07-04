#pragma once
#define CGLM_FORCE_LEFT_HANDED 1
#include <cmath>
#include "utils/math/Vector3.h"
#include "utils/math/Vector4.h"
#include "cglm/quat.h"

namespace math
{
	struct Quaternion
	{
	public:
		union
		{
			struct
			{
				float x, y, z, w;
			};
			versor raw;
		};

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

		operator versor&()
		{
			return raw;
		}
		operator const versor&() const
		{
			return raw;
		}

		auto operator+(const Quaternion& other) const -> Quaternion
		{
			return {x + other.x, y + other.y, z + other.z, w + other.w};
		}

		auto operator-(const Quaternion& other) const -> Quaternion
		{
			return {x - other.x, y - other.y, z - other.z, w - other.w};
		}

		auto operator*(Quaternion other) const -> Quaternion
		{
			Quaternion result;
			glm_quat_mul(const_cast<versor&>(raw), other, result);
			return result;
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

		[[nodiscard]] auto length() const -> float
		{
			return glm_quat_norm(const_cast<versor&>(raw));
		}

		[[nodiscard]] auto lengthSquared() const -> float
		{
			return length() * length();
		}

		[[nodiscard]] auto normalized() const -> Quaternion
		{
			Quaternion result;
			glm_quat_normalize_to(const_cast<versor&>(raw), result);
			return result;
		}

		void normalize()
		{
			glm_quat_normalize(raw);
		}

		[[nodiscard]] auto conjugate() const -> Quaternion
		{
			Quaternion result;
			glm_quat_conjugate(const_cast<versor&>(raw), result);
			return result;
		}

		[[nodiscard]] auto inverse() const -> Quaternion
		{
			Quaternion result;
			glm_quat_inv(const_cast<versor&>(raw), result);
			return result;
		}

		[[nodiscard]] auto toVector4() const -> Vector4
		{
			return {x, y, z, w};
		}

		[[nodiscard]] auto dot(Quaternion other) const -> float
		{
			return glm_quat_dot(const_cast<versor&>(raw), other);
		}

		static auto dot(Quaternion a, Quaternion b) -> float
		{
			return glm_quat_dot(a, b);
		}

		[[nodiscard]] auto angle() const -> float
		{
			return glm_quat_angle(const_cast<versor&>(raw));
		}

		[[nodiscard]] auto axis() const -> Vector3
		{
			Quaternion result;
			Vector3 imag;
			glm_quat_axis(const_cast<versor&>(raw), result);
			glm_quat_imag(result, imag);
			return imag;
		}

		[[nodiscard]] auto toEuler() const -> Vector3
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
		static auto fromEuler(const Vector3& euler) -> Quaternion
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

		static auto identity() -> Quaternion
		{
			return {0.0F, 0.0F, 0.0F, 1.0F};
		}

		static auto rotationX(float angle) -> Quaternion
		{
			return {std::sin(angle * 0.5F), 0.0F, 0.0F, std::cos(angle * 0.5F)};
		}

		static auto rotationY(float angle) -> Quaternion
		{
			return {0.0F, std::sin(angle * 0.5F), 0.0F, std::cos(angle * 0.5F)};
		}

		static auto rotationZ(float angle) -> Quaternion
		{
			return {0.0F, 0.0F, std::sin(angle * 0.5F), std::cos(angle * 0.5F)};
		}

		static auto lerp(Quaternion a, Quaternion b, float t) -> Quaternion
		{
			Quaternion result;
			glm_quat_lerp(a, b, t, result);
			return result;
		}

		void lerp(Quaternion b, float t)
		{
			glm_quat_lerp(raw, b, t, raw);
		}

		static auto slerp(Quaternion a, Quaternion b, float t) -> Quaternion
		{
			Quaternion result;
			glm_quat_slerp(a, b, t, result);
			return result;
		}

		void slerp(Quaternion b, float t)
		{
			glm_quat_slerp(raw, b, t, raw);
		}

		static auto nlerp(Quaternion a, Quaternion b, float t) -> Quaternion
		{
			Quaternion result;
			glm_quat_nlerp(a, b, t, result);
			return result;
		}

		void nlerp(Quaternion b, float t)
		{
			glm_quat_nlerp(raw, b, t, raw);
		}

		

		static auto lookRotation(Vector3 direction, Vector3 up) -> Quaternion
		{
			Quaternion result;
			glm_quat_for(direction, up, result);
			return result;
		}

		static auto lookRotation(Vector3 from, Vector3 to, Vector3 up) -> Quaternion
		{
			Quaternion result;
			glm_quat_forp(from, to, up, result);
			return result;
		}
	};
}