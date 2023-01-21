
#include "Instances.hpp"

namespace Ilya
{
    bool Translate::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        // In raytracing, translation and rotation is often made using
        // what is called instancing: instead of immediate-mode actions
        // (aka a translate(), rotate() functions for each object class,
        // which demands full information on the object's geometry), two
        // new Hittable classes are created, which take Hittable
        // instances and represent them translated and/or rotated. That
        // is, if one creates a Hittable A, then translation of A is
        // performed via A = Translate{A, ...}; A is still a Hittable
        // object, but of class Translate containing a pointer to the
        // actual hittable (box, sphere, etc). Then, when hit detection
        // is performed, it is the `Translate` class hit function that
        // is called, which allow us to work on the incoming ray instead
        // of the actual object's geometry in order to do the
        // translation or rotation. For example, translating the object
        // A by 'offset' is the same as leaving the object A where it is
        // and translating all the incoming rays by '-offset'; this is
        // what is done here.
        Ray translated_r {r.orig - offset, r.dir, r.cast_time};
        if(!obj->hit(translated_r, tmin, tmax, rec))
            return false;

        rec.p += offset;

        return true;
    }

    bool Translate::bounding_box(BoundingBox& box, float t0, float t1) const
    {
        if(!obj->bounding_box(box, t0, t1))
            return false;

        // The bounding box itself doesn't change when translating, it
        // is only displaced by some amount.
        box = {box.min + offset, box.max + offset};

        return true;
    }

    template<Axis axis>
    Rotate<axis>::Rotate(const Ref<Hittable>& obj, float angle):
        obj(obj)
    {
        hadbox = obj->bounding_box(box, 0.f, 1.f);

        auto theta = radians(angle);
        sin = std::sin(theta);
        cos = std::cos(theta);

        Vec3 min {infinity}, max {-infinity};
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    // Go through each vertex of the box...
                    auto x = i*box.max.x + (1 - i)*box.min.x;
                    auto y = j*box.max.y + (1 - j)*box.min.y;
                    auto z = k*box.max.z + (1 - k)*box.min.z;

                    // ...rotate along the axis (note that this is
                    // a rotation of '-angle', just like we did a
                    // translation of '-offset')...
                    float xp, yp, zp;
                    if constexpr(axis == Axis::X)
                    {
                        xp = x;
                        yp = cos*y + sin*z;
                        zp = -sin*y + cos*z;
                    }
                    else if constexpr(axis == Axis::Y)
                    {
                        xp = cos*x + sin*z;
                        yp = y;
                        zp = -sin*x + cos*z;
                    }
                    else if constexpr(axis == Axis::Z)
                    {
                        xp = cos*x + sin*y;
                        yp = -sin*x + cos*y;
                        zp = z;
                    }

                    // ...and clamp {min, max} components to the new
                    // rotated values, so that we end up with an
                    // axis-aligned tightly bounding box for our
                    // rotated object.
                    Vec3 test {xp, yp, zp};
                    for (int c = 0; c < 3; ++c)
                    {
                        min[c] = std::min(min[c], test[c]);
                        max[c] = std::max(max[c], test[c]);
                    }
                }
            }
        }

        box = {min, max};
    }

    template<Axis axis>
    bool Rotate<axis>::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
    {
        int ax1, ax2;
        if constexpr(axis == Axis::X)
        {
            ax1 = (int)Axis::Y;
            ax2 = (int)Axis::Z;
        }
        else if constexpr(axis == Axis::Y)
        {
            ax1 = (int)Axis::X;
            ax2 = (int)Axis::Z;
        }
        else if constexpr(axis == Axis::Z)
        {
            ax1 = (int)Axis::X;
            ax2 = (int)Axis::Y;
        }

        auto orig = r.orig;
        auto dir = r.dir;
        
        orig[ax1] = cos*r.orig[ax1] - sin*r.orig[ax2];
        orig[ax2] = sin*r.orig[ax1] + cos*r.orig[ax2];

        dir[ax1] = cos*r.dir[ax1] - sin*r.dir[ax2];
        dir[ax2] = sin*r.dir[ax1] + cos*r.dir[ax2];

        Ray rp {orig, dir, r.cast_time};

        if(!obj->hit(rp, tmin, tmax, rec))
            return false;

        auto p = rec.p;
        auto normal = rec.normal;

        p[ax1] =  cos*rec.p[ax1] + sin*rec.p[ax2];
        p[ax2] = -sin*rec.p[ax1] + cos*rec.p[ax2];

        normal[ax1] =  cos*rec.normal[ax1] + sin*rec.normal[ax2];
        normal[ax2] = -sin*rec.normal[ax1] + cos*rec.normal[ax2];

        rec.p = p;
        rec.face_normal(rp, normal);

        return true;
    }

    template class Rotate<Axis::X>;
    template class Rotate<Axis::Y>;
    template class Rotate<Axis::Z>;

    Ref<Translate> translate(const Ref<Hittable>& obj, const Vec3& offset)
    {
        return std::make_shared<Translate>(obj, offset);
    }

    template<Axis N>
    Ref<Rotate<N>> rotate(const Ref<Hittable>& obj, float angle)
    {
        return std::make_shared<Rotate<N>>(obj, angle);
    }

    Ref<Flip> flip(const Ref<Hittable>& obj)
    {
        return std::make_shared<Flip>(obj);
    }

    template Ref<Rotate<Axis::X>> rotate<Axis::X>(const Ref<Hittable>&, float);
    template Ref<Rotate<Axis::Y>> rotate<Axis::Y>(const Ref<Hittable>&, float);
    template Ref<Rotate<Axis::Z>> rotate<Axis::Z>(const Ref<Hittable>&, float);
}