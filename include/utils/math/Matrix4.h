#pragma once
#define CGLM_FORCE_LEFT_HANDED 1
#include "cglm/affine.h"
#include "cglm/affine-pre.h"

#include "cglm/cam.h"
#include "utils/math/Quaternion.h"
#include "cglm/mat4.h"
#include "utils/math/Vector4.h"
#include "utils/math/Vector3.h"
#include <vector>

namespace math
{
	// column major matrices
	struct Matrix4
	{
	public:
		union
		{
			struct
			{
				float data[4][4];
			};
			mat4 raw;
		};

		operator mat4&()
		{
			return raw;
		}
		operator const mat4&() const
		{
			return raw;
		}

		Matrix4()
		{
			glm_mat4_identity(raw);
		}

		auto operator*(Matrix4 other) const -> Matrix4
		{
			Matrix4 result;
			glm_mat4_mul(const_cast<mat4&>(raw), other, result);
			return result;
		}

		auto operator*(Vector4 other) const -> Vector4
		{
			Vector4 result;
			glm_mat4_mulv(const_cast<mat4&>(raw), other, result);
			return result;
		}

		auto operator*(Vector3 other) const -> Vector3
		{
			Vector3 result;
			glm_mat4_mulv3(const_cast<mat4&>(raw), other, 0, result);
			return result;
		}

		auto operator*=(float other) const -> Matrix4
		{
			glm_mat4_scale(const_cast<mat4&>(raw), other);
			return *this;
		}

		static auto multiply(const std::vector<Matrix4>& matrices) -> Matrix4
		{
			Matrix4 result;
			std::vector<mat4*> rawMatrices;
			rawMatrices.reserve(matrices.size());
			for (auto matrix : matrices)
			{
				rawMatrices.push_back(&matrix.raw);
			}
			glm_mat4_mulN(rawMatrices.data(), rawMatrices.size(), result.raw);
			return result;
		}

		auto trace() -> float
		{
			return glm_mat4_trace(raw);
		}

		auto traceRot() -> float
		{
			return glm_mat4_trace3(raw);
		}

		void transpose()
		{
			glm_mat4_transpose(raw);
		}

		auto transposed() -> Matrix4
		{
			Matrix4 result;
			glm_mat4_transpose_to(raw, result);
			return result;
		}

		auto determinant() -> float
		{
			return glm_mat4_det(raw);
		}

		void invert()
		{
			glm_mat4_inv(raw, raw);
		}

		auto inverse() -> Matrix4
		{
			Matrix4 result;
			glm_mat4_inv(raw, result);
			return result;
		}

		void invertFast()
		{
			glm_mat4_inv_fast(raw, raw);
		}

		auto inverseFast() -> Matrix4
		{
			Matrix4 result;
			glm_mat4_inv_fast(raw, result);
			return result;
		}

		static auto rowMatrixColumn(Vector4 r, Matrix4 m, Vector4 c) -> float
		{
			return glm_mat4_rmc(r, m, c);
		}

		void translate(Vector3 pos)
		{
			glm_translate(raw, pos);
		}

		static auto translation(Vector3 pos) -> Matrix4
		{
			Matrix4 result;
			glm_translate_make(result, pos);
			return result;
		}

		void scale(Vector3 pos)
		{
			glm_scale(raw, pos);
		}

		static auto scaling(Vector3 pos) -> Matrix4
		{
			Matrix4 result;
			glm_scale_make(result, pos);
			return result;
		}

		static auto quatLook(Vector3 eye, Vector3 orientation) -> Matrix4
		{
			Matrix4 result;
			glm_quat_look(eye, orientation, result);
			return result;
		}

		void rotate(Quaternion rotation)
		{
			glm_quat_rotate(raw, rotation, raw);
		}

		static auto rotation(Quaternion rotation) -> Matrix4
		{
			Matrix4 result;
			glm_quat_rotate(result, rotation, result);
			return result;
		}

		void rotateAt(Quaternion rotation, Vector3 pivot)
		{
			glm_quat_rotate_at(raw, rotation, pivot);
		}

		void rotateAtNew(Quaternion rotation, Vector3 pivot)
		{
			glm_quat_rotate_atm(raw, rotation, pivot);
		}

		static auto frustum(float left, float right, float bottom, float top, float near, float far) -> Matrix4
		{
			Matrix4 result;
			glm_frustum(left, right, bottom, top, near, far, result);
			return result;
		}

		static auto ortho(float left, float right, float bottom, float top, float near,
							float far) -> Matrix4
		{
			Matrix4 result;
			glm_ortho(left, right, bottom, top, near, far, result);
			return result;
		}

		static auto ortho(float aspect) -> Matrix4
		{
			Matrix4 result;
			glm_ortho_default(aspect, result);
			return result;
		}

		static auto perspective(float fovY, float aspect, float near, float far)
		{
			Matrix4 result;
			glm_perspective(fovY, aspect, near, far, result);
			return result;
		}

		static auto perspective(float aspect) -> Matrix4
		{
			Matrix4 result;
			glm_perspective_default(aspect, result);
			return result;
		}

		void resizePerspective(float aspect)
		{
			glm_perspective_resize(aspect, raw);
		}

		static auto lookAt(Vector3 eye, Vector3 center, Vector3 up) -> Matrix4
		{
			Matrix4 result;
			glm_lookat(eye, center, up, result);
			return result;
		}

		static auto look(Vector3 eye, Vector3 direction, Vector3 up) -> Matrix4
		{
			Matrix4 result;
			glm_look(eye, direction, up, result);
			return result;
		}

		auto perspectiveAspect() -> float
		{
			return glm_persp_aspect(raw);
		}

		auto getRotation() -> Quaternion
		{
			Quaternion result;
			glm_mat4_quat(raw, result);
			return result;
		}
	};
}