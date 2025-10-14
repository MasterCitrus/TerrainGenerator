#pragma once

#include "Vec2.h"

namespace Math
{
	struct Mat2
	{
		union
		{
			float m[2][2];
			Vec2 axis[2];
		};

		inline Mat2() : m{ {1.f, 0.f}, {0.f, 1.f} } {}
		inline Mat2(float m00, float m01,float m10, float m11)
			: m{ {m00, m01}, {m10, m11} }
		{
		}

		float* operator[](size_t col) { return m[col]; }
		const float* operator[](size_t col) const { return m[col]; }

		Mat2 Identity() const;
		Mat2 Transpose();

		// Normalised Vector is expected in input
		Mat2 Rotate(float radians);
		Mat2 Scale(const Vec2& scale);

		float Determinant() const;

		Mat2 operator*(const Mat2& other);
		Vec2 operator*(const Vec2& other);
	};
}