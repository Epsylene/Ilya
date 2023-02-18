
#include "Image.hpp"

#include <fmt/os.h>

namespace Ilya
{
    Image::Image(uint32_t width, uint32_t height, const fs::path& path):
        width(width), height(height), path(path), img(fmt::output_file(path.string()))
    {
        // Outputting an image file: we use a PPM file, which is a
        // simple format looking like
        //
        //      P3
        //      [columns] [rows]
        //      [max_Color_value]
        //      [pixel00] [pixel01] ... [pixel0n]
        //      [pixel10] ...
        //      ...
        //
        img.print("P3\n{} {}\n255\n", width, height);
    }

    Image::Image(const Image& img):
            Image(img.width, img.height, img.path)
    {}

    void Image::write(const Color& color)
    {
        auto [r, g, b, a] = color;

        // Check for NaNs
        if(std::isnan(r)) r = 0.f;
        if(std::isnan(g)) g = 0.f;
        if(std::isnan(b)) b = 0.f;

        // Clamp colors to the [0, 256[ range.
        r = 256 * std::clamp(r, 0.f, 0.999f);
        g = 256 * std::clamp(g, 0.f, 0.999f);
        b = 256 * std::clamp(b, 0.f, 0.999f);

        img.print("{} {} {}\n", (int)r, (int)g, (int)b);
    }
}