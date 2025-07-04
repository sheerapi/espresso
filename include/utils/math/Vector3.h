#pragma once
#include "cglm/types.h"
#include "cglm/vec3.h"

namespace math
{
	struct Vector3
	{
		union
		{
			struct
			{
				float x, y, z;
			};
			vec3 raw;
		};

		Vector3() : raw{0.0F, 0.0F}
		{
		}
		Vector3(float x, float y, float z) : raw{x, y, z}
		{
		}

		// Implicit conversion to cglm's vec3 if needed
		operator vec3&()
		{
			return raw;
		}
		operator const vec3&() const
		{
			return raw;
		}

		[[nodiscard]] auto length() const -> float
		{
			return glm_vec3_norm(const_cast<vec3&>(raw));
		}

		[[nodiscard]] auto normalized() const -> Vector3
		{
			Vector3 out;
			glm_vec3_normalize_to(const_cast<vec3&>(raw), out.raw);
			return out;
		}

		void normalize()
		{
			glm_vec3_normalize(raw);
		}

		static auto dot(Vector3 a, Vector3 b) -> float
		{
			return glm_vec3_dot(a, b);
		}

		static auto cross(Vector3 a, Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_cross(a, b, result);
			return result;
		}

		auto dot(Vector3 other) -> float
		{
			return glm_vec3_dot(raw, other);
		}

		auto cross(Vector3 other) -> Vector3
		{
			Vector3 result;
			glm_vec3_cross(raw, other, result);
			return result;
		}

		auto magnitude() -> float
		{
			return glm_vec3_norm(raw);
		}

		auto magnitudeSquared() -> float
		{
			return glm_vec3_norm2(raw);
		}

		auto operator+(Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_add(raw, b, result);
			return result;
		}

		auto operator-(Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_sub(raw, b, result);
			return result;
		}

		auto operator-(float b) -> Vector3
		{
			Vector3 result;
			glm_vec3_subs(raw, b, result);
			return result;
		}

		auto operator*(Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_mul(raw, b, result);
			return result;
		}

		auto operator*(float b) -> Vector3
		{
			Vector3 result;
			glm_vec3_scale(raw, b, result);
			return result;
		}

		auto operator/(Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_div(raw, b, result);
			return result;
		}

		auto operator/(float b) -> Vector3
		{
			Vector3 result;
			glm_vec3_divs(raw, b, result);
			return result;
		}

		void operator+=(Vector3 b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
		}

		void operator-=(Vector3 b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
		}

		void operator*=(Vector3 b)
		{
			x *= b.x;
			y *= b.y;
			z *= b.z;
		}

		void operator*=(float b)
		{
			x *= b;
			y *= b;
			z *= b;
		}

		void operator/=(Vector3 b)
		{
			x /= b.x;
			y /= b.y;
			z /= b.z;
		}

		void operator/=(float b)
		{
			x /= b;
			y /= b;
			z /= b;
		}

		void negate()
		{
			glm_vec3_negate(raw);
		}

		[[nodiscard]] auto negated() const -> Vector3
		{
			Vector3 out;
			glm_vec3_negate_to(const_cast<vec3&>(raw), out.raw);
			return out;
		}

		void rotate(float angle, Vector3 axis)
		{
			glm_vec3_rotate(raw, angle, axis);
		}

		void project(Vector3 b)
		{
			glm_vec3_proj(raw, b, raw);
		}

		auto projected(Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_proj(raw, b, result);
			return result;
		}

		auto perpendicular(Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_ortho(raw, result);
			return result;
		}

		static auto center(Vector3 a, Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_center(a, b, result);
			return result;
		}

		static auto distance(Vector3 a, Vector3 b) -> float
		{
			return glm_vec3_distance(a, b);
		}

		static auto distanceSquared(Vector3 a, Vector3 b) -> float
		{
			return glm_vec3_distance2(a, b);
		}

		[[nodiscard]] auto distance(Vector3 b) const -> float
		{
			return glm_vec3_distance(const_cast<vec3&>(raw), b);
		}

		[[nodiscard]] auto distanceSquared(Vector3 b) const -> float
		{
			return glm_vec3_distance2(const_cast<vec3&>(raw), b);
		}

		static auto max(Vector3 a, Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_maxv(a, b, result);
			return result;
		}

		static auto min(Vector3 a, Vector3 b) -> Vector3
		{
			Vector3 result;
			glm_vec3_minv(a, b, result);
			return result;
		}

		void clamp(float min, float max)
		{
			glm_vec3_clamp(raw, min, max);
		}

		static auto lerp(Vector3 a, Vector3 b, float t) -> Vector3
		{
			Vector3 result;
			glm_vec3_lerp(a, b, t, result);
			return result;
		}

		void lerp(Vector3 b, float t)
		{
			glm_vec3_lerp(raw, b, t, raw);
		}

		void reflect(Vector3 normal)
		{
			glm_vec3_reflect(raw, normal, raw);
		}

		[[nodiscard]] auto reflected(Vector3 normal) const -> Vector3
		{
			Vector3 result;
			glm_vec3_reflect(const_cast<vec3&>(raw), normal.normalized(), result);
			return result;
		}

		void refract(Vector3 normal, float eta)
		{
			glm_vec3_refract(normalized(), normal.normalized(), eta, raw);
		}

		[[nodiscard]] auto refracted(Vector3 normal, float eta) const -> Vector3
		{
			Vector3 result;
			glm_vec3_refract(normalized(), normal.normalized(), eta, result);
			return result;
		}

		static auto angle(Vector3 a, Vector3 b) -> float
		{
			return glm_vec3_angle(a, b) * (180.F / (float)M_PI);
		}

		[[nodiscard]] auto angle(Vector3 b) const -> float
		{
			return glm_vec3_angle(const_cast<vec3&>(raw), b) * (180.F / (float)M_PI);
		}

		auto operator==(Vector3 b) -> bool
		{
			return glm_vec3_eqv(raw, b);
		}

		auto operator!=(Vector3 b) -> bool
		{
			return !glm_vec3_eqv(raw, b);
		}

		auto operator==(float b) -> bool
		{
			return glm_vec3_eq(raw, b);
		}

		auto operator!=(float b) -> bool
		{
			return !glm_vec3_eq(raw, b);
		}

		auto operator<(float b) -> bool
		{
			return magnitudeSquared() < (b * b);
		}

		auto operator>(float b) -> bool
		{
			return magnitudeSquared() > (b * b);
		}

		static auto up() -> Vector3
		{
			return {0.0F, 1.0F, 0.0F};
		}

		static auto down() -> Vector3
		{
			return {0.0F, -1.0F, 0.0F};
		}

		static auto left() -> Vector3
		{
			return {-1.0F, 0.0F, 0.0F};
		}

		static auto right() -> Vector3
		{
			return {1.0F, 0.0F, 0.0F};
		}

		static auto forward() -> Vector3
		{
			return {0.0F, 0.0F, 1.0F};
		}

		static auto backward() -> Vector3
		{
			return {0.0F, 0.0F, -1.0F};
		}

		static auto zero() -> Vector3
		{
			return {0.0F, 0.0F, 0.0F};
		}

		static auto one() -> Vector3
		{
			return {1.0F, 1.0F, 1.0F};
		}
	};
}