
#include "Material.hpp"
#include "Hittable.hpp"
#include "Utils/Random.hpp"

namespace Ilya
{

    Vec3 reflect(const Vec3& v, const Vec3& n)
    {
        return v - 2*dot(v, n)*n;
    }

    Vec3 refract(const Vec3& v, const Vec3& n, float ratio)
    {
        auto c = std::min(dot(-v, n), 1.f);
        auto t_normal = ratio * (v + c*n);
        auto t_tangent = -std::sqrt(std::abs(1.f - square(t_normal))) * n;

        return t_normal + t_tangent;
    }

    bool Lambertian::scatter(const Ray& in, ScatterRecord& scatter,
                             const HitRecord& rec) const
    {
        // Lambertian diffusion: contrary to specular reflection, where
        // the ray reflects off the surface at a precise angle, diffuse
        // reflection has rays scatter at many angles. The surface
        // luminance (that is, the luminous intensity per unit area of
        // light) of a perfect diffuse material is isotropic (uniform in
        // all directions). This means that the photon flux is the same
        // in all directions: the intensity, or radiance I of photons in
        // any given direction at an angle theta from above the
        // scattering point is then given by I = I0*cos(theta), which is
        // Lambert's cosine law.
        scatter.pdf = std::make_shared<CosinePDF>(rec.normal);
        scatter.albedo = albedo->val(rec.u, rec.v, rec.p);
        scatter.is_specular = false;

        return true;
    }

    float Lambertian::scattering_pdf(const Ray& in, const Ray& out,
                                     const HitRecord& rec) const
    {
        // Lambertian diffusion follows Lambert's cosine law: the
        // surface luminance (that is, the luminous intensity per unit
        // area of light) of a perfect diffuse material is isotropic
        // (uniform in all directions). can calculate with the dot
        // product of the material's surface normal and the scattering
        // ray direction (because N·L = |N||L|cos(theta) = cos(theta), N
        // and L being unitary)
        auto cos = dot(rec.normal, unit(out.dir));

        return cos < 0.f ? 0.f : cos/pi;
    }

    bool Metal::scatter(const Ray& in, ScatterRecord& scatter,
                        const HitRecord& rec) const
    {
        // Specular reflection is simple: the ray scatters off the
        // surface at the same angle, with opposite direction. The
        // fuziness parameter allows to add a diffusive component to our
        // metallic material: the more fuziness, the more rays there is
        // that do not scatter exactly like expected, and the more the
        // object has a matte look to it.
        auto reflected = reflect(unit(in.dir), rec.normal);
        scatter.ray = {rec.p, reflected + fuziness * Random::in_unit_sphere(), in.cast_time};

        scatter.albedo = albedo;
        scatter.is_specular = true;
        scatter.pdf = nullptr;

        return true;
    }

    bool Dielectric::scatter(const Ray& in, ScatterRecord& scatter,
                             const HitRecord& rec) const
    {
        // The surrounding medium is considered to be the air, so Snell
        // law's n/n' = 1/n'. If the rays were coming from under the
        // surface, just let them pass as if there was nothing there.
        float ratio = rec.frontFace ? (1.f/refraction) : 1.f;

        // The cosine (c) of the angle between the incoming ray and the
        // surface normal is given by the dot product of its (reoriented)
        // direction and the normal, clamped under 1. The sine (s) is then
        // simply sqrt(1 - c^2).
        auto udir = unit(in.dir);
        float c = std::min(dot(-udir, rec.normal), 1.f);
        float s = std::sqrt(1.f - c*c);

        Vec3 dir {};
        // On one hand, because of Snell's law, there can be no
        // refraction if (n/n')*sin(i) > 1. On the other hand, when
        // considering rays arriving at the dielectric, they will
        // tipically separate in a reflected and a refracted parts.
        // Rather than create two new rays, any incoming ray either
        // reflects or refracts (under Snell's law conditions) on a
        // random basis that depends on the reflectance of the material
        // (which depends on the refractive indices and the incoming
        // angle of the ray): if the reflectance (which ranges from 0 to
        // 1) is bigger than a random number between 0 and 1, then the
        // ray is reflected off the surface.
        if(ratio * s > 1.f || reflectance(c, ratio) > random_float())
            dir = reflect(udir, rec.normal);
        else
            dir = refract(udir, rec.normal, ratio);

        scatter.ray = {rec.p, dir, in.cast_time};
        scatter.albedo = Color::White;
        scatter.is_specular = true;
        scatter.pdf = nullptr;

        return true;
    }

    bool Isotropic::scatter(const Ray& in, ScatterRecord& scatter,
                            const HitRecord& rec) const
    {
        // In an isotropic material, rays are scattered off uniformly
        // in all directions: thus the direction of the scattered ray
        // is simply a point in the unit sphere.
        scatter.ray = {rec.p, Random::in_unit_sphere(), in.cast_time};
        scatter.albedo = albedo->val(rec.u, rec.v, rec.p);
        scatter.is_specular = false;
        scatter.pdf = nullptr;

        return true;
    }

    Color DiffuseLight::emitted(float u, float v, const Vec3& p,
                                const HitRecord& rec) const
    {
        if(!rec.frontFace)
            return {};

        return emitter->val(u, v, p);
    }
}