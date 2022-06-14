
#include "Hittable.hpp"

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

    // Once we got the solution, we save it in
    // the hit record, which we'll be able to
    // use later
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center(r.cast_time))/radius;
    rec.material = material;
    auto out_normal = (rec.p - center(r.cast_time))/radius;
    rec.face_normal(r, out_normal);

    return true;
}

Vec3 Sphere::center(float t) const
{
    return c0 + (t - t0)/(t1 - t0) * (c1 - c0);
}
