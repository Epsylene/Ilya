
#pragma once

#include "Hittable.hpp"

namespace Ilya
{
    /// Instancing class for translation: rather than applying
    /// immediate-mode transformations on objects whose geometry
    /// we may not know, instance classes wrap a hittable in a
    /// class representing an action on the object and where each
    /// function is overriden to simulate the effect of the action
    /// on the object.
    class Translate: public Hittable
    {
        public:

            Translate(const Ref<Hittable>& obj, const Vec3& offset):
                obj(obj), offset(offset) {}

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
            bool bounding_box(BoundingBox& box, float t0, float t1) const override;

        private:

            Vec3 offset;
            Ref<Hittable> obj;
    };

    /// Instancing class for rotations on an axis (see the `Translate`
    /// class for more information on instancing).
    template<Axis axis>
    class Rotate: public Hittable
    {
        public:

            Rotate(const Ref<Hittable>& obj, float angle);

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            bool bounding_box(BoundingBox& box, float t0, float t1) const override
            {
                box = this->box;

                return hadbox;
            }

        private:

            Ref<Hittable> obj;
            BoundingBox box {};
            bool hadbox;
            float sin, cos;
    };

    /// Instancing class for flipping the object's normals (see the
    /// `Translate` class for more information on instancing).
    class Flip: public Hittable
    {
        public:

            explicit Flip(const Ref<Hittable>& obj): obj(obj) {}

            bool hit(const Ray& r, float tmin, float tmax,
                             HitRecord& rec) const override
            {
                if(!obj->hit(r, tmin, tmax, rec))
                    return false;

                rec.frontFace = !rec.frontFace;
                return true;
            }

            bool bounding_box(BoundingBox& box, float t0, float t1) const override
            {
                return obj->bounding_box(box, t0, t1);
            }

            Vec3 random_point(const Vec3& origin) const override
            {
                return obj->random_point(origin);
            }

            float pdf_value(const Ray& r) override
            {
                return obj->pdf_value(r);
            }

        private:

            Ref<Hittable> obj;
    };

    Ref<Translate> translate(const Ref<Hittable>& obj, const Vec3& offset);
    template<Axis N> Ref<Rotate<N>> rotate(const Ref<Hittable>& obj, float angle);
    Ref<Flip> flip(const Ref <Hittable>& obj);
}