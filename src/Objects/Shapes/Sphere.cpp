
#include "Sphere.hpp"

namespace Ilya
{

    Sphere::Sphere(float radius, float zmin, float zmax, float phimax,
                   const Ref<Transform>& objtoworld, bool reverse_orientation):
            Shape(objtoworld, reverse_orientation), radius(radius)
    {
        this->zmin = glm::clamp(glm::min(zmin, zmax), -radius, radius);
        this->zmax = glm::clamp(glm::max(zmin, zmax), -radius, radius);

        thetaMin = glm::acos(zmin);
        thetaMax = glm::acos(zmax);

        phiMax = glm::radians(glm::clamp(phimax, 0.f, 360.f));
    }

    Bounds Sphere::objspace_bounds() const
    {
        return {{-radius, -radius, zmin},
                {radius, radius, zmax}};
    }
}