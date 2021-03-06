
#pragma once

#include "Utils/Vector3.hpp"
#include "Objects/Ray.hpp"

namespace Ilya
{
    class BoundingBox
    {
        public:

            Vec3 min, max;

            BoundingBox() = default;
            BoundingBox(const Vec3& a, const Vec3& b):
                    min(a), max(b) {}

            bool hit(const Ray& r, float tmin, float tmax) const
            {
                // One method for calculating ray-object intersections
                // more efficiently is the axis-aligned bounding boxes
                // method, or AABB: objects are enclosed within a simple
                // paralelliped volume, for which intersections are much
                // easier to calculate; if the ray doesn't hit the bounding
                // volume, it won't hit the object either, so it can be
                // discarded.
                // To check if the ray hits the bounding volume, we can use
                // the slab method: first, calculate the times t0 and t1
                // at which the ray hits the volume's faces on the X axis.
                // Then intersect that interval with [tmin, tmax], calculate
                // [t0, t1] for the Y axis, intersect, etc. We will eventually
                // end up with a [tmin, tmax] interval such that tmin > tmax
                // if there is no collision (we can easily understand why
                // this is in two dimensions: picture a square with its sides
                // prolonged by lines. Any ray that does not hit the square
                // will pass through the Y-axis planes before the X-axis
                // planes; then given that the initial [tmin, tmax] interval
                // is big enough to contain all the time points -which
                // is a reasonable assumption; we are simply giving ourselves
                // enough time to hit all t0x, t1x, t0y, t1y time points-
                // then the first intersection will result in [t0x, t1x].
                // Then, when trying to intersect with [t0y, t1y], the new
                // interval is flipped, because t0y < t0x and t1y < t1x
                // results in [t0x, t1y], where t1y < t0x. The reasoning
                // stays the same in three dimensions, but with one more
                // intersection).
                for (int i = 0; i < 3; ++i)
                {
                    auto t0 = (min[i] - r.orig[i])/r.dir[i];
                    auto t1 = (max[i] - r.orig[i])/r.dir[i];
                    if(t1 < t0)
                        std::swap(t0, t1);

                    tmin = std::max(t0, tmin);
                    tmax = std::min(t1, tmax);

                    if(tmax <= tmin)
                        return false;
                }

                return true;
            }
    };

    BoundingBox surrounding_box(const BoundingBox& b1, const BoundingBox& b2);
}