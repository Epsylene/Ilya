
#pragma once

#include "Utils/Math.hpp"
#include "Utils/Transform.hpp"
#include "Ray.hpp"
#include "Material.hpp"
#include "Bounds.hpp"

namespace Ilya
{
    /// Struct used to carry information about the point where the ray
    /// hit the surface (p), the normal to the surface at this point
    /// (normal), the time of impact (t), the UV coordinates of the
    /// surface at this point (u, v), whether it is a front face or not
    /// (frontFace), and the surface's material (material).
    struct HitRecord
    {
        Point3 p;
        Vec3 normal;
        float t;
        float u, v;
        bool frontFace;
        Ref<Material> material;

        inline void face_normal(const Ray& r, const Vec3& outNormal)
        {
            // If the ray and the outwards-poiting normal point in
            // opposite directions, then the ray hits the front face of
            // the object, and our normal is 'outNormal' ; if not, it is
            // the opposite.
            frontFace = dot(r.dir, outNormal) < 0;
            normal = frontFace ? outNormal : -outNormal;
        }
    };

    class Hittable
    {
        public:

            /// Tells whether the ray `r` hits the object between `tmin`
            /// and `tmax`, and puts that information in the hit record
            /// `rec`.
            virtual bool hit(const Ray& r, float tmin, float tmax,
                             HitRecord& rec) const = 0;

            /// Creates a bounding box `box` around the object between
            /// times t0 and t1.
            virtual bool bounding_box(Bounds& box, float t0, float t1) const = 0;

            /// Returns a random vector between `origin` and a point on
            /// the surface of the object.
            virtual Point3 random_point(const Point3& origin) const
            {
                return {};
            }

            /// Returns the probability for the ray `r` to hit the
            /// object on a point of the surface.
            virtual float pdf_value(const Ray& r)
            {
                return 0.f;
            }
    };

    /// List of hittables, basically an improved vector of `Hittable`
    /// references.
    class HittableList: public Hittable
    {
        public:

            HittableList() = default;

            explicit HittableList(const Ref<Hittable>& object)
            {
                objects.push_back(object);
            }

            void add(const Ref<Hittable>& object)
            {
                objects.push_back(object);
            }

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
            bool bounding_box(Bounds& box, float t0, float t1) const override;

            Point3 random_point(const Point3& origin) const override;

            float pdf_value(const Ray& r) override;

        public:

            std::vector<Ref<Hittable>> objects;
    };

    /// Generates a BVH (Bounding Volume Hierarchy),
    class BVHnode: public Hittable
    {
        public:

            explicit BVHnode(const HittableList& list, float t0 = 0.f,
                             float t1 = 1.f): BVHnode(list.objects, 0,
                                                      list.objects.size(),
                                                      t0, t1) {}

            BVHnode(const std::vector<Ref<Hittable>>& objects,
                    size_t start, size_t end, float t0, float t1);

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
            bool bounding_box(Bounds& box, float t0, float t1) const override;

        private:

            Ref<Hittable> left, right;
            Bounds box;
    };

    class Sphere: public Hittable
    {
        public:

            Sphere(const Vec3& center, float radius, const Ref<Material>& mat):
                    c0(center), c1(center), t0(0.f), t1(1.f), radius(radius), material(mat) {}

            Sphere(const Vec3& c0, const Vec3& c1, float t0, float t1, float radius, const Ref<Material>& mat):
                    c0(c0), c1(c1), t0(t0), t1(t1), radius(radius), material(mat) {}

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
            bool bounding_box(Bounds& box, float t0, float t1) const override;

            Point3 random_point(const Point3& origin) const override;

            float pdf_value(const Ray& r) override;

            Point3 center(float t) const;

        public:

            float t0, t1;
            Point3 c0, c1;
            float radius;
            Ref<Material> material;

        private:

            static std::pair<float, float> sphere_uv(const Vec3& p)
            {
                // To get u and v on the sphere, we first need to get the
                // sphere coordinates theta and phi; we know theta in
                // spherical coordinates x = -cos(phi)sin(theta),
                // y = -cos(theta) and z = sin(phi)sin(theta). Inverting
                // those equations give us theta and phi; we then only need
                // to tweak phi's equation a little so that it doesn't
                // range from -pi to pi but from 0 to 2*pi.
                auto theta = std::acos(-p.y);
                auto phi = std::atan2(-p.z, p.x) + pi;

                return { phi/(2*pi), theta/pi };
            }
    };

    template<Axis ax0, Axis ax1> requires (ax0 < ax1)
    class Rectangle: public Hittable
    {
        public:

            Rectangle(float r0, float s0, float r1, float s1, float k,
                      const Ref<Material>& mat):
                      r0(r0), s0(s0), r1(r1), s1(s1), k(k), material(mat) {}

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;
            bool bounding_box(Bounds& box, float t0, float t1) const override;
            Point3 random_point(const Point3& origin) const override;
            float pdf_value(const Ray& r) override;

        public:

            float r0, s0, r1, s1, k;
            Ref<Material> material;
    };

    class Box: public Hittable
    {
        public:

            Box(const Vec3& p0, const Vec3& p1, Ref<Material> mat);

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            bool bounding_box(Bounds& box, float t0, float t1) const override
            {
                box = {p0, p1};

                return true;
            }

        public:

            Point3 p0, p1;
            HittableList sides;
    };

    class ConstantMedium: public Hittable
    {
        public:

            ConstantMedium(const Ref<Hittable>& boundary,
                           const Ref<Texture>& tex, float density):
                           boundary(boundary), phase_func(std::make_shared<Isotropic>(tex)), density(density) {}

            ConstantMedium(const Ref<Hittable>& boundary, const Color& c, float density):
                    ConstantMedium(boundary, std::make_shared<SolidColor>(c), density) {}

            bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            bool bounding_box(Bounds& box, float t0, float t1) const override
            {
                return boundary->bounding_box(box, t0, t1);
            }

        public:

            float density;
            Ref<Hittable> boundary;
            Ref<Material> phase_func;
    };
}