
#include "Hittable.hpp"

namespace Ilya
{

    bool HittableList::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        HitRecord temp_rec {};
        bool hit = false;
        auto closest_so_far = tmax;

        for (const auto& obj: objects)
        {
            // For each object, check if the ray hits it in the interval
            // [tmin,closest_so_far], such that the range is reduced each
            // cast_time the ray hits an object. In other words, if there are
            // three spheres at t2, t1 and t3 (such that t1 < t2 < t3),
            // then :
            //  1) Sphere 1 : hit() true at t2, closest_so_far = t2
            //  2) Sphere 2 : t1 < t2, so hit() true at t1,
            //      closest_so_far = t1
            //  3) Sphere 3 : t3 > t1, so hit() returns false
            // The result is that the recorded position, normals, etc
            // are the ones for the closest sphere, which is what is
            // expected physically.
            if(obj->hit(r, tmin, closest_so_far, temp_rec))
            {
                hit = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit;
    }

    bool HittableList::bounding_box(float t0, float t1, BoundingBox& box) const
    {
        if(objects.empty())
            return false;

        BoundingBox objbox {};
        bool first_box = true;

        for (auto& object: objects)
        {
            if(!object->bounding_box(t0, t1, objbox))
                return false;

            box = first_box ? objbox : surrounding_box(box, objbox);
            first_box = false;
        }

        return true;
    }

    bool Sphere::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        // How do we detect if a ray hits a sphere ? Let's say the ray
        // is described with a point P and a sphere of radius R is placed
        // at a point C. Then saying that the ray hits the sphere is the
        // same as saying that P is anywhere in a radius R of the center
        // (because that is how the sphere boundary is defined), or in
        // equation that (P - C)^2 = R^2. But we know P = A + tB, where
        // A is the ray origin, B its direction and t the cast_time; then
        // (A + tB - C)^2 = R^2 <=> (tB)^2 + 2tB(A-C) + (A-C)^2 - R^2 = 0,
        // which is simply a quadratic equation in t. The solutions of this
        // equation are the times at which the ray hits the sphere.
        auto oc = r.orig - center(r.cast_time);
        auto a = dot(r.dir, r.dir);
        auto b = 2 * dot(r.dir, oc);
        auto c = dot(oc, oc) - radius*radius;

        auto discriminant = b*b - 4*a*c;
        if(discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);
        auto root = (-b - sqrtd)/(2*a);

        // We want the solution to be within the cast_time
        // range [tmin,tmax].
        if(root < tmin || root > tmax)
        {
            root = (-b + sqrtd)/(2*a);

            if(root < tmin || root > tmax)
                return false;
        }

        // Once we got the solution, we save it in the hit record,
        // which will allow us to use that data later
        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = (rec.p - center(r.cast_time))/radius;
        rec.material = material;
        auto out_normal = (rec.p - center(r.cast_time))/radius;
        rec.face_normal(r, out_normal);
        std::tie(rec.u, rec.v) = sphere_uv(out_normal);

        return true;
    }

    Vec3 Sphere::center(float t) const
    {
        // Center of a sphere that moves at constant speed between points
        // C0 and C1 in the time t1 - t0, at the time t
        return c0 + (t - t0)/(t1 - t0) * (c1 - c0);
    }

    bool Sphere::bounding_box(float t0, float t1, BoundingBox& box) const
    {
        BoundingBox box1 {center(t0) - Vec3{radius}, center(t0) + Vec3{radius}};
        BoundingBox box2 {center(t1) - Vec3{radius}, center(t1) + Vec3{radius}};
        box = surrounding_box(box1, box2);

        return true;
    }

