
#include "Math.hpp"

namespace Ilya
{
    float square(const Vec3& p)
    {
        return glm::dot(p, p);
    }

    Vec3 sqrt(const Vec3& p)
    {
        return glm::sqrt(p);
    }

    ONB::ONB(const Vec3& w)
    {
        // To build an orthogonal basis (u, v, w) from a given vector,
        // first normalize it:
        this->w = normalize(w);
        // Then check if it is not equal to some axis, which we take here
        // to be X. We don't check exactly for the equality, to avoid
        // floating point rounding errors.
        Vec3 a = (std::abs(w.x) > 0.9f) ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        // From those two vectors, build a third, perpendicular to both,
        // using the cross product:
        v = normalize(cross(w, a));
        // Finally, with two unitary and orthogonal vectors in our basis,
        // we can easily find the third and last using the cross product
        // again:
        u = cross(w, v);
    }
}