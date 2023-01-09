
#pragma once

#include "Ray.hpp"
#include "Texture.hpp"

namespace Ilya
{
    struct HitRecord;

    class Material
    {
        public:

            virtual Color emitted(float u, float v, const Vec3& p) const
            {
                return {};
            }

            virtual bool scatter(const Ray& in, Ray& out, Color& albedo,
                    const HitRecord& rec, float & pdf) const
            {
                return false;
            }

            virtual float scattering_pdf(const Ray& in, const Ray& out, const HitRecord& rec) const
            {
                return 0;
            }
    };

    /// Ideal diffuse reflection, where rays scatter uniformly in random
    /// directions off the surface.
    class Lambertian: public Material
    {
        public:

            std::shared_ptr<Texture> albedo;

            explicit Lambertian(const Color& albedo): albedo(std::make_shared<SolidColor>(albedo)) {}
            explicit Lambertian(const std::shared_ptr<Texture>& tex): albedo(tex) {}

            virtual bool
            scatter(const Ray& in, Ray& out, Color& albedo,
                    const HitRecord& rec, float & pdf) const override;

            virtual float scattering_pdf(const Ray& in, const Ray& out,
                                         const HitRecord& rec) const override;
    };

    /// Specular reflection: the rays scatter off the surface at the same
    /// angle with which they arrived. The fuziness parameter adds a bit
    /// of diffusivity to the material, so the metal has a more matte look.
    class Metal: public Material
    {
        public:

            Color albedo;
            float fuziness;

            explicit Metal(const Color& albedo, float fuziness):
                    albedo(albedo), fuziness(fuziness < 1 ? fuziness : 1) {}

            virtual bool
            scatter(const Ray& in, Ray& out, Color& albedo,
                    const HitRecord& rec, float & pdf) const override;
    };

    class Dielectric: public Material
    {
        public:

            float refraction;

            Dielectric(float refraction): refraction(refraction) {}

            virtual bool
            scatter(const Ray& in, Ray& out, Color& albedo,
                    const HitRecord& rec, float & pdf) const override;

        private:

            static float reflectance(float cos, float ratio)
            {
                // Schlick's approximation for reflectance
                auto r0 = (1.f - ratio)/(1.f + ratio);
                r0 *= r0;

                return r0 + (1.f - r0)*std::pow(1 - cos, 5);
            }
    };

    class DiffuseLight: public Material
    {
        public:

            std::shared_ptr<Texture> emitter;

            explicit DiffuseLight(const std::shared_ptr<Texture>& emitter):
                emitter(emitter) {}

            explicit DiffuseLight(const Color& c):
                emitter(std::make_shared<SolidColor>(c)) {}

            explicit DiffuseLight(float factor):
                DiffuseLight(Color{factor}) {}

            virtual bool
            scatter(const Ray& in, Ray& out, Color& albedo,
                    const HitRecord& rec, float & pdf) const override
            {
                return false;
            }

            virtual Color emitted(float u, float v, const Vec3& p) const override
            {
                return emitter->val(u, v, p);
            }
    };

    class Isotropic: public Material
    {
        public:

            std::shared_ptr<Texture> albedo;

            explicit Isotropic(const Color& c):
                albedo(std::make_shared<SolidColor>(c)) {}

            explicit Isotropic(const std::shared_ptr<Texture>& tex):
                albedo(tex) {}

            virtual bool
            scatter(const Ray& in, Ray& out, Color& albedo,
                    const HitRecord& rec, float & pdf) const override;
    };
}