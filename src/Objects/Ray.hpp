
#pragma once

#include "Utils/Math.hpp"
#include "Utils/Color.hpp"

namespace Ilya
{
    class Ray
    {
        public:

            Ray() = default;
            Ray(const Vec3& orig, const Vec3& dir, float time = 0.f):
                    orig(orig), dir(dir), cast_time(time) {}

            Vec3 at(float t) const
            {
                return orig + dir * t;
            }

        public:

            Vec3 orig, dir;
            float cast_time;
    };
}