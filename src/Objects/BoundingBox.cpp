
#include "BoundingBox.hpp"

namespace Ilya
{
    BoundingBox surrounding_box(const BoundingBox& b1, const BoundingBox& b2)
    {
        Vec3 min { std::min(b1.min.x, b2.min.x),
                   std::min(b1.min.y, b2.min.y),
                   std::min(b1.min.z, b2.min.z) };

        Vec3 max { std::max(b1.max.x, b2.max.x),
                   std::max(b1.max.y, b2.max.y),
                   std::max(b1.max.z, b2.max.z) };

        return { min, max };
    }
}