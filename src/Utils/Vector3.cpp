
#include "Vector3.hpp"

namespace Ilya
{
    Vector3 operator+(Vector3 vec1, const Vector3& vec2)
    {
        vec1 += vec2;
        return vec1;
    }

    Vector3 operator-(Vector3 vec1, const Vector3& vec2)
    {
        vec1 -= vec2;
        return vec1;
    }

    Vector3 operator*(Vector3 vec, float scalar)
    {
        vec *= scalar;
        return vec;
    }

    Vector3 operator*(float scalar, Vector3 vec)
    {
        vec *= scalar;
        return vec;
    }

    Vector3 operator/(Vector3 vec1, float scalar)
    {
        vec1 /= scalar;
        return vec1;
    }

    float length(const Vector3& vec)
    {
        return std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
    }

    Vector3 unit(const Vector3& vec)
    {
        auto invnorm = 1.f / std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
        return vec*invnorm;
    }

    float dot(const Vector3& vec1, const Vector3& vec2)
    {
        return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
    }

    Vector3 cross(const Vector3& vec1, const Vector3& vec2)
    {
        return { vec1.y * vec2.z - vec2.y * vec1.z,
                 vec1.z * vec2.x - vec2.z * vec1.x,
                 vec1.x * vec2.y - vec2.x * vec1.y };
    }

    float square(const Vector3& vec)
    {
        return dot(vec, vec);
    }

    Vector3 sqrt(const Vector3& vec)
    {
        return {std::sqrt(vec.x), std::sqrt(vec.y), std::sqrt(vec.z)};
    }

    bool near_zero(const Vector3& vec)
    {
        return (std::abs(vec.x) < epsilon
            && std::abs(vec.y) < epsilon
            && std::abs(vec.z) < epsilon);
    }

    ONB::ONB(const Vec3& w)
    {
        // To build an orthogonal basis (u, v, w) from a given vector,
        // first normalize it:
        this->w = unit(w);
        // Then check if it is not equal to some axis, which we take here
        // to be X. We don't check exactly for the equality, to avoid
        // floating point rounding errors.
        Vec3 a = (std::abs(w.x) > 0.9f) ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        // From those two vectors, build a third, perpendicular to both,
        // using the cross product:
        v = unit(cross(w, a));
        // Finally, with two unitary and orthogonal vectors in our basis,
        // we can easily find the third and last using the cross product
        // again:
        u = cross(w, v);
    }
}