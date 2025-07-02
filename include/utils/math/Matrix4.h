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
		float M[4][4];

		Matrix4()
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					M[i][j] = (i == j) ? 1.0F : 0.0F;
				}
			}
		};

		Matrix4(const float m[4][4])
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					M[i][j] = m[i][j];
				}
			}
		};

		Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2,
				const Vector4& col3)
		{
			M[0][0] = col0.X;
			M[1][0] = col0.Y;
			M[2][0] = col0.Z;
			M[3][0] = col0.W;

			M[0][1] = col1.X;
			M[1][1] = col1.Y;
			M[2][1] = col1.Z;
			M[3][1] = col1.W;

			M[0][2] = col2.X;
			M[1][2] = col2.Y;
			M[2][2] = col2.Z;
			M[3][2] = col2.W;

			M[0][3] = col3.X;
			M[1][3] = col3.Y;
			M[2][3] = col3.Z;
			M[3][3] = col3.W;
		};

		auto operator*(const Matrix4& other) const -> Matrix4
		{
			Matrix4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.M[i][j] = M[i][0] * other.M[0][j] + M[i][1] * other.M[1][j] +
									 M[i][2] * other.M[2][j] + M[i][3] * other.M[3][j];
				}
			}
			return result;
		};

		auto operator*(const Vector4& vec) const -> Vector4
		{
			return {(M[0][0] * vec.X) + (M[1][0] * vec.Y) + (M[2][0] * vec.Z) +
						(M[3][0] * vec.W),
					(M[0][1] * vec.X) + (M[1][1] * vec.Y) + (M[2][1] * vec.Z) +
						(M[3][1] * vec.W),
					(M[0][2] * vec.X) + (M[1][2] * vec.Y) + (M[2][2] * vec.Z) +
						(M[3][2] * vec.W),
					(M[0][3] * vec.X) + (M[1][3] * vec.Y) + (M[2][3] * vec.Z) +
						(M[3][3] * vec.W)};
		};

		auto operator*(float scalar) const -> Matrix4
		{
			Matrix4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.M[i][j] = M[i][j] * scalar;
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
					result.M[i][j] = M[i][j] / scalar;
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
					if (M[i][j] != other.M[i][j])
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
					result.M[i][j] = M[j][i];
				}
			}
			return result;
		}

		[[nodiscard]] auto Determinant() const -> float
		{
			// Calculate the determinant using cofactor expansion
			float det = 0.0F;
			det += M[0][0] * (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
							  M[1][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) +
							  M[1][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]));
			det -= M[0][1] * (M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
							  M[1][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) +
							  M[1][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]));
			det += M[0][2] * (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
							  M[1][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) +
							  M[1][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
			det -= M[0][3] * (M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
							  M[1][1] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]) +
							  M[1][2] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
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
							minor[rowIndex][colIndex] = M[row][col];
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
					result.M[j][i] = cofactor * invDet; // Transpose of cofactor matrix
				}
			}
			return result;
		}

		static auto Translation(float x, float y, float z) -> Matrix4
		{
			Matrix4 result = Identity();
			result.M[0][3] = x;
			result.M[1][3] = y;
			result.M[2][3] = z;
			return result;
		}

		static auto Translation(const Vector3& translation) -> Matrix4
		{
			Matrix4 result = Identity();
			result.M[0][3] = translation.X;
			result.M[1][3] = translation.Y;
			result.M[2][3] = translation.Z;
			return result;
		}

		static auto Scale(float x, float y, float z) -> Matrix4
		{
			Matrix4 result = Identity();
			result.M[0][0] = x;
			result.M[1][1] = y;
			result.M[2][2] = z;
			return result;
		}

		static auto Scale(const Vector3& scale) -> Matrix4
		{
			return Scale(scale.X, scale.Y, scale.Z);
		}

		static auto Rotation(const Quaternion& quaternion) -> Matrix4
		{
			Matrix4 result = Identity();
			float x = quaternion.X;
			float y = quaternion.Y;
			float z = quaternion.Z;
			float w = quaternion.W;

			float xx = 2 * x * x;
			float yy = 2 * y * y;
			float zz = 2 * z * z;
			float xy = 2 * x * y;
			float xz = 2 * x * z;
			float yz = 2 * y * z;
			float xw = 2 * x * w;
			float yw = 2 * y * w;
			float zw = 2 * z * w;

			result.M[0][0] = 1 - yy - zz;
			result.M[0][1] = xy - zw;
			result.M[0][2] = xz + yw;

			result.M[1][0] = xy + zw;
			result.M[1][1] = 1 - xx - zz;
			result.M[1][2] = yz - xw;

			result.M[2][0] = xz - yw;
			result.M[2][1] = yz + xw;
			result.M[2][2] = 1 - xx - yy;

			return result;
		}

		static auto Perspective(float fov, float aspect, float near, float far) -> Matrix4
		{
			Matrix4 result;
			float tanHalfFov = std::tan(fov / 2.0F);

			result.M[0][0] = 1.0F / (aspect * tanHalfFov);
			result.M[1][1] = 1.0F / tanHalfFov;
			result.M[2][2] = -(far + near) / (far - near);
			result.M[2][3] = -1.0F;
			result.M[3][2] = -(2.0F * far * near) / (far - near);
			result.M[3][3] = 0.0F;

			return result;
		}

		static auto Orthographic(float left, float right, float bottom, float top,
								 float near, float far) -> Matrix4
		{
			Matrix4 result = Identity();

			result.M[0][0] = 2.0F / (right - left);
			result.M[1][1] = 2.0F / (top - bottom);
			result.M[2][2] = -2.0F / (far - near);

			result.M[3][0] = -(right + left) / (right - left);
			result.M[3][1] = -(top + bottom) / (top - bottom);
			result.M[3][2] = -(far + near) / (far - near);

			return result;
		}

		static auto LookAt(const Vector4& eye, const Vector4& center, const Vector4& up)
			-> Matrix4
		{
			Vector4 f = (center - eye).Normalized();
			Vector4 s = f.Cross(up.Normalized()).Normalized();
			Vector4 u = s.Cross(f);

			Matrix4 result = Identity();
			result.M[0][0] = s.X;
			result.M[1][0] = s.Y;
			result.M[2][0] = s.Z;

			result.M[0][1] = u.X;
			result.M[1][1] = u.Y;
			result.M[2][1] = u.Z;

			result.M[0][2] = -f.X;
			result.M[1][2] = -f.Y;
			result.M[2][2] = -f.Z;

			result.M[3][0] = -s.Dot(eye);
			result.M[3][1] = -u.Dot(eye);
			result.M[3][2] = f.Dot(eye);

			return result;
		}

		auto ToFloatArray() const -> std::array<float, 16>
		{
			std::array<float, 16> result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result[(j * 4) + i] = M[i][j]; // Column-major order
				}
			}
			return result;
		}
	};
}