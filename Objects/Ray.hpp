
#pragma once

#include "Utils/Vector3.hpp"
#include "Utils/Color.hpp"

class Ray
{
    public:

        Vector3 orig, dir;
        float cast_time;

        Ray() = default;

        Ray(const Vector3& orig, const Vector3& dir, float time = 0.f):
                orig(orig), dir(dir), cast_time(time) {}

        Vector3 at(float t) const
        {
            return orig + dir * t;
        }
};