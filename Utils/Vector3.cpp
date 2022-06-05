
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