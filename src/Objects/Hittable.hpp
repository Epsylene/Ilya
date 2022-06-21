
#pragma once

#include "Ilya.hpp"

#include "Utils/Vector3.hpp"
#include "Ray.hpp"
#include "Material.hpp"
#include "BoundingBox.hpp"

namespace Ilya
{
    struct HitRecord
    {
        Vec3 p, normal;
        float t;
        float u, v;
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
            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const = 0;
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

            virtual bool
            bounding_box(float t0, float t1, BoundingBox& box) const override;
    };

    class BVHnode: public Hittable
    {
        public:

            std::shared_ptr<Hittable> left, right;
            BoundingBox box;

            explicit BVHnode(const HittableList& list, float t0 = 0.f, float t1 = 1.f):
                    BVHnode(list.objects, 0, list.objects.size(), t0, t1) {}

            BVHnode(const std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end, float t0, float t1);

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override;
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
            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override;

            Vec3 center(float t) const;

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

    enum class Axis
    {
        X = 0, Y = 1, Z = 2
    };

    template<Axis ax0, Axis ax1>
    concept XY = (ax0 == Axis::X && ax1 == Axis::Y);

    template<Axis ax0, Axis ax1>
    concept XZ = (ax0 == Axis::X && ax1 == Axis::Z);

    template<Axis ax0, Axis ax1>
    concept YZ = (ax0 == Axis::Y && ax1 == Axis::Z);

    template<Axis ax0, Axis ax1> requires (ax0 < ax1)
    class Rectangle: public Hittable
    {
        public:

            float r0, s0, r1, s1, k;
            std::shared_ptr<Material> material;

            Rectangle(float r0, float s0, float r1, float s1, float k,
                      const std::shared_ptr<Material>& mat):
                      r0(r0), s0(s0), r1(r1), s1(s1), k(k), material(mat) {}

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override
            {
                if constexpr(XY<ax0, ax1>)
                    box = {{r0, s0, k - 0.0001f}, {r1, s1, k + 0.0001f}};
                else if constexpr(XZ<ax0, ax1>)
                    box = {{r0, k - 0.0001f, s0 }, {r1, k + 0.0001f, s1}};
                else if constexpr(YZ<ax0, ax1>)
                    box = {{k - 0.0001f, r0, s0 }, {k + 0.0001f, r1, s1}};

                return true;
            }
    };

    class Box: public Hittable
    {
        public:

            Vec3 p0, p1;
            HittableList sides;

            Box(const Vec3& p0, const Vec3& p1, std::shared_ptr<Material> mat);

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override
            {
                box = {p0, p1};

                return true;
            }
    };

    class ConstantMedium: public Hittable
    {
        public:

            float density;
            std::shared_ptr<Hittable> boundary;
            std::shared_ptr<Material> phase_func;

            ConstantMedium(const std::shared_ptr<Hittable>& boundary,
                           const std::shared_ptr<Texture>& tex, float density):
                           boundary(boundary), phase_func(std::make_shared<Isotropic>(tex)), density(density) {}

            ConstantMedium(std::shared_ptr<Hittable> boundary, const Color& c, float density):
                    ConstantMedium(boundary, std::make_shared<SolidColor>(c), density) {}

            virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

            virtual bool bounding_box(float t0, float t1, BoundingBox& box) const override
            {
                return boundary->bounding_box(t0, t1, box);
            }
    };
}