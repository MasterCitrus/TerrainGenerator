#pragma once

namespace Math
{
	struct Vec3
	{
		float x, y, z;

		Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
		inline Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		float& operator[](size_t index);
		const float& operator[](size_t index) const;

		Vec3 operator-() const;

		Vec3 operator+(const Vec3& other) const;
		Vec3 operator-(const Vec3& other) const;
		Vec3 operator*(float scalar) const;
		Vec3 operator/(float scalar) const;

		Vec3& operator+=(const Vec3& other);
		Vec3& operator-=(const Vec3& other);
		Vec3& operator*=(float scalar);
		Vec3& operator/=(float scalar);

		bool operator==(const Vec3& other) const;
		bool operator!=(const Vec3& other) const;

		float Length() const;
		float LengthSqr() const;

		float Distance(const Vec3& other) const;
		float DistanceSqr(const Vec3& other) const;

		float Dot(const Vec3& other) const;

		void Normalise();

		Vec3 Normalised() const;

		Vec3 Cross(const Vec3& other) const;

		static Vec3 Cross(const Vec3& a, const Vec3& b);
	};
}
