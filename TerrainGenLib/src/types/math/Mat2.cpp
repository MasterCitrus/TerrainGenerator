#include "types/math/Mat2.h"

#include <cmath>

using namespace Math;

Mat2 Math::Mat2::Identity() const
{
    return Mat2();
}

Mat2 Math::Mat2::Transpose()
{
    // 00 01 ----> 00 10
    // 10 11 ----> 01 11
    return Mat2(m[0][0], m[1][0], m[0][1], m[1][1]);
}

Mat2 Math::Mat2::Rotate(float radians)
{
    float c = std::cos(radians);
    float s = std::sin(radians);

    return Mat2(c, s,
                -s, c);
}

Mat2 Math::Mat2::Scale(const Vec2& scale)
{
    return Mat2(scale.x, 0.f,
                0.f, scale.y);
}

float Math::Mat2::Determinant() const
{
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}

Mat2 Math::Mat2::operator*(const Mat2& other)
{
    Mat2 result;
    for (int i = 0; i < 2; i++)
    {
        Vec2 row = { m[0][i], m[1][i] };
        for (int j = 0; j < 2; j++)
        {
            result.m[j][i] = row.Dot(other.axis[j]);
        }
    }
    return result;
}

Vec2 Math::Mat2::operator*(const Vec2& other)
{
    return Vec2(axis[0].Dot(other),
                axis[1].Dot(other));
}