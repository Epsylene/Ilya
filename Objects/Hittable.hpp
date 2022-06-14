
#pragma once

#include "Ilya.hpp"

#include "Utils/Vector3.hpp"
#include "Ray.hpp"
#include "Material.hpp"

struct HitRecord
{
    Vec3 p, normal;
    float t;
    bool frontFace;
    std::shared_ptr<Material> material;

    inline void face_normal(const Ray& r, const Vec3& outNormal)
    {
        // If the ray and the outwards-poiting normal point in opposite
        // directions, then the ray hits the front face of the object,
        // and our normal is 'outNormal' ; if not, it is the opposite.
        frontFace = dot(r.dir, outNormal) < 0;
        normal = frontFace ? outNormal : -outNormal;
    }
};

class Hittable
{
    public:

        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const = 0;
};

class HittableList: public Hittable
{
    public:

        std::vector<std::shared_ptr<Hittable>> objects;

        HittableList() = default;

        explicit HittableList(const std::shared_ptr<Hittable>& object)
        {
            objects.push_back(object);
        }

        void add(const std::shared_ptr<Hittable>& object)
        {
            objects.push_back(object);
        }

        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
};

class Sphere: public Hittable
{
    public:

        float t0, t1;
        Vec3 c0, c1;
        float radius;
        std::shared_ptr<Material> material;

        Sphere() = default;
        Sphere(const Vec3& center, float radius, const std::shared_ptr<Material>& mat):
            c0(center), c1(center), t0(0.f), t1(1.f), radius(radius), material(mat) {}

        Sphere(const Vec3& c0, const Vec3& c1, float t0, float t1, float radius, const std::shared_ptr<Material>& mat):
                c0(c0), c1(c1), t0(t0), t1(t1), radius(radius), material(mat) {}

        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

        Vec3 center(float t) const;
};