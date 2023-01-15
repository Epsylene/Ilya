
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

            Rotate(const std::shared_ptr<Hittable>& obj, float angle);

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override
            {
                box = this->box;

                return hadbox;
            }

        public:

            std::shared_ptr<Hittable> obj;
            BoundingBox box;
            bool hadbox;
            float sin, cos;
    };

    class Flip: public Hittable
    {
        public:

            Flip(std::shared_ptr<Hittable> obj): obj(obj) {}

            virtual bool hit(const Ray& r, float tmin, float tmax,
                             HitRecord& rec) const override
            {
                if(!obj->hit(r, tmin, tmax, rec))
                    return false;

                rec.frontFace = !rec.frontFace;
                return true;
            }

            virtual bool bounding_box(float t0, float t1,
                                      BoundingBox& box) const override
            {
                return obj->bounding_box(t0, t1, box);
            }

        public:

            std::shared_ptr<Hittable> obj;
    };
}