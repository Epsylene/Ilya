
#include "functions.hpp"

namespace Ilya
{
    float dot(const Vec3& v, const Point3& p)
    {
        return dot(v, Vec3{p.x, p.y, p.z});
    }

    float square(const Vec3& v)
    {
        return glm::dot(v, v);
    }

    Vec3 sqrt(const Vec3& v)
    {
        return glm::sqrt(v);
    }
}