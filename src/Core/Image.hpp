
#pragma once

#include <vulkan/vulkan.h>

namespace Ilya
{
    enum class ImageFormat
    {
        None = 0, RGBA, RGBA32F
    };

    class Image
    {
        public:

            Image(std::string_view path);
            Image(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);

            ~Image();

            void set_data(const void* data);

            uint32_t width = 0, height = 0;
            std::string path;
            ImageFormat format = ImageFormat::None;
            VkDescriptorSet descriptorSet = nullptr;

        private:

            void allocate_memory(uint64_t size);

            VkImage image = nullptr;
            VkImageView imageView = nullptr;
            VkDeviceMemory memory = nullptr;
            VkSampler sampler = nullptr;
            VkBuffer stagingBuffer = nullptr;
            VkDeviceMemory stagingBufferMemory = nullptr;

            size_t alignedSize = 0;
    };
}