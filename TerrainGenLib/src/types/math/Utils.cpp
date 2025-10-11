#include "types/math/Utils.h"

#include <numbers>

float Deg2Rad(float degrees)
{
    return degrees * (std::numbers::pi / 180.0f);
}

float Rad2Deg(float radians)
{
    return radians * (180.0f / std::numbers::pi);
}
