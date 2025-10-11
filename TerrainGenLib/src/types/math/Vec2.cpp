#include "types/math/Vec2.h"

#include <cmath>

using namespace Math;

const float THRESHOLD = 0.00001f;

float& Vec2::operator[](size_t index)
{
    return *(&x + index);
}
const float& Vec2::operator[](size_t index) const
{
    return *(&x + index);
}

Vec2 Vec2::operator-() const
{
    return Vec3(-x, -y, -z);
}

Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(this->x + other.x, this->y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(this->x - other.x, this->y - other.y);
}

Vec2 Vec2::operator*(float scalar) const
{
    return Vec2(this->x * scalar, this->y * scalar);
}

Vec2 Vec2::operator/(float scalar) const
{
    return Vec2(this->x / scalar, this->y / scalar);
}

Vec2& Vec2::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vec2& Vec2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2& Vec2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

bool Math::Vec2::operator==(const Vec2& other) const
{
    float xDist = std::abs(x - other.x);
    float yDist = std::abs(y - other.y);

    return xDist < THRESHOLD && yDist < THRESHOLD;
}

bool Math::Vec2::operator!=(const Vec2& other) const
{
    return !(*this == other);
}

float Vec2::Length() const
{
    return std::sqrt(x * x + y * y);
}

float Math::Vec2::LengthSqr() const
{
    return Dot(*this);
}

float Math::Vec2::Distance(const Vec2& other) const
{
    return (*this - other).Length();
}

float Math::Vec2::DistanceSqr(const Vec2& other) const
{
    return (*this - other).LengthSqr();
}

float Math::Vec2::Dot(const Vec2& other) const
{
    return (x * other.x + y * other.y);
}

void Math::Vec2::Normalise()
{
    float m = Length();
    if (m == 0.0f) x = y = 0.0f;
    else
    {
        x /= m;
        y /= m;
    }
}

Vec2 Math::Vec2::Normalised() const
{
    Vec2 temp = *this;
    temp.Normalise();

    return temp;
}

Vec2 Math::Vec2::Perpendicular() const
{
    return Vec2(-y, x);
}
