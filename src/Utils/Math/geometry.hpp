
#pragma once

#include "ilpch.hpp"

namespace Ilya
{
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    /// @brief 3D point class
    ///
    /// Points are zero-dimensional locations in
    /// space. They can be translated, but not
    /// rotated or scaled, contrary to vectors, which
    /// represent arrows (directions with a length).
    class Point3
    {
        public:

            union
            {
                struct { float x, y, z; };
                struct { std::array<float, 3> coords {}; };
            };

            Point3(): x(0), y(0), z(0) {};
            Point3(float x, float y, float z):
                x(x), y(y), z(z) {}

            explicit Point3(float val):
                x(val), y(val), z(val) {}

            explicit Point3(const Vec3& v):
                x(v.x), y(v.y), z(v.z) {}

            float& operator[](std::size_t idx);
            float operator[](std::size_t idx) const;

            Point3& operator+=(const Vec3& v);
    };

    Point3 operator+(const Point3& p, const Vec3& v);
    Point3 operator-(const Point3& p, const Vec3& v);
    Vec3 operator-(const Point3& p1, const Point3& p2);
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

            explicit Normal(const Vec3& v):
                x(v.x), y(v.y), z(v.z) {}
    };

    /// Orthonormal base of 3 vectors.
    class ONB
    {
        public:

            /// Constructs an orthonormal base from
            /// the vector w.
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

////////////////////////////////////////////////////
// To get the glm::vec3, glm::vec4 and Point classes
// to support structured bindings.
////////////////////////////////////////////////////
namespace std
{
    template<auto L, typename T, auto Q>
    struct tuple_size<glm::vec<L, T, Q>>
            : std::integral_constant<std::size_t, 3> {};

    template<std::size_t N, auto L, typename T, auto Q>
    struct tuple_element<N, glm::vec<L, T, Q>> { using type = float; };

    template<>
    struct tuple_size<Ilya::Point3>
            : std::integral_constant<std::size_t, 3> {};

    template<std::size_t N>
    struct tuple_element<N, Ilya::Point3> { using type = float; };

}
///////////////////////////////////////////////////////
namespace glm
{
    template<std::size_t idx, auto L, typename T, auto Q>
    auto get(const glm::vec<L, T, Q>& v)
    {
        if constexpr (idx == 0) return v.x;
        if constexpr (idx == 1) return v.y;
        if constexpr (idx == 2) return v.z;
    }

    template<std::size_t idx, auto L, typename T, auto Q>
    auto& get(glm::vec<L, T, Q>& v)
    {
        if constexpr (idx == 0) return v.x;
        if constexpr (idx == 1) return v.y;
        if constexpr (idx == 2) return v.z;
    }
}

namespace Ilya
{
    template<std::size_t idx>
    auto get(const Ilya::Point3& p)
    {
        if constexpr (idx == 0) return p.x;
        if constexpr (idx == 1) return p.y;
        if constexpr (idx == 2) return p.z;
    }

    template<std::size_t idx>
    auto& get(Ilya::Point3& p)
    {
        if constexpr (idx == 0) return p.x;
        if constexpr (idx == 1) return p.y;
        if constexpr (idx == 2) return p.z;
    }
}
///////////////////////////////////////////////