#include "terraingen/types/math/Mat3.h"

#include <cmath>

using namespace Math;

Mat3 Math::Mat3::Identity() const
{
    return Mat3();
}

Mat3 Math::Mat3::Transpose()
{
    // 00 01 02 ----> 00 10 20
    // 10 11 12 ----> 01 11 21
    // 20 21 22 ----> 02 12 22
    return Mat3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1], m[0][2], m[1][2], m[2][2]);
}

Mat3 Math::Mat3::Rotate(const Vec3& axis, float radians)
{
    float c = std::cos(radians);
    float s = std::sin(radians);
    float t = 1.0f - c;

    return Mat3(t * axis.x * axis.x + c, t * axis.x * axis.y + s * axis.z, t * axis.x * axis.z - s * axis.y,
                t * axis.x * axis.y - s * axis.z, t * axis.y * axis.y + c, t * axis.y * axis.z - s * axis.x,
                t * axis.x * axis.z + s * axis.y, t * axis.y * axis.z - s * axis.x, t * axis.z * axis.z + c);
}

Mat3 Math::Mat3::Scale(const Vec3& scale)
{
    return Mat3(scale.x, 0.f, 0.f,
                0.f, scale.y, 0.f,
                0.f, 0.f, scale.z);
}

float Math::Mat3::Determinant() const
{
    float a00 = m[0][0], a10 = m[1][0], a20 = m[2][0];
    float a01 = m[0][1], a11 = m[1][1], a21 = m[2][1];
    float a02 = m[0][2], a12 = m[1][2], a22 = m[2][2];

    return a00 * (a11 * a22 - a12 * a21)
        - a10 * (a01 * a22 - a02 * a21)
        + a20 * (a01 * a12 - a02 * a11);
}

Mat3 Math::Mat3::operator*(const Mat3& other)
{
    Mat3 result;
    for (int i = 0; i < 3; i++)
    {
        Vec3 row = { m[0][i], m[1][i], m[2][i] };
        for (int j = 0; j < 3; j++)
        {
            result.m[j][i] = row.Dot(other.axis[j]);
        }
    }
    return result;
}

Vec3 Math::Mat3::operator*(const Vec3& other)
{
    return Vec3(axis[0].Dot(other),
                axis[1].Dot(other),
                axis[2].Dot(other));
}
