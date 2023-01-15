
#include "Image.hpp"

#include <fmt/os.h>

namespace Ilya
{
    Image::Image(uint32_t width, uint32_t height, const fs::path& path):
        width(width), height(height), path(path), img(path)
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
        img << fmt::format("P3\n{} {}\n255\n", width, height);
    }

    Image::Image(const Image& img):
            Image(img.width, img.height, img.path)
    {}

    void Image::write(const Color& color)
    {
        auto r = static_cast<int>(256 * std::clamp(color.r, 0.f, 0.999f));
        auto g = static_cast<int>(256 * std::clamp(color.g, 0.f, 0.999f));
        auto b = static_cast<int>(256 * std::clamp(color.b, 0.f, 0.999f));

        img << fmt::format("{} {} {}\n", r, g, b);
    }
}