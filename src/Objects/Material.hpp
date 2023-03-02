
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
        Color albedo {};
        bool is_specular;
        Ref<PDF> pdf;
    };

    class Material
    {
        public:

            /// Color emitted by the material.
            virtual Color emitted(float u, float v, const Point3& p,
                                  const HitRecord& rec) const
            {
                return {};
            }

            /// Function that tells if the ray `in` scatters off the
            /// surface of the material and puts that information on
            /// the scatter and hit records.
            virtual bool scatter(const Ray& in, ScatterRecord& scatter,
                                 const HitRecord& rec) const
            {
                return false;
            }

            /// Probability for the ray `in` to scatter as `out`, as per
            /// the material PDF.
            virtual float scattering_pdf(const Ray& in, const Ray& out,
                                         const HitRecord& rec) const
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
            explicit Lambertian(const Ref<Texture>& tex): albedo(tex) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                         const HitRecord& rec) const override;

            float scattering_pdf(const Ray& in, const Ray& out,
                                 const HitRecord& rec) const override;

        public:

            Ref<Texture> albedo;
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

    /// In physics, dielectrics are electrical insulators that can be
    /// polarised by an applied electric field (that is, materials where
    /// the electric current does not flow freely, but that under the
    /// action of an external field start producing their own). When an
    /// electromagnetic wave (like light) hits the material, it is
    /// divided in two parts, one that is reflected off the surface, and
    /// one that is refracted inside the material. The first element is
    /// the only one happening in conductors, because of the free
    /// charges on the surface that create a wave which interferes
    /// destructively below the surface (this almost-perfect reflection
    /// is also the reason why metals are shiny). The second element is
    /// the one allowing for a number of dielectrics to be transparent:
    /// light is able to pass through the material, to an extent, making
    /// things visible on both sides.
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

    /// A material that produces diffuse light, that is, light going in
    /// all directions in a random way.
    class DiffuseLight: public Material
    {
        public:

            explicit DiffuseLight(const Ref<Texture>& emitter):
                emitter(emitter) {}

            explicit DiffuseLight(const Color& c):
                emitter(std::make_shared<SolidColor>(c)) {}

            explicit DiffuseLight(float factor):
                DiffuseLight(Color{factor}) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                    const HitRecord& rec) const override
            {
                // If it's a light, we don't want rays to scatter off
                // it, because that's where they actually physically
                // come from (the whole point of raytracing being that
                // rays are sent from the camera to hit objects and
                // scatter off surfaces up until they hit a light
                // source, at which point a light path has been found,
                // and color can be output on the image for the
                // corresponding pixel).
                return false;
            }

            Color emitted(float u, float v, const Point3& p,
                          const HitRecord& rec) const override;

        public:

            Ref<Texture> emitter;
    };

    /// A material is isotropic if it sends rays around uniformly. This is
    /// the case for constant density mediums, for example, like some smoke
    /// or a fog, which are traversed by rays until they hit a particle of
    /// the medium and scatter in a random direction.
    class Isotropic: public Material
    {
        public:

            explicit Isotropic(const Color& c):
                albedo(std::make_shared<SolidColor>(c)) {}

            explicit Isotropic(const Ref<Texture>& tex):
                albedo(tex) {}

            bool scatter(const Ray& in, ScatterRecord& scatter,
                    const HitRecord& rec) const override;

        public:

            Ref<Texture> albedo;
    };
}