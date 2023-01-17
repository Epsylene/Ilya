
#pragma once

#include "Ray.hpp"
#include "Texture.hpp"

namespace Ilya
{
    struct HitRecord;
    class PDF;

    /// Struct containing information on the scattering ray as well as
    /// the material's albedo, PDF and specularity.
    struct ScatterRecord
    {
        Ray ray {};
        bool is_specular;
        Color albedo {};
        std::shared_ptr<PDF> pdf;
    };

    class Material
    {
        public:

            virtual Color emitted(float u, float v, const Vec3& p,
                                  const HitRecord& rec) const
            {
                return {};
            }

            virtual bool scatter(const Ray& in, ScatterRecord& scatter,
                                 const HitRecord& rec) const
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

            explicit Lambertian(const Color& albedo): albedo(std::make_shared<SolidColor>(albedo)) {}
            explicit Lambertian(const std::shared_ptr<Texture>& tex): albedo(tex) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                         const HitRecord& rec) const override;

            float scattering_pdf(const Ray& in, const Ray& out,
                                 const HitRecord& rec) const override;

        public:

            std::shared_ptr<Texture> albedo;
    };

    /// Specular reflection: the rays scatter off the surface at the same
    /// angle with which they arrived. The fuziness parameter adds a bit
    /// of diffusivity to the material, so the metal has a more matte look.
    class Metal: public Material
    {
        public:

            explicit Metal(const Color& albedo, float fuziness):
                    albedo(albedo), fuziness(fuziness < 1 ? fuziness : 1) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                         const HitRecord& rec) const override;

        public:

            Color albedo;
            float fuziness;
    };

    class Dielectric: public Material
    {
        public:

            explicit Dielectric(float refraction):
                refraction(refraction) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                         const HitRecord& rec) const override;

        public:

            float refraction;

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

            explicit DiffuseLight(const std::shared_ptr<Texture>& emitter):
                emitter(emitter) {}

            explicit DiffuseLight(const Color& c):
                emitter(std::make_shared<SolidColor>(c)) {}

            explicit DiffuseLight(float factor):
                DiffuseLight(Color{factor}) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                    const HitRecord& rec) const override
            {
                return false;
            }

            Color emitted(float u, float v, const Vec3& p,
                          const HitRecord& rec) const override;

        public:

            std::shared_ptr<Texture> emitter;
    };

    class Isotropic: public Material
    {
        public:

            explicit Isotropic(const Color& c):
                albedo(std::make_shared<SolidColor>(c)) {}

            explicit Isotropic(const std::shared_ptr<Texture>& tex):
                albedo(tex) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                    const HitRecord& rec) const override;

        public:

            std::shared_ptr<Texture> albedo;
    };
}