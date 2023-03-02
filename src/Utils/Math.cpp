
#include "Math.hpp"

namespace Ilya
{
    Point3 operator+(const Point3& p, const Vec3& v)
    {
        return {p.x + v.x, p.y + v.y, p.z + v.z};
    }

    Point3 operator-(const Point3& p, const Vec3& v)
    {
        return {p.x - v.x, p.y - v.y, p.z - v.z};
    }

    Vec3 operator-(const Point3& p1, const Point3& p2)
    {
        return {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
    }

    Point3 operator*(const Point3& p, float scalar)
    {
        return {p.x*scalar, p.y*scalar, p.z*scalar};
    }

    Point3 operator/(const Point3& p, float scalar)
    {
        auto inv = 1 / scalar;
        return {p.x*inv, p.y*inv, p.z*inv};
    }

    float& Point3::operator[](std::size_t idx)
    {
        return coords[idx];
    }

    float Point3::operator[](std::size_t idx) const
    {
        return coords[idx];
    }

    Point3& Point3::operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
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