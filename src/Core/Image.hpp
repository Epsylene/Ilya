
#pragma once

#include "Core.hpp"
#include "Utils/Color.hpp"

namespace Ilya
{
    class Image
    {
        public:

            uint32_t width, height;
            fs::path path;

            explicit Image(uint32_t width, uint32_t height, const fs::path& path = app_path/"image.ppm");
            Image(const Image& img);

            void write(const Color& color);

        private:

            std::ofstream image;
    };
}