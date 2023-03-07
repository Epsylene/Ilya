
#include "Transform.hpp"
#include "Core.hpp"

namespace Ilya
{
    Transform operator*(const Transform& t1, const Transform& t2)
    {
        return {t1.transform * t2.transform,
                t2.inv * t1.inv};
    }

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

    Ref<Transform> inverse(const Ref<Transform>& T)
    {
        return std::make_shared<Transform>(T->inv, T->transform);
    }

    bool Transform::swaps_handedness() const
    {
        // A transformation swaps the handedness of
        // the coordinate system if its determinant
        // is negative; we need only to check this on
        // the actual transformation matrix, the 3x3
        // upper-left sub-matrix.
        auto sub = glm::mat3{transform[0], transform[1], transform[2]};
        auto det = determinant(sub);

        return det < 0.f;
    }

    Point3 Transform::operator()(const Point3& p) const
    {
        // Homogeneous coordinates (also called
        // projective coordinates) are coordinates
        // used in projective geometry, like
        // Cartesian coordinates are in Euclidean
        // geometry. The projective plane (where
        // points of the projective geometry live)
        // can be thought of as the Euclidean plane
        // with additional points added, the "points
        // at infinity", which can be seen as the
        // limit point for each given direction in
        // space; parallel lines in Euclidean space
        // are said to intersect at the point at
        // infinity corresponding to their direction.
        //
        // Points on the projective plane represent
        // coordinates on the Euclidean plane. These
        // coordinates do not change upon scaling of
        // the projective space points, because doing
        // so is equivalent to moving them along the
        // lines that connect them to the origin (the
        // projective line), which doesn't change the
        // projection on the Euclidean plane. Thus,
        // points (x, y, z) in Euclidean space become
        // (x*w, y*w, z*w, w) in projective space;
        // the factor w is commonly set to 1, so to
        // have the homogeneous system of coordinates
        // (x, y, z, 1). This is what we do here: the
        // Euclidean point p is put in projective
        // space and transformed inside that space.
        auto point = Vec4{p.x, p.y, p.z, 1.f};
        auto newp = transform * point;

        // To get the Euclidean coordinates of the
        // new point, we have to get rid of the w
        // factor in the expression given above, so
        // we divide by the fourth coordinate:
        if(newp.w == 1)
            return Point3{newp};
        else
            return Point3{newp}/newp.w;
    }

    Vec3 Transform::operator()(const Vec3& v) const
    {
        // Vectors, when seen as directions,
        // correspond to lines in projective space.
        // All parallel lines (which are headed
        // towards the same direction) share the same
        // point at infinity; thus, the coordinates
        // of this point are enough to represent a
        // direction in projective space. Points
        // at infinity can be seen as normal points
        // where the scaling factor w has been set
        // to zero (in other words, points that have
        // been scaled along the line all the way
        // down to the origin). Then a vector v in
        // Euclidean coordinates becomes (v, 0) in
        // projective space:
        auto vector = Vec4{v.x, v.y, v.z, 0.f};

        return Vec3{transform * vector};
    }

    Normal Transform::operator()(const Normal& n) const
    {
        // Normals do not transform in the same way
        // as vectors do (picture a normal on a
        // circle that is being scaled along some
        // axis: the normal changes in a different
        // way than the rest of the shape). We know
        // by construction that for any normal vector
        // n and tangent vector t at the same point
        // dot(n, t) = 0, which we can write as n^T.t
        // = 0 (where n^T is the transpose of n).
        // When we transform the point by some matrix
        // M, the new tangent vector at this point is
        // M.t ; then if the new normal is S.n (for
        // some matrix S to be determined), we have
        // that (Sn)^T.(Mt) = 0 <=> n^T.S^T.Mt = 0.
        // Since n^T.t = 0, S^T.M = Id, therefore S^T
        // = M^-1 and so S = (M^-1)^T: normals must be
        // transformed by the inverse transpose of the
        // transformation matrix.
        auto normal = Vec4{n.x, n.y, n.z, 0.f};
        auto newn = transpose(inverse(transform)) * normal;

        return Normal{newn};
    }

    Bounds Transform::operator()(const Bounds& b) const
    {
        // The transformation of an object bounding box
        // is calculated by transforming each of the
        // former box corners and calculating the new
        // box extension at each step.

        const auto& T = *this;
        const auto& min = b.min;
        const auto& max = b.max;

        Point3 corner0 = {b.min.x, b.min.y, b.min.z};
        Point3 corner1 = {b.max.x, b.min.y, b.min.z};
        Point3 corner2 = {b.min.x, b.max.y, b.min.z};
        Point3 corner3 = {b.min.x, b.min.y, b.max.z};
        Point3 corner4 = {b.min.x, b.max.y, b.max.z};
        Point3 corner5 = {b.max.x, b.max.y, b.min.z};
        Point3 corner6 = {b.max.x, b.min.y, b.max.z};
        Point3 corner7 = {b.max.x, b.max.y, b.max.z};

        Bounds ret {T(corner0)};
        ret = surrounding_box(ret, {T(corner1)});
        ret = surrounding_box(ret, {T(corner2)});
        ret = surrounding_box(ret, {T(corner3)});
        ret = surrounding_box(ret, {T(corner4)});
        ret = surrounding_box(ret, {T(corner5)});
        ret = surrounding_box(ret, {T(corner6)});
        ret = surrounding_box(ret, {T(corner7)});

        return ret;
    }
}