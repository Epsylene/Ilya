
#include "Shape.hpp"

namespace Ilya
{
    Bounds Shape::worldspace_bounds() const
    {
        // By default, to get the object bounds in world
        // space, just apply the "object space to world
        // space" transform on the object space bounds.
        const auto& T = *objtoworld;
        return T(objspace_bounds());
    }

    Shape::Shape(const Ref<Transform>& objtoworld, bool reverse_orientation):
        objtoworld(objtoworld), reverse_orientation(reverse_orientation),
        worldtoobj(inverse(objtoworld)),
        T_swaps_handedness(objtoworld->swaps_handedness())
    {
    }
}