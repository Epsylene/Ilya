
#include "Hittable.hpp"

#include "Utils/Math.hpp"
#include "Utils/functions.hpp"

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
            // The result is that the recorded position, normals, etc.
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

    bool HittableList::bounding_box(Bounds& box, float t0, float t1) const
    {
        if(objects.empty())
            return false;

        Bounds objbox {};
        bool first_box = true;

        for (auto& object: objects)
        {
            if(!object->bounding_box(objbox, t0, t1))
                return false;

            box = first_box ? objbox : surrounding_box(box, objbox);
            first_box = false;
        }

        return true;
    }

    Point3 HittableList::random_point(const Point3& origin) const
    {
        // Return a random point of one of the objects in the list,
        // chosen at random.
        auto index = Random::uint(0, objects.size() - 1);
        return objects[index]->random_point(origin);
    }

    float HittableList::pdf_value(const Ray& r)
    {
        // Sum the probability values for every object in the list and
        // return it normalized.

        auto sum = 0.f;
        for (auto& obj: objects)
        {
            sum += obj->pdf_value(r);
        }

        return sum/objects.size();
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
        rec.p = r(rec.t);
        rec.normal = (rec.p - center(r.cast_time))/radius;
        rec.material = material;
        auto out_normal = (rec.p - center(r.cast_time))/radius;
        rec.face_normal(r, out_normal);
        std::tie(rec.u, rec.v) = sphere_uv(out_normal);

        return true;
    }

    Point3 Sphere::center(float t) const
    {
        // Center of a sphere that moves at constant speed between points
        // C0 and C1 in the time t1 - t0, at the time t
        return c0 + (t - t0)/(t1 - t0) * (c1 - c0);
    }

    bool Sphere::bounding_box(Bounds& box, float t0, float t1) const
    {
        Bounds box1 {center(t0) - Vec3{radius}, center(t0) + Vec3{radius}};
        Bounds box2 {center(t1) - Vec3{radius}, center(t1) + Vec3{radius}};
        box = surrounding_box(box1, box2);

        return true;
    }

    Point3 Sphere::random_point(const Point3& origin) const
    {
        auto dir = c0 - origin;
        auto d = length(dir);

        // To get a random point on the surface of the sphere, we get
        // two random numbers, which we use for the angles theta and phi...
        auto r1 = Random::rfloat();
        auto r2 = Random::rfloat();
        auto phi = 2*pi*r1;

        // ...and then transform to cartesian coordinates. We have that
        // r2 = Integral{2pi*f(u)*sin(t)} (see `Random::cosine_dir()`),
        // with f(u) = C a constant because we are sampling uniformly
        // over the sphere, and with r2 = 1 at theta = theta_max, so in
        // the end cos(t) = 1 + r2*(cos(t_max) - 1). But sin(t_max),
        // geometrically, is the ratio between the radius of the sphere
        // and the distance from the viewer to the center of the sphere,
        // that is, sin(t_max) = r/d. Then cos(t_max) = sqrt(1 - sin^2),
        // and transforming from spherical coordinates give us x, y and
        // z.
        auto cos_t_max = std::sqrt(1 - radius*radius/(d*d));
        auto z = 1 + r2*(cos_t_max - 1);
        auto x = std::cos(phi)*std::sqrt(1 - z*z);
        auto y = std::sin(phi)*std::sqrt(1 - z*z);

        ONB uvw {dir};
        return Point3{uvw.local(x, y, z)};
    }

    float Sphere::pdf_value(const Ray& r)
    {
        // Check that the ray hits the sphere; if not, the PDF value is 0.
        HitRecord rec {};
        if(!hit(r, 0.001, infinity, rec))
            return 0.f;

        // The probability of hitting the sphere of a certain radius at
        // a certain distance is the inverse of the solid angle through
        // which this sphere is seen (think "the probability of hitting
        // one unit of the surface of the sphere seen by the viewer",
        // which is precisely the solid angle). Then, knowing that the
        // solid angle is given by the integral over theta and phi at a
        // constant radius, W = Integral{sin(theta)} = 2*pi*(1 - cos(t_max))
        // and calculating cos(t_max) as described in `random_point()`,
        // we get the result.
        auto d = length(center(rec.t) - r.orig);
        auto cos_t_max = std::sqrt(1 - radius*radius/(d*d));
        auto solid_angle = 2*pi*(1 - cos_t_max);

        return 1/solid_angle;
    }

    /// Return whether the hittable `a` is to the left (true) or the
    /// right (false) of `b` on the axis N.
    template<unsigned N> requires (N < 3)
    inline bool box_compare(const Ref<Hittable>& a, const Ref<Hittable>& b)
    {
        // Compare two boxes.
        Bounds box_a {}, box_b {};

        // First check if the hittables `a` and `b` actually have bounding
        // boxes (that is, if one or both of them are not empty).
        if(!a->bounding_box(box_a, 0, 0) || !b->bounding_box(box_b, 0, 0))
            error("No bounding box in BVHnode constructor.\n");

        // Then, compare: on the axis N, is the surrounding box of `a`
        // more to the left than that of `b` ?
        return box_a.min[N] < box_b.min[N];
    }

    BVHnode::BVHnode(const std::vector<Ref<Hittable>>& objs, size_t start,
                     size_t end, float t0, float t1)
    {
        // The bounding volume hierarchy (BVH) is a structure that
        // constructs a tree from a set of objects, by dividing space
        // recursively in "left" and "right" boxes, which contain
        // objects of the scene. To do so, we need a comparator
        // function, which works on a per-axis basis: the axis is chosen
        // randomly as the constructor keeps being called when
        // constructing the tree (in other words, node splitting is done
        // each time along one random axis).
        auto objects = objs;
        auto axis = Random::uint(0, 2);
        auto comparator = (axis == 0) ? box_compare<0>
                                      : (axis == 1) ? box_compare<1>
                                                    : box_compare<2>;

        size_t span = end - start;
        if(span == 1)
        {
            // If there is only one object left, there is only one leaf
            // in this branch of the tree.
            left = right = objects[start];
        }
        else if(span == 2)
        {
            // If there are 2 objects, we can compare them directly:
            if(comparator(objects[start], objects[start + 1]))
            {
                // If the comparator returns true, the first parameter is
                // the left leaf and the second parameter the right leaf.
                left = objects[start];
                right = objects[start + 1];
            }
            else
            {
                // If it returns false, it is the other way around.
                left = objects[start + 1];
                right = objects[start];
            }
        }
        else
        {
            // For lists of 3 or more objects, we work with a top-down
            // method to construct the BVH tree: first sort the objects
            // using the comparator (so that the array ordering matches
            // the spatial ordering), split the tree in two nodes, and
            // repeat until all cases have been covered (that is, until
            // the if clauses are reduced to the 1- or 2-object cases).
            // Eventually, we are left with the leftmost and rightmost
            // objects of the scene as the 'left' and 'right' leaves.
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto half = start + span/2;
            left = std::make_shared<BVHnode>(objects, start, half, t0, t1);
            right = std::make_shared<BVHnode>(objects, half, end, t0, t1);
        }

        // Once the left and right nodes are found, check that they are
        // not empty...
        Bounds box_left {}, box_right {};
        if(!left->bounding_box(box_left, t0, t1)
           || !right->bounding_box(box_right, t0, t1))
        {
            error("No bounding box in BVHnode constructor.");
        }

        // ...and create the BVH tree surrounding box from the left and
        // right boxes.
        box = surrounding_box(box_left, box_right);
    }

    bool BVHnode::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        // If the ray doesn't hit the surrounding box, it won't hit
        // anything.
        if(!box.hit(r, tmin, tmax))
            return false;

        // If the ray hits the left node in the given time interval, the
        // maximum time at which the right node could be hit is the time
        // at which the left node has been (that is, `rec.t`): a ray
        // coming from the right could only have hit the left box if it
        // has hit the right one before.
        bool hit_left = left->hit(r, tmin, tmax, rec);
        bool hit_right = right->hit(r, tmin, hit_left ? rec.t : tmax, rec);

        return hit_left || hit_right;
    }

    bool BVHnode::bounding_box(Bounds& box, float t0, float t1) const
    {
        box = this->box;
        return true;
    }

    template<Axis ax0, Axis ax1>
    concept XY = (ax0 == Axis::X && ax1 == Axis::Y);

    template<Axis ax0, Axis ax1>
    concept XZ = (ax0 == Axis::X && ax1 == Axis::Z);

    template<Axis ax0, Axis ax1>
    concept YZ = (ax0 == Axis::Y && ax1 == Axis::Z);

    template<Axis ax0, Axis ax1> requires (ax0 < ax1)
    bool Rectangle<ax0, ax1>::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        // Calculate the time at which the ray hits the rectangle: it is
        // the distance between the rectangle and the ray origin (k -
        // r.orig.*), divided by the velocity of the ray (r.dir.*), a ray
        // being defined as r = a + b*t, where `a` is the origin and `b`
        // the direction, which has the units of a velocity.
        float t;
        if constexpr(XY<ax0, ax1>)
            t = (k - r.orig.z)/r.dir.z;
        else if constexpr(XZ<ax0, ax1>)
            t = (k - r.orig.y)/r.dir.y;
        else if constexpr(YZ<ax0, ax1>)
            t = (k - r.orig.x)/r.dir.x;

        // After checking that t is in the time interval...
        if(t < tmin || t > tmax)
            return false;

        //...we can get the point where the ray hit the rectangle, check
        // that it is not out of its physical bounds, and calculate its
        // UV coordinates as well as the surface normal.
        auto [x, y, z] = r(t);
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

    template<Axis ax0, Axis ax1>
    requires (ax0 < ax1)bool
    Rectangle<ax0, ax1>::bounding_box(Bounds& box, float t0,
                                      float t1) const
    {
        // The bounding box cannot have zero width, so we leave a little
        // room on the normal axis.
        if constexpr(XY<ax0, ax1>)
            box = {{r0, s0, k - 0.0001f}, {r1, s1, k + 0.0001f}};
        else if constexpr(XZ<ax0, ax1>)
            box = {{r0, k - 0.0001f, s0 }, {r1, k + 0.0001f, s1}};
        else if constexpr(YZ<ax0, ax1>)
            box = {{k - 0.0001f, r0, s0 }, {k + 0.0001f, r1, s1}};

        return true;
    }

    template<Axis ax0, Axis ax1>
    requires (ax0 < ax1)
    Point3 Rectangle<ax0, ax1>::random_point(const Point3& origin) const
    {
        Point3 random_point {};

        if constexpr(XY<ax0, ax1>)
            random_point = {Random::rfloat(r0, r1), Random::rfloat(s0, s1), k};
        else if constexpr(XZ<ax0, ax1>)
            random_point = {Random::rfloat(r0, r1), k, Random::rfloat(s0, s1) };
        else if constexpr(YZ<ax0, ax1>)
            random_point = {k, Random::rfloat(r0, r1), Random::rfloat(s0, s1) };

        return Point3{random_point - origin};
    }

    template<Axis ax0, Axis ax1>
    requires (ax0 < ax1)
    float Rectangle<ax0, ax1>::pdf_value(const Ray& r)
    {
        // Check that the ray hits the rectangle (in other words, that
        // it is directed towards it): if it doesn't, return 0, because
        // we want this PDF to be a random distribution directed at this
        // rectangle (a light, for example, which we will want to
        // "attract" rays, in order to avoid repetitive and noisy ray
        // bounces around the box).
        HitRecord rec;
        if(!hit(r, 0.001, infinity, rec))
            return 0.f;

        // If it does hit the rectangle, we need the PDF for the random
        // incoming vector. Two surfaces might be considered: the first is
        // the rectangle itself, where we want random vectors to be
        // distributed uniformly, so the probability is dA/A (in the same
        // fashion that the probability of obtaining a number between 1 and
        // 6 on a die is 1, the number element, divided by 6, the number of
        // elements). The second surface is the solid angle surface of the
        // unit sphere around the ray origin (that is, the intersection
        // between the sphere surface and the cone with its tip on the
        // origin and its base at the rectangle), which has element dw. We
        // can show geometrically that if the ray and the rectangle make an
        // angle alpha, then dw = cos(alpha)*dA/d^2, where d^2 is the
        // squared distance between the ray origin and the rectangle. But
        // both probabilities must be the same, so pdf_val*dw = dA/A and
        // finally pdf_val = d^2/(cos(alpha)*A).
        auto area = (r1-r0)*(s1-s0);
        auto d = rec.t * length(r.dir);
        auto cos = std::abs(dot(r.dir, rec.normal) / length(r.dir));

        return d*d/(cos*area);
    }

    using Axis::X, Axis::Y, Axis::Z;
    template class Rectangle<X, Y>;
    template class Rectangle<X, Z>;
    template class Rectangle<Y, Z>;

    Box::Box(const Vec3& p0, const Vec3& p1, Ref<Material> mat):
        p0(p0), p1(p1)
    {
        sides.add(make_shared<Rectangle<X, Y>>(p0.x, p0.y, p1.x, p1.y, p1.z, mat));
        sides.add(make_shared<Rectangle<X, Y>>(p0.x, p0.y, p1.x, p1.y, p0.z, mat));

        sides.add(make_shared<Rectangle<X, Z>>(p0.x, p0.z, p1.x, p1.z, p1.y, mat));
        sides.add(make_shared<Rectangle<X, Z>>(p0.x, p0.z, p1.x, p1.z, p0.y, mat));

        sides.add(make_shared<Rectangle<Y, Z>>(p0.y, p0.z, p1.y, p1.z, p1.x, mat));
        sides.add(make_shared<Rectangle<Y, Z>>(p0.y, p0.z, p1.y, p1.z, p0.x, mat));
    }

    bool Box::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        return sides.hit(r, tmin, tmax, rec);
    }

    bool ConstantMedium::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        // Raytracing what is known as volumes or participating media
        // (like fog or smoke) looks tricky, mainly because rays are
        // supposed to hit a volume, not a surface. One technique
        // consists in doing things probabilistically: once the ray
        // is inside the volume, give it a certain distance until it
        // scatters off it. We first need to check if the ray is indeed
        // inside the volume: this is done with two recorders, the first
        // of which will record the "entry point" of the ray in the
        // volume and the second the "leaving point" of the ray was
        // it not affected by the scattering.
        HitRecord rec1, rec2;

        if(!boundary->hit(r, -infinity, infinity, rec1)) return false;
        if(!boundary->hit(r, rec1.t + 0.0001f, infinity, rec2)) return false;

        if(rec1.t < tmin) rec1.t = tmin;
        if(rec2.t > tmax) rec2.t = tmax;
        if(rec1.t >= rec2.t) return false;
        if(rec1.t < 0.f) rec1.t = 0.f;

        // Next ww need to compute the actual distance that the ray
        // travels inside the volume. For any ray entering the volume,
        // the probability that it scatters after a small distance dl
        // is dP = C.dl, where C is the density of the volume. However,
        // because this volume is composed of volatile media, like fog
        // or mist, this density can be written as C = D.P, where D is
        // the actual (physical) density of the volume and P the
        // probability of "finding" the matter in the distance dl. Then
        // we get dP = D.P.dl, which is the same as dP/P = D.dl and after
        // integrating ln(P) = D.l, which gives us the distance.
        const auto ray_length = length(r.dir);
        const auto dist_in_boundary = (rec2.t - rec1.t) * ray_length;
        const auto hit_distance = -std::log(Random::rfloat()) / density;

        if(hit_distance > dist_in_boundary)
            return false;

        rec.t = rec1.t + hit_distance/ray_length;
        rec.p = r(rec.t);
        rec.normal = {1.f, 0.f, 0.f};
        rec.frontFace = true;
        rec.material = phase_func;

        return true;
    }
}