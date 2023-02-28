
#include "Transform.hpp"

namespace Ilya
{
    Transform translate(const Vec3& delta)
    {
        auto m = glm::translate(delta);
        auto inv = glm::translate(-delta);

        return {m, inv};
    }

    Transform scale(const Vec3& factor)
    {
        auto m = glm::scale(factor);
        auto inv = glm::scale(1.f / factor);

        return {m, inv};
    }

    template<>
    Transform rotate<Axis::X>(float angle)
    {
        auto m = glm::rotate(angle, Vec3{1.f, 0.f, 0.f});
        auto inv = transpose(m);

        return {m, inv};
    }

    template<>
    Transform rotate<Axis::Y>(float angle)
    {
        auto m = glm::rotate(angle, Vec3{0.f, 1.f, 0.f});
        auto inv = transpose(m);

        return {m, inv};
    }

    template<>
    Transform rotate<Axis::Z>(float angle)
    {
        auto m = glm::rotate(angle, Vec3{0.f, 0.f, 1.f});
        auto inv = transpose(m);

        return {m, inv};
    }

    Transform rotate(float angle, const Vec3& axis)
    {
        auto m = glm::rotate(angle, axis);
        auto inv = transpose(m);

        return {m, inv};
    }

    Transform look_at(const Vec3& pos, const Vec3& at, const Vec3& up)
    {
        auto inv = glm::lookAt(pos, at, up);
        auto m = inverse(inv);

        return {m, inv};
    }

    Vec3 Transform::operator()(const Vec3& v) const
    {
        auto vector = Vec4{v.x, v.y, v.z, 0.f};

        return transform*vector;
    }

    Point3 Transform::operator()(const Point3& p) const
    {
        auto point = Vec4{p.x, p.y, p.z, 1.f};
        auto newp = transform * point;

        if(newp.w == 1)
            return Point3{newp};
        else
            return Point3{newp}/newp.w;
    }
}