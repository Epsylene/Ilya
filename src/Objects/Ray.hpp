
#pragma once

#include "Utils/Math.hpp"
#include "Utils/Color.hpp"

namespace Ilya
{
    class Ray
    {
        public:

            Ray() = default;
            Ray(const Point3& orig, const Vec3& dir, float time = 0.f):
                    orig(orig), dir(dir), cast_time(time) {}
            Ray(const Ray& r) = default;

            Point3 operator()(float t) const
            {
                return orig + dir * t;
            }

        public:

            Point3 orig;
            Vec3 dir;
            float cast_time;
    };
}