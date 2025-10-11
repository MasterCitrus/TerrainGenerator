#pragma once

#include "Vec3.h"
#include "Vec4.h"

namespace Math
{
	struct Mat4
	{
		union
		{
			float m[4][4];
			Vec4 axis[4];
		};

		inline Mat4() : m{ {1.f, 0.f, 0.f, 0.f}, {0.f, 1.f, 0.f, 0.f}, {0.f, 0.f, 1.f, 0.f}, {0.f, 0.f, 0.f, 1.f } } {}
		inline Mat4(float m00, float m01, float m02, float m03,
					float m10, float m11, float m12, float m13,
					float m20, float m21, float m22, float m23,
					float m30, float m31, float m32, float m33)
			: m{ {m00, m01, m02, m03}, {m10, m11, m12, m13}, {m20, m21, m22, m23}, {m30, m31, m32, m33} }
		{
		}

		float* operator[](size_t col) { return m[col]; }
		const float* operator[](size_t col) const { return m[col]; }

		Mat4 Identity() const;
		Mat4 Transpose();
		Mat4 Inverse() const;

		// Normalised Vector is expected in input
		Mat4 Translate(const Vec3& translation);
		Mat4 Rotate(const Vec3& axis, float radians);
		Mat4 Scale(const Vec3& scale);

		Mat4 Perspective(float fov, float aspect, float near, float far) const;
		Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far) const;

		float Determinant() const;

		Mat4 operator*(const Mat4& other);
		Vec4 operator*(const Vec4& other);
	};
}