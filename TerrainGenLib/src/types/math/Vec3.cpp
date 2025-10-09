#include "types/math/Vec3.h"

#include <cmath>

using namespace Math;

const float THRESHOLD = 0.00001f;

float& Vec3::operator[](size_t index)
{
    return *(&x + index);
}
const float& Vec3::operator[](size_t index) const
{
    return *(&x + index);
}

Vec3 Vec3::operator+(const Vec3& other) const
{
    return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
}

Vec3 Vec3::operator-(const Vec3& other) const
{
    return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
}

Vec3 Vec3::operator*(float scalar) const
{
    return Vec3(this->x * scalar, this->y * scalar, this->z * scalar);
}

Vec3 Vec3::operator/(float scalar) const
{
    return Vec3(this->x / scalar, this->y / scalar, this->z / scalar);
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3& Vec3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vec3& Vec3::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

bool Math::Vec3::operator==(const Vec3& other) const
{
    float xDist = std::abs(x - other.x);
    float yDist = std::abs(y - other.y);
    float zDist = std::abs(z - other.z);

    return xDist < THRESHOLD && yDist < THRESHOLD && zDist < THRESHOLD;
}

bool Math::Vec3::operator!=(const Vec3& other) const
{
    return !(*this == other);
}

float Vec3::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Math::Vec3::LengthSqr() const
{
    return Dot(*this);
}

float Math::Vec3::Distance(const Vec3& other) const
{
    return (*this - other).Length();
}

float Math::Vec3::DistanceSqr(const Vec3& other) const
{
    return (*this - other).LengthSqr();
}

float Math::Vec3::Dot(const Vec3& other) const
{
    return (x * other.x + y * other.y) + z * other.z;
}

void Math::Vec3::Normalise()
{
    float m = Length();
    if (m == 0.0f) x = y = 0.0f;
    else
    {
        x /= m;
        y /= m;
        z /= m;
    }
}

Vec3 Math::Vec3::Normalised() const
{
    Vec3 temp = *this;
    temp.Normalise();

    return temp;
}

Vec3 Math::Vec3::Cross(const Vec3& other) const
{
    return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}
