
#pragma once

#include "Ray.hpp"
#include "Texture.hpp"

struct HitRecord;

class Material
{
    public:

        virtual bool scatter(const Ray& in, Ray& out, Color& attenuation, const HitRecord& rec) const = 0;
};

class Lambertian: public Material
{
    public:

        std::shared_ptr<Texture> albedo;

        explicit Lambertian(const Color& albedo): albedo(std::make_shared<SolidColor>(albedo)) {}
        explicit Lambertian(const std::shared_ptr<Texture> tex): albedo(tex) {}

        virtual bool scatter(const Ray& in, Ray& out, Color& attenuation,
                             const HitRecord& rec) const override;
};

class Metal: public Material
{
    public:

        Color albedo;
        float fuziness;

        explicit Metal(const Color& albedo, float fuziness):
            albedo(albedo), fuziness(fuziness < 1 ? fuziness : 1) {}

        virtual bool scatter(const Ray& in, Ray& out, Color& attenuation,
                             const HitRecord& rec) const override;
};

class Dielectric: public Material
{
    public:

        float refraction;

        Dielectric(float refraction): refraction(refraction) {}

        virtual bool scatter(const Ray& in, Ray& out, Color& attenuation,
                             const HitRecord& rec) const override;

    private:

        static float reflectance(float cos, float ratio)
        {
            // Schlick's approximation for reflectance
            auto r0 = (1.f - ratio)/(1.f + ratio);
            r0 *= r0;

            return r0 + (1.f - r0)*std::pow(1 - cos, 5);
        }
};