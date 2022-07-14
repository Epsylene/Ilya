
#include "Image.hpp"

namespace Ilya
{
    Image::Image(uint32_t width, uint32_t height, const fs::path& path):
        image(path), width(width), height(height), path(path)
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
        image << "P3\n" << width << ' ' << height
          << "\n255\n";
    }

    void Image::write(const Color& color)
    {
        image << static_cast<int>(256 * std::clamp(color.r, 0.f, 0.999f)) << ' '
              << static_cast<int>(256 * std::clamp(color.g, 0.f, 0.999f)) << ' '
              << static_cast<int>(256 * std::clamp(color.b, 0.f, 0.999f)) << '\n';
    }

    Image::Image(const Image& img):
        Image(img.width, img.height, img.path)
    {}
}