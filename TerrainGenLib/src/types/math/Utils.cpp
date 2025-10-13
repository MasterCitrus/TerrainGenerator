#include "types/math/Utils.h"

#include <numbers>

namespace Math
{
    float Deg2Rad(float degrees)
    {
        return degrees * (std::numbers::pi / 180.0f);
    }

    float Rad2Deg(float radians)
    {
        return radians * (180.0f / std::numbers::pi);
    }

    float Lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    float InverseLerp(float a, float b, float value)
    {
        return (value - a) / (b - a);
    }
}
