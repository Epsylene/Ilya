
#pragma once

#include <vector>
#include <memory>

#include "../Utils/Vector3.hpp"
#include "Ray.hpp"

struct HitRecord
{
    Vec3 p, normal;
    float t;
    bool frontFace;

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

        Vec3 center;
        float radius;

        Sphere() = default;
        Sphere(const Vec3& center, float radius):
            center(center), radius(radius) {}

        virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
};