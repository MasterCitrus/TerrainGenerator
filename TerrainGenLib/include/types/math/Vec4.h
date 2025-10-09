#pragma once

namespace Math
{
	struct Vec4
	{
		float x, y, z, w;

		Vec4() = default;
		inline Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		float& operator[](size_t index);
		const float& operator[](size_t index) const;

		Vec4 operator+(const Vec4& other) const;
		Vec4 operator-(const Vec4& other) const;
		Vec4 operator*(float scalar) const;
		Vec4 operator/(float scalar) const;

		Vec4& operator+=(const Vec4& other);
		Vec4& operator-=(const Vec4& other);
		Vec4& operator*=(float scalar);
		Vec4& operator/=(float scalar);

		bool operator==(const Vec4& other) const;
		bool operator!=(const Vec4& other) const;

		float Length() const;
		float LengthSqr() const;

		float Distance(const Vec4& other) const;
		float DistanceSqr(const Vec4& other) const;

		float Dot(const Vec4& other) const;

		void Normalise();

		Vec4 Normalised() const;

		Vec4 Cross(const Vec4& other) const;
	};
}
