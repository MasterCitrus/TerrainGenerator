#pragma once

namespace Math
{
	float Deg2Rad(float degrees);
	float Rad2Deg(float radians);

	float Lerp(float a, float b, float t);
	float InverseLerp(float a, float b, float value);
}
