#include "types/math/Mat4.h"

#include <cmath>

using namespace Math;

Mat4 Math::Mat4::Identity() const
{
    return Mat4();
}

Mat4 Math::Mat4::Transpose()
{
    // 00 01 02 03 ----> 00 10 20 30
    // 10 11 12 13 ----> 01 11 21 31
    // 20 21 22 23 ----> 02 12 22 32
    // 30 31 32 33 ----> 03 13 23 33
    return Mat4(m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
}

Mat4 Math::Mat4::Translate(const Vec4& translation)
{
    return Mat4(1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                translation.x, translation.y, translation.z, 1.f);
}

Mat4 Math::Mat4::Rotate(const Vec4& axis, float radians)
{
    float c = std::cos(radians);
    float s = std::sin(radians);
    float t = 1.0f - c;

    return Mat4(t * axis.x * axis.x + c, t * axis.x * axis.y + s * axis.z, t * axis.x * axis.z - s * axis.y, 0.f,
                t * axis.x * axis.y - s * axis.z, t * axis.y * axis.y + c, t * axis.y * axis.z - s * axis.x, 0.f,
                t * axis.x * axis.z + s * axis.y, t * axis.y * axis.z - s * axis.x, t * axis.z * axis.z + c, 0.f,
                0.f,                              0.f,                              0.f,                     1.f);
}

Mat4 Math::Mat4::Scale(const Vec4& scale)
{
    return Mat4(scale.x, 0.f, 0.f, 0.f,
                0.f, scale.y, 0.f, 0.f,
                0.f, 0.f, scale.z, 0.f,
                0.f, 0.f, 0.f, 1.f);
}

Mat4 Math::Mat4::Perspective(float fov, float aspect, float near, float far) const
{
    float f = 1.0f / std::tan(fov * 0.5f);
    float nf = 1.0f / (near - far);
    return Mat4(f / aspect, 0.f, 0.f, 0.f,
                0.f, f, 0.f, 0.f,
                0.f, 0.f, (far + near) * nf, -1.f,
                0.f, 0.f, 2 * far * near * nf, 0.f);
}

Mat4 Math::Mat4::Orthographic(float left, float right, float bottom, float top, float near, float far) const
{
    float rl = 1.0f / (right - left);
    float tb = 1.0f / (top - bottom);
    float fn = 1.0f / (far - near);
    return Mat4(2 * rl, 0.f, 0.f, 0.f,
                0.f, 2 * tb, 0.f, 0.f,
                0.f, 0.f, -2 * fn, 0.f,
                -(right + left) * rl, -(top + bottom) * tb, -(far + near) * fn, 1.f);
}

float Math::Mat4::Determinant() const
{
    // TODO fix
    float a00 = m[0][0], a10 = m[1][0], a20 = m[2][0];
    float a01 = m[0][1], a11 = m[1][1], a21 = m[2][1];
    float a02 = m[0][2], a12 = m[1][2], a22 = m[2][2];

    return a00 * (a11 * a22 - a12 * a21)
        - a10 * (a01 * a22 - a02 * a21)
        + a20 * (a01 * a12 - a02 * a11);
}

Mat4 Math::Mat4::operator*(const Mat4& other)
{
    Mat4 result;
    for (int i = 0; i < 4; i++)
    {
        Vec4 row = { m[0][i], m[1][i], m[2][i], m[3][i]};
        for (int j = 0; j < 4; j++)
        {
            result.m[j][i] = row.Dot(other.axis[j]);
        }
    }
    return result;
}

Vec4 Math::Mat4::operator*(const Vec4& other)
{
    return Vec4(axis[0].Dot(other),
                axis[1].Dot(other),
                axis[2].Dot(other),
                axis[3].Dot(other));
}