    template<int N> requires (N < 3)
    inline bool box_compare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable> b)
    {
        BoundingBox box_a {}, box_b {};

        if(!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
            std::cerr << "No bounding box in BVHnode constructor.";

        return box_a.min[N] < box_b.min[N];
    }

    BVHnode::BVHnode(const std::vector<std::shared_ptr<Hittable>>& objs, size_t start, size_t end, float t0,
                     float t1)
    {
        // The bounding volume hierarchy (BVH) is a structure that constructs
        // a tree from a set of objects. To do so, we need a comparator
        // function, which works on a per-axis basis; the axis is chosen
        // randomly as the constructor keeps being called when constructing
        // the tree (in other words, node splitting is done each time along
        // one random axis).
        auto objects = objs;
        int axis = random_int(0, 2);
        auto comparator = (axis == 0) ? box_compare<0>
                                      : (axis == 1) ? box_compare<1>
                                                    : box_compare<2>;

        size_t span = end - start;
        if(span == 1)
        {
            left = right = objects[start];
        }
        else if(span == 2)
        {
            if(comparator(objects[start], objects[start + 1]))
            {
                left = objects[start];
                right = objects[start + 1];
            }
            else
            {
                left = objects[start + 1];
                right = objects[start];
            }
        }
        else
        {
            // We work with a top-down method to construct the BVH tree:
            // first sort the objects using the comparator (so that the
            // array ordering matches the spatial ordering), split the tree
            // in two nodes, and repeat until all cases have been covered.
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + span/2;
            left = std::make_shared<BVHnode>(objects, start, mid, t0, t1);
            right = std::make_shared<BVHnode>(objects, mid, end, t0, t1);
        }

        BoundingBox box_left {}, box_right {};
        if(!left->bounding_box(t0, t1, box_left)
           || !right->bounding_box(t0, t1, box_right))
        {
            std::cerr << "No bounding box in BVHnode constructor.";
        }

        box = surrounding_box(box_left, box_right);
    }

    bool BVHnode::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        if(!box.hit(r, tmin, tmax))
            return false;

        // Check if the ray has hit the left node (or, more accurately,
        // "could hit the left node in the [tmin, tmax] interval). If it has,
        // the 'tmax' for the right node is the time stored in the hit
        // record (if the first node has been calculated to be hit at a
        // certain time, the second node can't be hit after that point
        // in time).
        bool hit_left = left->hit(r, tmin, tmax, rec);
        bool hit_right = right->hit(r, tmin, hit_left ? rec.t : tmax, rec);

        return hit_left || hit_right;
    }

    bool BVHnode::bounding_box(float t0, float t1, BoundingBox& box) const
    {
        box = this->box;
        return true;
    }

    template<Axis ax0, Axis ax1> requires (ax0 != ax1)
    bool Rectangle<ax0, ax1>::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        float t = -1.f;
        if constexpr(XY<ax0, ax1>)
            t = (k - r.orig.z)/r.dir.z;
        else if constexpr(XZ<ax0, ax1>)
            t = (k - r.orig.y)/r.dir.y;
        else if constexpr(YZ<ax0, ax1>)
            t = (k - r.orig.x)/r.dir.x;

        if(t < tmin || t > tmax)
            return false;

        auto [x, y, z] = r.at(t);
        if constexpr(XY<ax0, ax1>)
        {
            if(x < r0 || x > r1 || y < s0 || y > s1)
                return false;

            rec.u = (x - r0)/(r1 - r0);
            rec.v = (y - s0)/(s1 - s0);
            rec.face_normal(r, {0.f, 0.f, 1.f});
        }
        else if constexpr(XZ<ax0, ax1>)
        {
            if(x < r0 || x > r1 || z < s0 || z > s1)
                return false;

            rec.u = (x - r0)/(r1 - r0);
            rec.v = (z - s0)/(s1 - s0);
            rec.face_normal(r, {0.f, 1.f, 0.f});
        }
        else if constexpr(YZ<ax0, ax1>)
        {
            if(y < r0 || y > r1 || z < s0 || z > s1)
                return false;

            rec.u = (y - r0)/(r1 - r0);
            rec.v = (z - s0)/(s1 - s0);
            rec.face_normal(r, {1.f, 0.f, 0.f});
        }

        rec.t = t;
        rec.material = material;
        rec.p = {x, y, z};

        return true;
    }

    using Axis::X, Axis::Y, Axis::Z;
    template class Rectangle<X, Y>;
    template class Rectangle<X, Z>;
    template class Rectangle<Y, Z>;
}