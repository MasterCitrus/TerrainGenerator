#pragma once

#include "Vec3.h"

namespace Math
{
	struct Mat3
	{
		union
		{
			float m[3][3];
			Vec3 axis[3];
		};

		float* operator[](size_t col) { return m[col]; }
		const float* operator[](size_t col) const { return m[col]; }

		inline Mat3() : m{ {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f} } {}
		inline Mat3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
			: m{ {m00, m01, m02}, {m10, m11, m12}, {m20, m21, m22} } { }

		Mat3 Identity() const;
		Mat3 Transpose();

		// Normalised Vector is expected in input
		Mat3 Rotate(const Vec3& axis, float radians);
		Mat3 Scale(const Vec3& scale);

		float Determinant() const;

		Mat3 operator*(const Mat3& other);
		Vec3 operator*(const Vec3& other);
	};
}