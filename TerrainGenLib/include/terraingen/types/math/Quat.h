#pragma once

#include "Mat3.h"
#include "Mat4.h"

namespace Math
{
	struct Quat
	{
		float w, x, y, z;

		inline Quat() : w(1), x(0), y(0), z(0) {}
		inline Quat(float w, float x, float y, float z)
			: w(w), x(x), y(y), z(z) { }
		Quat(float yaw, float pitch, float roll);
		inline Quat(const Vec3& euler) : Quat(euler.y, euler.x, euler.z) {}

		Quat Normalised() const;
		Quat& Normalise();
		Quat Conjugate() const;
		Quat Inverse() const;

		Vec3 Rotate(const Vec3& axis) const;		

		Mat3 ToMat3() const;
		Mat4 ToMat4() const;

		Quat operator*(const Quat& other) const;
	};
}