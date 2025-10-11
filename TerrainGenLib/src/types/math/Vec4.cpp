#include "types/math/Vec4.h"

#include <cmath>

using namespace Math;

const float THRESHOLD = 0.00001f;

float& Vec4::operator[](size_t index)
{
    return *(&x + index);
}
const float& Vec4::operator[](size_t index) const
{
    return *(&x + index);
}

Vec4 Vec4::operator-() const
{
    return Vec4(-x, -y, -z, -w);
}

Vec4 Vec4::operator+(const Vec4& other) const
{
    return Vec4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
}

Vec4 Vec4::operator-(const Vec4& other) const
{
    return Vec4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
}

Vec4 Vec4::operator*(float scalar) const
{
    return Vec4(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar);
}

Vec4 Vec4::operator/(float scalar) const
{
    return Vec4(this->x / scalar, this->y / scalar, this->z / scalar, this->w / scalar);
}

Vec4& Vec4::operator+=(const Vec4& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vec4& Vec4::operator-=(const Vec4& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vec4& Vec4::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vec4& Vec4::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

bool Math::Vec4::operator==(const Vec4& other) const
{
    float xDist = std::abs(x - other.x);
    float yDist = std::abs(y - other.y);
    float zDist = std::abs(z - other.z);
    float wDist = std::abs(w - other.w);

    return xDist < THRESHOLD && yDist < THRESHOLD && zDist < THRESHOLD && wDist < THRESHOLD;
}

bool Math::Vec4::operator!=(const Vec4& other) const
{
    return !(*this == other);
}

float Vec4::Length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Math::Vec4::LengthSqr() const
{
    return Dot(*this);
}

float Math::Vec4::Distance(const Vec4& other) const
{
    return (*this - other).Length();
}

float Math::Vec4::DistanceSqr(const Vec4& other) const
{
    return (*this - other).LengthSqr();
}

float Math::Vec4::Dot(const Vec4& other) const
{
    return (x * other.x + y * other.y + z * other.z + w * other.w);
}

void Math::Vec4::Normalise()
{
    float m = Length();
    if (m == 0.0f) x = y = 0.0f;
    else
    {
        x /= m;
        y /= m;
        z /= m;
        w /= m;
    }
}

Vec4 Math::Vec4::Normalised() const
{
    Vec4 temp = *this;
    temp.Normalise();

    return temp;
}

Vec4 Math::Vec4::Cross(const Vec4& other) const
{
    return Vec4(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x, 0);
}
