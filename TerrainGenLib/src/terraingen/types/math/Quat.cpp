#include "terraingen/types/math/Quat.h"

#include <cmath>

using namespace Math;

Quat::Quat(float yaw, float pitch, float roll)
{
	float hy = yaw * 0.5f;
	float hp = pitch * 0.5f;
	float hr = roll * 0.5f;
	float cy = std::cos(hy), sy = std::sin(hy);
	float cp = std::cos(hp), sp = std::sin(hp);
	float cr = std::cos(hr), sr = std::sin(hr);

	w = cy * cp * cr + sy * sp * sr;
	x = cy * sp * cr + sy * cp * sr;
	y = sy * cp * cr - cy * sp * sr;
	z = cy * cp * sr - sy * sp * cr;

	this->Normalise();
}

Quat Quat::Normalised() const
{
	Quat temp = *this;
	return temp.Normalise();
}

Quat& Quat::Normalise()
{
	float len = std::sqrt(w * w + x * x + y * y + z * z);
	w /= len;
	x /= len;
	y /= len;
	z /= len;

	return *this;
}

Quat Math::Quat::Conjugate() const
{
	return Quat(w, -x, -y, -z);
}

Quat Math::Quat::Inverse() const
{
	return Conjugate().Normalised();
}

Vec3 Math::Quat::Rotate(const Vec3& axis) const
{
	Quat p(0, axis.x, axis.y, axis.z);
	Quat result = (*this) * p * this->Inverse();
	return Vec3(result.x, result.y, result.z);
}

Mat3 Math::Quat::ToMat3() const
{
	Mat3 result;
	float xx = x * x, yy = y * y, zz = z * z;
	float xy = x * y, xz = x * z, yz = y * z;
	float wx = w * x, wy = w * y, wz = w * z;

	result[0][0] = 1 - 2 * (yy + zz);
	result[1][0] = 2 * (xy + wz);
	result[2][0] = 2 * (xz - wy);

	result[0][1] = 2 * (xy - wz);
	result[1][1] = 1 - 2 * (xx + zz);
	result[2][1] = 2 * (yz + wx);

	result[0][2] = 2 * (xz + wy);
	result[1][2] = 2 * (yz - wx);
	result[2][2] = 1 - 2 * (xx + yy);

	return result;
}

Mat4 Math::Quat::ToMat4() const
{
	Mat4 result;

	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	result.m[0][0] = 1.0f - 2.0f * (yy + zz);
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = 1.0f - 2.0f * (xx + zz);
	result.m[1][2] = 2.0f * (yz + wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = 1.0f - 2.0f * (xx + yy);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Quat Math::Quat::operator*(const Quat& other) const
{
	return Quat(w * other.w - x * other.x - y * other.y - z * other.z,
				w * other.x + x * other.w + y * other.z - z * other.y,
				w * other.y - x * other.z + y * other.w + z * other.x,
				w * other.z + x * other.w - y * other.x + z * other.w);
}
