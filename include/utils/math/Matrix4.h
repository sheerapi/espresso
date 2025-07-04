#pragma once
#include "utils/math/Quaternion.h"
#include "utils/math/Vector4.h"
#include <array>
#include <cmath>

namespace math
{
	// column major matrices
	struct Matrix4
	{
	public:
		float m[4][4];

		Matrix4()
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					m[i][j] = (i == j) ? 1.0F : 0.0F;
				}
			}
		};

		Matrix4(const float n[4][4])
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					m[i][j] = n[i][j];
				}
			}
		};

		Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2,
				const Vector4& col3)
		{
			m[0][0] = col0.x;
			m[1][0] = col0.y;
			m[2][0] = col0.z;
			m[3][0] = col0.w;

			m[0][1] = col1.x;
			m[1][1] = col1.y;
			m[2][1] = col1.z;
			m[3][1] = col1.w;

			m[0][2] = col2.x;
			m[1][2] = col2.y;
			m[2][2] = col2.z;
			m[3][2] = col2.w;

			m[0][3] = col3.x;
			m[1][3] = col3.y;
			m[2][3] = col3.z;
			m[3][3] = col3.w;
		};

		auto operator*(const Matrix4& other) const -> Matrix4
		{
			Matrix4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m[i][j] = m[i][0] * other.m[0][j] + m[i][1] * other.m[1][j] +
									 m[i][2] * other.m[2][j] + m[i][3] * other.m[3][j];
				}
			}
			return result;
		};

		auto operator*(const Vector4& vec) const -> Vector4
		{
			return {(m[0][0] * vec.x) + (m[1][0] * vec.y) + (m[2][0] * vec.z) +
						(m[3][0] * vec.w),
					(m[0][1] * vec.x) + (m[1][1] * vec.y) + (m[2][1] * vec.z) +
						(m[3][1] * vec.w),
					(m[0][2] * vec.x) + (m[1][2] * vec.y) + (m[2][2] * vec.z) +
						(m[3][2] * vec.w),
					(m[0][3] * vec.x) + (m[1][3] * vec.y) + (m[2][3] * vec.z) +
						(m[3][3] * vec.w)};
		};

		auto operator*(float scalar) const -> Matrix4
		{
			Matrix4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m[i][j] = m[i][j] * scalar;
				}
			}
			return result;
		};

		auto operator/(float scalar) const -> Matrix4
		{
			Matrix4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m[i][j] = m[i][j] / scalar;
				}
			}
			return result;
		};

		auto operator==(const Matrix4& other) const -> bool
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					if (m[i][j] != other.m[i][j])
					{
						return false;
					}
				}
			}
			return true;
		};

		auto operator!=(const Matrix4& other) const -> bool
		{
			return !(*this == other);
		};

		static auto Identity() -> Matrix4
		{
			return {};
		}

		[[nodiscard]] auto Transpose() const -> Matrix4
		{
			Matrix4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m[i][j] = m[j][i];
				}
			}
			return result;
		}

		[[nodiscard]] auto Determinant() const -> float
		{
			// Calculate the determinant using cofactor expansion
			float det = 0.0F;
			det += m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
							  m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
							  m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
			det -= m[0][1] * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
							  m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
							  m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
			det += m[0][2] * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
							  m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
							  m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
			det -= m[0][3] * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
							  m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
							  m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
			return det;
		}

		[[nodiscard]] auto Inverse() const -> Matrix4
		{
			Matrix4 result;
			float det = Determinant();
			if (det == 0.0F)
			{
				// Return identity matrix if not invertible
				return Identity();
			}
			float invDet = 1.0F / det;

			// Calculate cofactors and adjugate matrix
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					// Minor matrix determinant
					float minor[3][3];
					int rowIndex = 0;
					int colIndex = 0;
					for (int row = 0; row < 4; ++row)
					{
						if (row == i)
						{
							continue;
						}
						colIndex = 0;
						for (int col = 0; col < 4; ++col)
						{
							if (col == j)
							{
								continue;
							}
							minor[rowIndex][colIndex] = m[row][col];
							++colIndex;
						}
						++rowIndex;
					}
					float cofactor =
						((i + j) % 2 == 0 ? 1.0F : -1.0F) *
						(minor[0][0] *
							 (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1]) -
						 minor[0][1] *
							 (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0]) +
						 minor[0][2] *
							 (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]));
					result.m[j][i] = cofactor * invDet; // Transpose of cofactor matrix
				}
			}
			return result;
		}

		static auto Translation(float x, float y, float z) -> Matrix4
		{
			Matrix4 result = Identity();
			result.m[0][3] = x;
			result.m[1][3] = y;
			result.m[2][3] = z;
			return result;
		}

		static auto Translation(const Vector3& translation) -> Matrix4
		{
			Matrix4 result = Identity();
			result.m[0][3] = translation.x;
			result.m[1][3] = translation.y;
			result.m[2][3] = translation.z;
			return result;
		}

		static auto Scale(float x, float y, float z) -> Matrix4
		{
			Matrix4 result = Identity();
			result.m[0][0] = x;
			result.m[1][1] = y;
			result.m[2][2] = z;
			return result;
		}

		static auto Scale(const Vector3& scale) -> Matrix4
		{
			return Scale(scale.x, scale.y, scale.z);
		}

		static auto Rotation(const Quaternion& quaternion) -> Matrix4
		{
			Matrix4 result = Identity();
			float x = quaternion.x;
			float y = quaternion.y;
			float z = quaternion.z;
			float w = quaternion.w;

			float xx = 2 * x * x;
			float yy = 2 * y * y;
			float zz = 2 * z * z;
			float xy = 2 * x * y;
			float xz = 2 * x * z;
			float yz = 2 * y * z;
			float xw = 2 * x * w;
			float yw = 2 * y * w;
			float zw = 2 * z * w;

			result.m[0][0] = 1 - yy - zz;
			result.m[0][1] = xy - zw;
			result.m[0][2] = xz + yw;

			result.m[1][0] = xy + zw;
			result.m[1][1] = 1 - xx - zz;
			result.m[1][2] = yz - xw;

			result.m[2][0] = xz - yw;
			result.m[2][1] = yz + xw;
			result.m[2][2] = 1 - xx - yy;

			return result;
		}

		static auto Perspective(float fov, float aspect, float near, float far) -> Matrix4
		{
			Matrix4 result;
			float tanHalfFov = std::tan(fov / 2.0F);

			result.m[0][0] = 1.0F / (aspect * tanHalfFov);
			result.m[1][1] = 1.0F / tanHalfFov;
			result.m[2][2] = -(far + near) / (far - near);
			result.m[2][3] = -1.0F;
			result.m[3][2] = -(2.0F * far * near) / (far - near);
			result.m[3][3] = 0.0F;

			return result;
		}

		static auto Orthographic(float left, float right, float bottom, float top,
								 float near, float far) -> Matrix4
		{
			Matrix4 result = Identity();

			result.m[0][0] = 2.0F / (right - left);
			result.m[1][1] = 2.0F / (top - bottom);
			result.m[2][2] = -2.0F / (far - near);

			result.m[3][0] = -(right + left) / (right - left);
			result.m[3][1] = -(top + bottom) / (top - bottom);
			result.m[3][2] = -(far + near) / (far - near);

			return result;
		}

		static auto LookAt(const Vector4& eye, const Vector4& center, const Vector4& up)
			-> Matrix4
		{
			Vector4 f = (center - eye).Normalized();
			Vector4 s = f.Cross(up.Normalized()).Normalized();
			Vector4 u = s.Cross(f);

			Matrix4 result = Identity();
			result.m[0][0] = s.x;
			result.m[1][0] = s.y;
			result.m[2][0] = s.z;

			result.m[0][1] = u.x;
			result.m[1][1] = u.y;
			result.m[2][1] = u.z;

			result.m[0][2] = -f.x;
			result.m[1][2] = -f.y;
			result.m[2][2] = -f.z;

			result.m[3][0] = -s.Dot(eye);
			result.m[3][1] = -u.Dot(eye);
			result.m[3][2] = f.Dot(eye);

			return result;
		}

		auto ToFloatArray() const -> std::array<float, 16>
		{
			std::array<float, 16> result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result[(j * 4) + i] = m[i][j]; // Column-major order
				}
			}
			return result;
		}
	};
}