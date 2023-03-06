
#pragma once

#include "Core.hpp"
#include "Utils/Transform.hpp"

namespace Ilya
{
    class Shape
    {
        public:

            //
            Shape(const Ref<Transform>& objtoworld,
                const Ref<Transform>& worldtoobj, bool reverse_orientation):
                objtoworld(objtoworld), worldtoobj(worldtoobj),
                reverse_orientation(reverse_orientation),
                swapped_handedness(objtoworld->swaps_handedness()) {}

            virtual Bounds objspace_bounds() const = 0;
            virtual Bounds worldspace_bounds() const
            {
                // By default, to get the object
                // bounds in world space, just apply
                // the "object space to world space"
                // transform on the object space
                // bounds.
                const auto& T = *objtoworld;
                return T(objspace_bounds());
            }

        public:

            const Ref<Transform> objtoworld, worldtoobj;
            const bool reverse_orientation;
            const bool swapped_handedness;
    };
}