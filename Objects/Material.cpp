
#include "Material.hpp"

#include "Hittable.hpp"

bool Lambertian::scatter(const Ray& in, Ray& out, Color& attenuation,
                         const HitRecord& rec) const
{
    // Lambertian diffusion : contrary to specular reflection,
    // where the ray reflects off the surface at a precise angle,
    // diffuse reflection has rays scatter at many different
    // angles. Lambertian diffusion is the ideal case of diffuse
    // reflection, where the ray scatter following a cosinus law.
    // Geometrically, this means that the ray flux has the shape
    // of a unit circle tangent to the scattering point. The
    // relation with the cosinus law can then be understood
    // when one considers the angle between the center of the
    // circle, the scattering point and a random point on the
    // surface of the sphere: when this angle is 0, the distance
    // between the scattering point and the point on the surface
    // is maximal, and when it is pi/2, it is minimal, just like
    // with the cosinus function. We thus need simply for our
    // new ray direction to place a random point on the surface
    // of this unit sphere, by offseting by the normal (which
    // is unit-length) and then adding a random unit vector.
    auto scattered = rec.normal + random_unit_vector();
    if(near_zero(scattered))
        scattered = rec.normal;

    out = {rec.p, scattered};
    attenuation = albedo;

    return true;
}

bool Metal::scatter(const Ray& in, Ray& out, Color& attenuation,
                    const HitRecord& rec) const
{
    // Specular reflection is simple: the ray scatters off the
    // surface at the same angle, with opposite direction. Doing
    // some geometry, we can then write that the reflected ray
    // direction is:
    auto udir = unit(in.dir);
    auto reflected = udir - 2*dot(udir, rec.normal)*rec.normal;

    // The fuziness parameter allows to add a diffusive component to
    // our metallic material: the more fuziness, the more rays there is
    // that do not scatter exactly like expected, and the more the
    // object has a matte look to it.
    out = {rec.p, reflected + fuziness * rand_in_unit_sphere()};
    attenuation = albedo;

    // We only want scattering to happen if rays are indeed reflected
    // off the surface.
    return (dot(out.dir, rec.normal) > 0);
}
