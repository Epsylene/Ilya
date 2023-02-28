
#pragma once

#include "ilpch.hpp"

namespace Ilya
{
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    float square(const Vec3& p);
    Vec3 sqrt(const Vec3& p);

    /// @brief 3D point class
    ///
    /// Points are zero-dimensional locations in
    /// space. They can be translated, but not
    /// rotated or scaled, contrary to vectors, which
    /// represent arrows (directions with a length).
    class Point3
    {
        public:

            float x, y, z;

            Point3() = default;
            Point3(float x, float y, float z):
                    x(x), y(y), z(z) {}

            explicit Point3(const Vec3& v):
                x(v.x), y(v.y), z(v.z) {}
    };

    Point3 operator*(const Point3& p, float scalar);
    Point3 operator/(const Point3& p, float scalar);

    /// @brief 3D normal class
    ///
    /// A normal is a vector that is perpendicular to
    /// the surface at a specific position. As such,
    /// they behave differently than vectors upon
    /// certain transformations, like scaling.
    class Normal
    {
        public:

            float x, y, z;

            Normal() = default;
            Normal(float x, float y, float z):
                    x(x), y(y), z(z) {}
    };

    /// Orthonormal base of 3 vectors.
    class ONB
    {
        public:

            /// Constructs an orthonormal base from the vector w.
            explicit ONB(const Vec3& w);

            Vec3 local(float a, float b, float c) const
            {
                return a*u + b*v + c*w;
            }

            Vec3 local(const Vec3& v) const
            {
                return local(v.x, v.y, v.z);
            }

        public:

            Vec3 u, v, w;
    };
}

////////////////////////////////////////////
// To get glm::vec3 and glm::vec4 to support
// structured bindings.
////////////////////////////////////////////
namespace std
{
    template<auto L, typename T, auto Q>
    struct tuple_size<glm::vec<L, T, Q>>
            : std::integral_constant<std::size_t, 3> {};

    template<std::size_t N, auto L, typename T, auto Q>
    struct tuple_element<N, glm::vec<L, T, Q>> { using type = float; };
}
////////////////////////////////////////////
namespace glm
{
    template<std::size_t idx, auto L, typename T, auto Q>
    auto get(const glm::vec<L, T, Q>& person)
    {
        if constexpr (idx == 0) return person.x;
        if constexpr (idx == 1) return person.y;
        if constexpr (idx == 2) return person.z;
    }

    template<std::size_t idx, auto L, typename T, auto Q>
    auto& get(glm::vec<L, T, Q>& person)
    {
        if constexpr (idx == 0) return person.x;
        if constexpr (idx == 1) return person.y;
        if constexpr (idx == 2) return person.z;
    }
}
///////////////////////////////////////////////