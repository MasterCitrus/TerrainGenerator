#include "terraingen/types/math/Curve.h"

#include <algorithm>

Curve::Curve()
{
    keys.push_back({ 0.0f, 0.0f, 0.0f, 0.0f });
    keys.push_back({ 1.0f, 1.0f, 0.0f, 0.0f });
}

float Curve::Evaluate(float time) const
{
    if (keys.empty()) return 0.0f;
    if (time <= keys.front().time) return keys.front().value;
    if (time >= keys.back().time) return keys.back().value;

    for (size_t i = 0; i < keys.size() - 1; i++)
    {
        if (time >= keys[i].time && time <= keys[i + 1].time)
        {
            float t = (time - keys[i].time) / (keys[i + 1].time - keys[i].time);

            // Hermite interpolation
            float p0 = keys[i].value;
            float p1 = keys[i + 1].value;
            float m0 = keys[i].outTangent * (keys[i + 1].time - keys[i].time);
            float m1 = keys[i + 1].inTangent * (keys[i + 1].time - keys[i].time);

            float t2 = t * t;
            float t3 = t2 * t;

            return (2 * t3 - 3 * t2 + 1) * p0 + (t3 - 2 * t2 + t) * m0 +
                (-2 * t3 + 3 * t2) * p1 + (t3 - t2) * m1;
        }
    }
    return 0.0f;
}

void Curve::AddKey(const CurveKey& key)
{
    keys.push_back(key);

    if (keys.size() > 1)
    {
        std::sort(keys.begin(), keys.end());
    }
}
