
#pragma once

#include "Objects/Ray.hpp"

namespace Ilya
{
    /// Bounding boxes for hittables and sets of hittables, to better
    /// determine ray paths in the scene (see BVHnode).
    class Bounds
    {
        public:

            Bounds() = default;
            Bounds(const Point3& a, const Point3& b):
                    min(a), max(b) {}

            /// Is the bounding box hit by the ray `r` between `tmin`
            /// and `tmax` ?
            bool hit(const Ray& r, float tmin, float tmax) const;

        public:

            Point3 min, max;
    };

    Bounds surrounding_box(const Bounds& b1, const Bounds& b2);
}