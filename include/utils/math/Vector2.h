#pragma once
#include "cglm/types.h"
#include "cglm/vec2.h"

namespace math
{
	struct Vector2
	{
		union
		{
			struct
			{
				float x, y;
			};
			vec2 raw;
		};

		Vector2() : raw{0.0F, 0.0F}
		{
		}
		Vector2(float x, float y) : raw{x, y}
		{
		}

		// Implicit conversion to cglm's vec2 if needed
		operator vec2&()
		{
			return raw;
		}
		operator const vec2&() const
		{
			return raw;
		}

		[[nodiscard]] auto length() const -> float
		{
			return glm_vec2_norm(const_cast<vec2&>(raw));
		}

		[[nodiscard]] auto normalized() const -> Vector2
		{
			Vector2 out;
			glm_vec2_normalize_to(const_cast<vec2&>(raw), out.raw);
			return out;
		}

		void normalize()
		{
			glm_vec2_normalize(raw);
		}

		static auto dot(Vector2 a, Vector2 b) -> float
		{
			return glm_vec2_dot(a, b);
		}

		static auto cross(Vector2 a, Vector2 b) -> float
		{
			return glm_vec2_cross(a, b);
		}

		auto dot(Vector2 other) -> float
		{
			return glm_vec2_dot(raw, other);
		}

		auto cross(Vector2 other) -> float
		{
			return glm_vec2_cross(raw, other);
		}

		auto magnitude() -> float
		{
			return glm_vec2_norm(raw);
		}

		auto magnitudeSquared() -> float
		{
			return glm_vec2_norm2(raw);
		}

		auto operator +(Vector2 b) -> Vector2
		{
			Vector2 result;
			glm_vec2_add(raw, b, result);
			return result;
		}

		auto operator-(float b) -> Vector2
		{
			Vector2 result;
			glm_vec2_adds(raw, b, result);
			return result;
		}

		auto operator*(Vector2 b) -> Vector2
		{
			Vector2 result;
			glm_vec2_mul(raw, b, result);
			return result;
		}

		auto operator*(float b) -> Vector2
		{
			Vector2 result;
			glm_vec2_scale(raw, b, result);
			return result;
		}

		auto operator/(Vector2 b) -> Vector2
		{
			Vector2 result;
			glm_vec2_div(raw, b, result);
			return result;
		}

		auto operator/(float b) -> Vector2
		{
			Vector2 result;
			glm_vec2_divs(raw, b, result);
			return result;
		}

		void operator+=(Vector2 b)
		{
			x += b.x;
			y += b.y;
		}

		void operator-=(Vector2 b)
		{
			x -= b.x;
			y -= b.y;
		}

		void operator*=(Vector2 b)
		{
			x *= b.x;
			y *= b.y;
		}

		void operator*=(float b)
		{
			x *= b;
			y *= b;
		}

		void operator/=(Vector2 b)
		{
			x /= b.x;
			y /= b.y;
		}

		void operator/=(float b)
		{
			x /= b;
			y /= b;
		}

		void negate()
		{
			glm_vec2_negate(raw);
		}

		[[nodiscard]] auto negated() const -> Vector2
		{
			Vector2 out;
			glm_vec2_negate_to(const_cast<vec2&>(raw), out.raw);
			return out;
		}

		void rotate(float angle)
		{
			glm_vec2_rotate(raw, angle, raw);
		}

		[[nodiscard]] auto rotated(float angle) const -> Vector2
		{
			Vector2 result;
			glm_vec2_rotate(const_cast<vec2&>(raw), angle, result);
			return result;
		}

		static auto center(Vector2 a, Vector2 b) -> Vector2
		{
			Vector2 result;
			glm_vec2_center(a, b, result);
			return result;
		}

		static auto distance(Vector2 a, Vector2 b) -> float
		{
			return glm_vec2_distance(a, b);
		}

		static auto distanceSquared(Vector2 a, Vector2 b) -> float
		{
			return glm_vec2_distance2(a, b);
		}

		[[nodiscard]] auto distance(Vector2 b) const -> float
		{
			return glm_vec2_distance(const_cast<vec2&>(raw), b);
		}

		[[nodiscard]] auto distanceSquared(Vector2 b) const -> float
		{
			return glm_vec2_distance2(const_cast<vec2&>(raw), b);
		}

		static auto max(Vector2 a, Vector2 b) -> Vector2
		{
			Vector2 result;
			glm_vec2_maxv(a, b, result);
			return result;
		}

		static auto min(Vector2 a, Vector2 b) -> Vector2
		{
			Vector2 result;
			glm_vec2_minv(a, b, result);
			return result;
		}

		void clamp(float min, float max)
		{
			glm_vec2_clamp(raw, min, max);
		}

		static auto lerp(Vector2 a, Vector2 b, float t) -> Vector2
		{
			Vector2 result;
			glm_vec2_lerp(a, b, t, result);
			return result;
		}

		void lerp(Vector2 b, float t)
		{
			glm_vec2_lerp(raw, b, t, raw);
		}

		void reflect(Vector2 normal)
		{
			glm_vec2_reflect(raw, normal, raw);
		}

		[[nodiscard]] auto reflected(Vector2 normal) const -> Vector2
		{
			Vector2 result;
			glm_vec2_reflect(const_cast<vec2&>(raw), normal.normalized(), result);
			return result;
		}

		void refract(Vector2 normal, float eta)
		{
			glm_vec2_refract(normalized(), normal.normalized(), eta, raw);
		}

		[[nodiscard]] auto refracted(Vector2 normal, float eta) const -> Vector2
		{
			Vector2 result;
			glm_vec2_refract(normalized(), normal.normalized(), eta, result);
			return result;
		}

		auto operator==(Vector2 b) -> bool
		{
			return glm_vec2_eqv(raw, b);
		}

		auto operator!=(Vector2 b) -> bool
		{
			return !glm_vec2_eqv(raw, b);
		}

		auto operator==(float b) -> bool
		{
			return glm_vec2_eq(raw, b);
		}

		auto operator!=(float b) -> bool
		{
			return !glm_vec2_eq(raw, b);
		}

		auto operator<(float b) -> bool
		{
			return magnitudeSquared() < (b * b);
		}

		auto operator>(float b) -> bool
		{
			return magnitudeSquared() > (b * b);
		}
	};
}