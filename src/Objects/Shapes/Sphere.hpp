
#pragma once

#include "Shape.hpp"
#include "Objects/Bounds.hpp"

namespace Ilya
{
    class Sphere: public Shape
    {
        public:

            Sphere(float radius, float zmin, float zmax, float phimax,
                   const Ref<Transform>& objtoworld, bool reverse_orientation);

            Bounds objspace_bounds() const override;

        public:

            const float radius;
            float zmin, zmax;
            float thetaMin, thetaMax, phiMax;
    };
}