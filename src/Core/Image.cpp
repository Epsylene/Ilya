
#include "Image.hpp"

#include "Application.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "backends/imgui_impl_vulkan.h"

namespace Ilya
{
    namespace Utils
    {
        static uint32_t getVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
        {
            VkPhysicalDeviceMemoryProperties prop;
            vkGetPhysicalDeviceMemoryProperties(Application::getPhysicalDevice(), &prop);
            for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
            {
                if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
                    return i;
            }

            return 0xffffffff;
        }

        static uint32_t bytes_per_pixel(ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::None: return 0;
                case ImageFormat::RGBA: return 4;
                case ImageFormat::RGBA32F: return 16;
            }
            
            return 0;
        }

        static VkFormat to_Vulkan_format(ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::RGBA:    return VK_FORMAT_R8G8B8A8_UNORM;
                case ImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
            
            return (VkFormat)0;
        }
    }
    
    Image::Image(std::string_view path):
        path(path)
    {
        int width, height, channels;
        uint8_t* data = nullptr;

        if (stbi_is_hdr(this->path.c_str()))
        {
            data = (uint8_t*)stbi_loadf(this->path.c_str(), &width, &height, &channels, 4);
            format = ImageFormat::RGBA32F;
        }
        else
        {
            data = stbi_load(this->path.c_str(), &width, &height, &channels, 4);
            format = ImageFormat::RGBA;
        }

        this->width = width;
        this->height = height;

        allocate_memory(width * height * Utils::bytes_per_pixel(format));
        set_data(data);
    }

    Image::Image(uint32_t width, uint32_t height, ImageFormat format, const void* data)
            : width(width), height(height), format(format)
    {
        allocate_memory(width * height * Utils::bytes_per_pixel(format));
        if (data) set_data(data);
    }

    Image::~Image()
    {
        Application::submit_ressource_free(
                [sampler = sampler, imageView = imageView, image = image,
                        memory = memory, stagingBuffer = stagingBuffer, stagingBufferMemory = stagingBufferMemory]()
                {
                    VkDevice device = Application::getDevice();

                    vkDestroySampler(device, sampler, nullptr);
                    vkDestroyImageView(device, imageView, nullptr);
                    vkDestroyImage(device, image, nullptr);
                    vkFreeMemory(device, memory, nullptr);
                    vkDestroyBuffer(device, stagingBuffer, nullptr);
                    vkFreeMemory(device, stagingBufferMemory, nullptr);
                });

        sampler = nullptr;
        imageView = nullptr;
        image = nullptr;
        memory = nullptr;
        stagingBuffer = nullptr;
        stagingBufferMemory = nullptr;
    }

    void Image::set_data(const void* data)
    {
        VkDevice device = Application::getDevice();
        size_t upload_size = width * height * Utils::bytes_per_pixel(format);

        VkResult err;
        if (!stagingBuffer)
        {
            // Create the Upload Buffer
            {
                VkBufferCreateInfo buffer_info = {};
                buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                buffer_info.size = upload_size;
                buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                err = vkCreateBuffer(device, &buffer_info, nullptr, &stagingBuffer);
                check_vk_result(err);
                VkMemoryRequirements req;
                vkGetBufferMemoryRequirements(device, stagingBuffer, &req);
                alignedSize = req.size;
                VkMemoryAllocateInfo alloc_info = {};
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.allocationSize = req.size;
                alloc_info.memoryTypeIndex = Utils::getVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
                err = vkAllocateMemory(device, &alloc_info, nullptr, &stagingBufferMemory);
                check_vk_result(err);
                err = vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);
                check_vk_result(err);
            }

        }

        // Upload to Buffer
        {
            char* map = nullptr;
            err = vkMapMemory(device, stagingBufferMemory, 0, alignedSize, 0, (void**)(&map));
            check_vk_result(err);
            memcpy(map, data, upload_size);
            VkMappedMemoryRange range[1] = {};
            range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[0].memory = stagingBufferMemory;
            range[0].size = alignedSize;
            err = vkFlushMappedMemoryRanges(device, 1, range);
            check_vk_result(err);
            vkUnmapMemory(device, stagingBufferMemory);
        }

        // Copy to Image
        {
            VkCommandBuffer command_buffer = Application::getCommandBuffer(true);

            VkImageMemoryBarrier copy_barrier = {};
            copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier.image = image;
            copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_barrier.subresourceRange.levelCount = 1;
            copy_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &copy_barrier);

            VkBufferImageCopy region = {};
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.layerCount = 1;
            region.imageExtent.width = width;
            region.imageExtent.height = height;
            region.imageExtent.depth = 1;
            vkCmdCopyBufferToImage(command_buffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            VkImageMemoryBarrier use_barrier = {};
            use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier.image = image;
            use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            use_barrier.subresourceRange.levelCount = 1;
            use_barrier.subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &use_barrier);

            Application::flush_command_buffer(command_buffer);
        }
    }

    void Image::allocate_memory(uint64_t size)
    {
        VkDevice device = Application::getDevice();
        VkFormat vulkanFormat = Utils::to_Vulkan_format(format);
        VkResult err;

        // Create the Image
        {
            VkImageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = vulkanFormat;
            info.extent.width = width;
            info.extent.height = height;
            info.extent.depth = 1;
            info.mipLevels = 1;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            err = vkCreateImage(device, &info, nullptr, &image);
            check_vk_result(err);
            VkMemoryRequirements req;
            vkGetImageMemoryRequirements(device, image, &req);
            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = req.size;
            alloc_info.memoryTypeIndex = Utils::getVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
            err = vkAllocateMemory(device, &alloc_info, nullptr, &memory);
            check_vk_result(err);
            err = vkBindImageMemory(device, image, memory, 0);
            check_vk_result(err);
        }

        // Create the Image View:
        {
            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = image;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = vulkanFormat;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.layerCount = 1;
            err = vkCreateImageView(device, &info, nullptr, &imageView);
            check_vk_result(err);
        }

        // Create sampler:
        {
            VkSamplerCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            info.magFilter = VK_FILTER_LINEAR;
            info.minFilter = VK_FILTER_LINEAR;
            info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            info.minLod = -1000;
            info.maxLod = 1000;
            info.maxAnisotropy = 1.0f;
            VkResult err = vkCreateSampler(device, &info, nullptr, &sampler);
            check_vk_result(err);
        }

        // Create the Descriptor Set:
        descriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}