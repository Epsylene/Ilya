
#pragma once

#include "ilpch.hpp"
#include "ImGui/Layer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Ilya
{
    struct WindowData
    {
        std::string name = "Ilya app";
        uint32_t width = 400;
        uint32_t height = 400;
    };

    class Application
    {
        public:

            explicit Application(const WindowData& windowData = {});
            ~Application();

            template<typename T> requires std::is_base_of_v<Layer, T>
            void pushLayer()
            {
                layers.emplace_back(std::make_shared<T>());
            }

            void run();
            void close();

        private:

            WindowData data;
            GLFWwindow* window = nullptr;
            bool running = true;
            std::vector<std::shared_ptr<Layer>> layers;
    };
}