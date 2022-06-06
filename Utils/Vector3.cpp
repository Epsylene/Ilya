
#include "Vector3.hpp"

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

Vector3 unit(const Vector3& vec)
{
    auto invnorm = 1.f / std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
    return vec*invnorm;
}

float dot(const Vector3& vec1, const Vector3& vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}

float square(const Vector3& vec)
{
    return dot(vec, vec);
}

Vector3 sqrt(const Vector3& vec)
{
    return {std::sqrt(vec.x), std::sqrt(vec.y), std::sqrt(vec.z)};
}

Vec3 rand_in_unit_sphere()
{
    while (true)
    {
        // A random point inside of a unit sphere is a random vector in
        // a unit cube which has norm less than 1.
        auto p = Vec3::random(-1,1);
        if (square(p) >= 1)
            continue;

        return p;
    }
}