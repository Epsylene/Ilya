
#pragma once

#include "Hittable.hpp"

namespace Ilya
{
    class Translate: public Hittable
    {
        public:

            Vec3 offset;
            std::shared_ptr<Hittable> obj;

            Translate(const std::shared_ptr<Hittable>& obj, const Vec3& offset):
                obj(obj), offset(offset) {}

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override;
    };

    template<Axis axis>
    class Rotate: public Hittable
    {
        public:

            std::shared_ptr<Hittable> obj;
            BoundingBox box;
            bool hadbox;
            float sin, cos;

            Rotate(const std::shared_ptr<Hittable>& obj, float angle);

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override
            {
                box = this->box;

                return hadbox;
            }
    };
}