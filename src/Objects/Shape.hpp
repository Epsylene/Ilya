
#pragma once

#include "Core.hpp"
#include "Utils/Transform.hpp"

namespace Ilya
{
    class Shape
    {
        public:

            Shape(const Ref<Transform>& objtoworld,
                const Ref<Transform>& worldtoobj, bool reverse_orientation):
                objtoworld(objtoworld), worldtoobj(worldtoobj),
                reverse_orientation(reverse_orientation),
                swapped_handedness(objtoworld->swaps_handedness()) {}

            virtual Bounds objspace_bounds() const = 0;
            virtual Bounds worldspace_bounds() const
            {
                return (*objtoworld)(objspace_bounds());
            }

        public:

            const Ref<Transform> objtoworld, worldtoobj;
            const bool reverse_orientation;
            const bool swapped_handedness;
    };
}