
#include "Renderer.hpp"

namespace Ilya
{
    Renderer::Renderer(const Image& img, const HittableList& world, uint32_t samples,
                       uint32_t depth): img(img), world(world),
                        samples_per_pixel(samples), depth(depth)
    {}

    Color Renderer::ray_color(const Ray& r, std::shared_ptr<Hittable> light,
                        const Color& background, int depth)
    {
        HitRecord rec {};

        if(depth <= 0)
            return {};

        // Check if the ray hits the 'world' hittable, and bounce off
        // the surface with some Color attenuation (to approximate the
        // fact that part of the rays are being absorbed by the
        // material). The time at which rays are cast is not set exactly
        // at 0: if we did so, because of floating point rounding
        // errors, a bunch of the rays that were supposed to scatter off
        // the surface actually would start a little under the surface,
        // intersect with it and never get out. This would produce an
        // image riddled with random black pixels, an outcome known as
        // "shadow acne"; setting the cast time at which the ray
        // detection starts just a bit after 0 gets rid of most of it
        // spectacularly well.
        if(!world.hit(r, 0.001f, infinity, rec))
            return background;

        ScatterRecord scatter {};
        Color emitted = rec.material->emitted(rec.u, rec.v, rec.p, rec);
        // If the ray doesn't scatter from the material, it means that
        // it is emissive (it produces light); then the color we want to
        // output is the color of the emitted light.
        if(!rec.material->scatter(r, scatter, rec))
            return emitted;

        // If the ray reflection is specular, we don't need to play with
        // PDFs like we do later, because each incoming ray scatters in
        // a specific, calculable direction. The color change of the ray
        // is then reduced to the material's albedo factor.
        if(scatter.is_specular)
            return scatter.albedo * ray_color(scatter.ray, light, background, depth - 1);

        // If it is a regular material, create a mixture PDF from the
        // light-directed PDF and the material PDF (contained in the
        // ray scatter record).
        auto light_pdf = std::make_shared<HittablePDF>(light, rec.p);
        MixturePDF pdf {scatter.pdf, light_pdf};

        Ray scattered = {rec.p, pdf.random_vector(), r.cast_time};
        auto pdf_val = pdf.val(scattered.dir);

        // In the other cases, we want the colors from both the emitted
        // light and the ray itself. The ray color is multiplied by two
        // factors: the albedo, which is the material's reflection
        // color, and the scattering PDF, which represents the
        // scattering distribution of the material (that is, which
        // directions are privileged for the rays when scattering off
        // the surface of this material). Now, the final color will the
        // integration of this quantity over all directions. However,
        // because of how we render our image -- sending rays in random
        // directions --, actually integrating is neither simple nor
        // useful. What we do instead is a statistical average of the
        // color function, dividing by the value of the PDF for the
        // scattered ray.
        return emitted + scatter.albedo * rec.material->scattering_pdf(r, scattered, rec)
                         * ray_color(scattered, light, background, depth - 1) / pdf_val;
    }

    void Renderer::render(const Camera& cam, std::shared_ptr<Hittable> light)
    {
        // We render the image from top to bottom, because the image is
        // stored in memory from bottom to top. This is because the
        // image is stored in a 1D array, and the first pixel is the
        // bottom-left one.
        for (int j = img.height - 1; j >= 0; --j)
        {
            fmt::print("Scanlines remaining: {}\n", j);

            for (int i = 0; i < img.width; ++i)
            {
                Color pixel_Color {};

                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    auto u = (i + random_float()) / (img.width - 1);
                    auto v = (j + random_float()) / (img.height - 1);

                    pixel_Color += ray_color(cam.ray(u, v), light, {}, depth);
                }

                // Colors are scaled inversely proportional to the
                // samples per pixel: antialiasing works by taking a
                // pixel and sampling a great number of neighboring
                // pixels into it; this gives a blurry effect at the
                // edges, where colours change rapidly from one pixel to
                // another. However, because of the sampling, we have to
                // tone down each pixel's Color, or else the resulting
                // antialised pixel will be way brighter than it
                // actually is. However, our image is darker than it
                // should be, for another reason. Human eyes perceive
                // light in a non-linear fashion, following an
                // approximate power function. To account for this,
                // software perform what is known as gamma correction,
                // elevating the input at a power that lies tipically in
                // the range 1.8 to 2.2. Because image viewers expect
                // images to have been gamma-corrected, we need first to
                // inverse gamma-correct them; supposing that gamma = 2,
                // we need to elevate to the power 1/2.
                auto scale = 1.f/samples_per_pixel;
                pixel_Color = sqrt(pixel_Color * scale);

                img.write(pixel_Color);
            }

        }
    }
}