#include "terraingen/types/math/Mat4.h"
#include "terraingen/types/math/Utils.h"

#include <cmath>
#include <numbers>

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

Mat4 Math::Mat4::Inverse() const
{
    Mat4 result;
    float det;

    float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
    float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
    float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
    float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

    result[0][0] = m11 * (m22 * m33 - m23 * m32) - m21 * (m12 * m33 - m13 * m32) + m31 * (m12 * m23 - m13 * m22);
    result[0][1] = -m01 * (m22 * m33 - m23 * m32) + m21 * (m02 * m33 - m03 * m32) - m31 * (m02 * m23 - m03 * m22);
    result[0][2] = m01 * (m12 * m33 - m13 * m32) - m11 * (m02 * m33 - m03 * m32) + m31 * (m02 * m13 - m03 * m12);
    result[0][3] = -m01 * (m12 * m23 - m13 * m22) + m11 * (m02 * m23 - m03 * m22) - m21 * (m02 * m13 - m03 * m12);

    result[1][0] = -m10 * (m22 * m33 - m23 * m32) + m20 * (m12 * m33 - m13 * m32) - m30 * (m12 * m23 - m13 * m22);
    result[1][1] = m00 * (m22 * m33 - m23 * m32) - m20 * (m02 * m33 - m03 * m32) + m30 * (m02 * m23 - m03 * m22);
    result[1][2] = -m00 * (m12 * m33 - m13 * m32) + m10 * (m02 * m33 - m03 * m32) - m30 * (m02 * m13 - m03 * m12);
    result[1][3] = m00 * (m12 * m23 - m13 * m22) - m10 * (m02 * m23 - m03 * m22) + m20 * (m02 * m13 - m03 * m12);

    result[2][0] = m10 * (m21 * m33 - m23 * m31) - m20 * (m11 * m33 - m13 * m31) + m30 * (m11 * m23 - m13 * m21);
    result[2][1] = -m00 * (m21 * m33 - m23 * m31) + m20 * (m01 * m33 - m03 * m31) - m30 * (m01 * m23 - m03 * m21);
    result[2][2] = m00 * (m11 * m33 - m13 * m31) - m10 * (m01 * m33 - m03 * m31) + m30 * (m01 * m13 - m03 * m11);
    result[2][3] = -m00 * (m11 * m23 - m13 * m21) + m10 * (m01 * m23 - m03 * m21) - m20 * (m01 * m13 - m03 * m11);

    result[3][0] = -m10 * (m21 * m32 - m22 * m31) + m20 * (m11 * m32 - m12 * m31) - m30 * (m11 * m22 - m12 * m21);
    result[3][1] = m00 * (m21 * m32 - m22 * m31) - m20 * (m01 * m32 - m02 * m31) + m30 * (m01 * m22 - m02 * m21);
    result[3][2] = -m00 * (m11 * m32 - m12 * m31) + m10 * (m01 * m32 - m02 * m31) - m30 * (m01 * m12 - m02 * m11);
    result[3][3] = m00 * (m11 * m22 - m12 * m21) - m10 * (m01 * m22 - m02 * m21) + m20 * (m01 * m12 - m02 * m11);

    det = m00 * result[0][0] + m01 * result[1][0] + m02 * result[2][0] + m03 * result[3][0];

    if (det == 0.0f)
        return Identity();

    float invDet = 1.0f / det;

    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            result[col][row] *= invDet;

    return result;
}

Mat4 Math::Mat4::Translate(const Vec3& translation)
{
    return Mat4(1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                translation.x, translation.y, translation.z, 1.f);
}

Mat4 Math::Mat4::Rotate(const Vec3& axis, float radians)
{
    float c = std::cos(radians);
    float s = std::sin(radians);
    float t = 1.0f - c;

    return Mat4(t * axis.x * axis.x + c, t * axis.x * axis.y + s * axis.z, t * axis.x * axis.z - s * axis.y, 0.f,
                t * axis.x * axis.y - s * axis.z, t * axis.y * axis.y + c, t * axis.y * axis.z - s * axis.x, 0.f,
                t * axis.x * axis.z + s * axis.y, t * axis.y * axis.z - s * axis.x, t * axis.z * axis.z + c, 0.f,
                0.f,                              0.f,                              0.f,                     1.f);
}

Mat4 Math::Mat4::Scale(const Vec3& scale)
{
    return Mat4(scale.x, 0.f, 0.f, 0.f,
                0.f, scale.y, 0.f, 0.f,
                0.f, 0.f, scale.z, 0.f,
                0.f, 0.f, 0.f, 1.f);
}

Mat4 Math::Mat4::Perspective(float fov, float aspect, float near, float far) const
{
    float fovR = Deg2Rad(fov);
    float f = 1.0f / std::tan(fovR * 0.5f);
    float range = near - far;
    return Mat4(1.0f / (f * aspect), 0.f, 0.f, 0.f,
                0.f, f, 0.f, 0.f,
                0.f, 0.f, (-near - far) / range, -1.f,
                0.f, 0.f, (2 * far * near) / range, 0.f);
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