#pragma once
#include "cglm/types.h"
#include "cglm/vec4.h"

namespace math
{
	struct Vector4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			vec4 raw;
		};

		Vector4() : raw{0.0F, 0.0F, 0.0F, 0.0F}
		{
		}
		Vector4(float x, float y, float z, float w) : raw{x, y, z, w}
		{
		}

		// Implicit conversion to cglm's vec4 if needed
		operator vec4&()
		{
			return raw;
		}
		operator const vec4&() const
		{
			return raw;
		}

		[[nodiscard]] auto length() const -> float
		{
			return glm_vec4_norm(const_cast<vec4&>(raw));
		}

		[[nodiscard]] auto normalized() const -> Vector4
		{
			Vector4 out;
			glm_vec4_normalize_to(const_cast<vec4&>(raw), out.raw);
			return out;
		}

		void normalize()
		{
			glm_vec4_normalize(raw);
		}

		static auto dot(Vector4 a, Vector4 b) -> float
		{
			return glm_vec4_dot(a, b);
		}

		auto dot(Vector4 other) -> float
		{
			return glm_vec4_dot(raw, other);
		}

		auto magnitude() -> float
		{
			return glm_vec4_norm(raw);
		}

		auto magnitudeSquared() -> float
		{
			return glm_vec4_norm2(raw);
		}

		auto operator+(Vector4 b) -> Vector4
		{
			Vector4 result;
			glm_vec4_add(raw, b, result);
			return result;
		}

		auto operator-(float b) -> Vector4
		{
			Vector4 result;
			glm_vec4_adds(raw, b, result);
			return result;
		}

		auto operator*(Vector4 b) -> Vector4
		{
			Vector4 result;
			glm_vec4_mul(raw, b, result);
			return result;
		}

		auto operator*(float b) -> Vector4
		{
			Vector4 result;
			glm_vec4_scale(raw, b, result);
			return result;
		}

		auto operator/(Vector4 b) -> Vector4
		{
			Vector4 result;
			glm_vec4_div(raw, b, result);
			return result;
		}

		auto operator/(float b) -> Vector4
		{
			Vector4 result;
			glm_vec4_divs(raw, b, result);
			return result;
		}

		void operator+=(Vector4 b)
		{
			x += b.x;
			y += b.y;
			z += b.z;
			w += b.w;
		}

		void operator-=(Vector4 b)
		{
			x -= b.x;
			y -= b.y;
			z -= b.z;
			w -= b.w;
		}

		void operator*=(Vector4 b)
		{
			x *= b.x;
			y *= b.y;
			z *= b.z;
			w *= b.w;
		}

		void operator*=(float b)
		{
			x *= b;
			y *= b;
			z *= b;
			w *= b;
		}

		void operator/=(Vector4 b)
		{
			x /= b.x;
			y /= b.y;
			z /= b.z;
			w /= b.w;
		}

		void operator/=(float b)
		{
			x /= b;
			y /= b;
			z /= b;
			w /= b;
		}

		void negate()
		{
			glm_vec4_negate(raw);
		}

		[[nodiscard]] auto negated() const -> Vector4
		{
			Vector4 out;
			glm_vec4_negate_to(const_cast<vec4&>(raw), out.raw);
			return out;
		}

		static auto distance(Vector4 a, Vector4 b) -> float
		{
			return glm_vec4_distance(a, b);
		}

		static auto distanceSquared(Vector4 a, Vector4 b) -> float
		{
			return glm_vec4_distance2(a, b);
		}

		[[nodiscard]] auto distance(Vector4 b) const -> float
		{
			return glm_vec4_distance(const_cast<vec4&>(raw), b);
		}

		[[nodiscard]] auto distanceSquared(Vector4 b) const -> float
		{
			return glm_vec4_distance2(const_cast<vec4&>(raw), b);
		}

		static auto max(Vector4 a, Vector4 b) -> Vector4
		{
			Vector4 result;
			glm_vec4_maxv(a, b, result);
			return result;
		}

		static auto min(Vector4 a, Vector4 b) -> Vector4
		{
			Vector4 result;
			glm_vec4_minv(a, b, result);
			return result;
		}

		void clamp(float min, float max)
		{
			glm_vec4_clamp(raw, min, max);
		}

		static auto lerp(Vector4 a, Vector4 b, float t) -> Vector4
		{
			Vector4 result;
			glm_vec4_lerp(a, b, t, result);
			return result;
		}

		void lerp(Vector4 b, float t)
		{
			glm_vec4_lerp(raw, b, t, raw);
		}

		void reflect(Vector4 normal)
		{
			glm_vec4_reflect(raw, normal, raw);
		}

		[[nodiscard]] auto reflected(Vector4 normal) const -> Vector4
		{
			Vector4 result;
			glm_vec4_reflect(const_cast<vec4&>(raw), normal.normalized(), result);
			return result;
		}

		void refract(Vector4 normal, float eta)
		{
			glm_vec4_refract(normalized(), normal.normalized(), eta, raw);
		}

		[[nodiscard]] auto refracted(Vector4 normal, float eta) const -> Vector4
		{
			Vector4 result;
			glm_vec4_refract(normalized(), normal.normalized(), eta, result);
			return result;
		}

		auto operator==(Vector4 b) -> bool
		{
			return glm_vec4_eqv(raw, b);
		}

		auto operator!=(Vector4 b) -> bool
		{
			return !glm_vec4_eqv(raw, b);
		}

		auto operator==(float b) -> bool
		{
			return glm_vec4_eq(raw, b);
		}

		auto operator!=(float b) -> bool
		{
			return !glm_vec4_eq(raw, b);
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