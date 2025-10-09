#pragma once

namespace Math
{
	struct Vec2
	{
		float x, y;

		Vec2 operator+(const Vec2& other) const;
		Vec2 operator-(const Vec2& other) const;
		Vec2 operator*(float scalar) const;
		Vec2 operator/(float scalar) const;

		Vec2& operator+=(const Vec2& other);
		Vec2& operator-=(const Vec2& other);
		Vec2& operator*=(float scalar);
		Vec2& operator/=(float scalar);

		bool operator==(const Vec2& other) const;
		bool operator!=(const Vec2& other) const;

		float Length() const;
		float LengthSqr() const;

		float Distance(const Vec2& other) const;
		float DistanceSqr(const Vec2& other) const;

		float Dot(const Vec2& other) const;

		void Normalise();

		Vec2 Normalised() const;
	};
}
