
#pragma once

#include "ilpch.hpp"

namespace Ilya
{
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    float square(const Vec3& p);
    Vec3 sqrt(const Vec3& p);

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

namespace std
{
    template<>
    struct tuple_size<glm::vec3>
            : std::integral_constant<std::size_t, 3> {};

    template<std::size_t N> struct tuple_element<N, glm::vec3> { using type = float; };
}

namespace glm
{
    template<std::size_t idx>
    auto get(const glm::vec3& person)
    {
        if constexpr (idx == 0) return person.x;
        if constexpr (idx == 1) return person.y;
        if constexpr (idx == 2) return person.z;
    }

    template<std::size_t idx>
    auto& get(glm::vec3& person)
    {
        if constexpr (idx == 0) return person.x;
        if constexpr (idx == 1) return person.y;
        if constexpr (idx == 2) return person.z;
    }
}