
#pragma once

#include "Math/geometry.hpp"

namespace Ilya
{
    class Interaction
    {
        public:

            Interaction(const Point3& p, float time):
                p(p), t(time), dir() {}

            Interaction(const Point3& p, const Vec3& dir, float time):
                    p(p), dir(dir), t(time) {}

        public:

            Point3 p;
            Vec3 dir;
            float t;
    };

    struct SurfaceElement
    {
        // A surface element is defined by a normal
        // at a point (n), the derivatives of the
        // point in the local coordinates of the
        // surface (dpdu, dpdv), and the differential
        // change of the normal when moving the point
        // (dndu, dndv).
        Normal n;
        Normal dndu, dndv;
        Vec3 dpdu, dpdv;

        SurfaceElement(const Vec3& dpdu, const Vec3& dpdv,
                       const Normal& dndu, const Normal& dndv):
                       dpdu(dpdu), dpdv(dpdv), dndu(dndu),
                       dndv(dndv), n(normalize(cross(dpdu, dpdv)))
        {}
    };

    class SurfaceInteraction: public Interaction
    {
        public:

            SurfaceInteraction(const Point3& p, const Vec3& dir, const Point2& uv,
                               const SurfaceElement& element, float time):
                    Interaction(p, dir, time), element(element), shading(element) {}

        public:

            Point2 uv;
            SurfaceElement element;
            SurfaceElement shading;
    };
}