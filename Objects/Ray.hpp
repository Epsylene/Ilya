
#pragma once

#include "../Utils/Vector3.hpp"
#include "../Utils/Color.hpp"

class Ray
{
    public:

        Vector3 orig, dir;

        Ray() = default;

        Ray(const Vector3& orig, const Vector3& dir):
            orig(orig), dir(dir) {}

        Vector3 at(float t)
        {
            return orig + dir * t;
        }
};