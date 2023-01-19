
#pragma once

#include "Objects/Ray.hpp"

namespace Ilya
{
    /// Bounding boxes for hittables and sets of hittables, to better
    /// determine ray paths in the scene (see BVHnode).
    class BoundingBox
    {
        public:

            BoundingBox() = default;
            BoundingBox(const Vec3& a, const Vec3& b):
                    min(a), max(b) {}

            /// Is the bounding box hit by the ray `r` between `tmin`
            /// and `tmax` ?
            bool hit(const Ray& r, float tmin, float tmax) const;

        public:

            Vec3 min, max;
    };

    BoundingBox surrounding_box(const BoundingBox& b1, const BoundingBox& b2);
}