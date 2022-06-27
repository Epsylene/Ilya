cmake_minimum_required(VERSION 3.22)
project(Ilya)

set(CMAKE_CXX_STANDARD 20)

file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# DEPENDENCIES #

add_library(imgui STATIC lib/imgui/imgui_tables.cpp
        lib/imgui/imconfig.h
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui.h
        lib/imgui/imgui.cpp
        lib/imgui/imgui_internal.h
        lib/imgui/imstb_rectpack.h
        lib/imgui/imgui_widgets.cpp
        lib/imgui/imstb_textedit.h
        lib/imgui/imstb_truetype.h
        lib/imgui/backends/imgui_impl_vulkan.h)
target_include_directories(imgui PUBLIC lib/imgui)

add_library(stb_image STATIC lib/stb_image/stb_image.cpp lib/stb_image/stb_image.h)
target_include_directories(stb_image PUBLIC lib/stb_image)

add_subdirectory(lib/glfw)
find_package(Vulkan REQUIRED)

# LIBRARY #
add_library(Ilya SHARED src/Utils/Vector4.hpp src/Utils/Color.cpp src/Objects/Ray.hpp src/Utils/Vector3.cpp src/Objects/Hittable.cpp src/Objects/Hittable.hpp src/Ilya.hpp src/Objects/Camera.hpp src/Objects/Material.cpp src/Objects/Material.hpp src/Objects/BoundingBox.hpp src/Objects/BoundingBox.cpp src/Objects/Texture.hpp src/Utils/Perlin.hpp src/Objects/Instances.cpp src/Objects/Instances.hpp src/Core/Application.cpp src/Core/Application.hpp src/ilpch.hpp src/Core/ImGui/ImGuiBuild.cpp src/Core/ImGui/Layer.hpp)

target_link_libraries(Ilya PUBLIC stb_image glfw Vulkan::Vulkan imgui)

target_compile_definitions(Ilya PUBLIC -DILYA_RES_DIR=${CMAKE_SOURCE_DIR}/res)
target_precompile_headers(Ilya PUBLIC src/Ilya.hpp)

set(ILYA_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib/stb_image ${CMAKE_SOURCE_DIR}/lib/glfw ${CMAKE_SOURCE_DIR}/lib/imgui)
target_include_directories(Ilya PUBLIC ${ILYA_INCLUDE_DIRS})

add_subdirectory(app)