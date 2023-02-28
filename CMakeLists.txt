cmake_minimum_required(VERSION 3.22)
project(Ilya)

set(CMAKE_CXX_STANDARD 20)

# Bin #

file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Libs, include #

add_library(Ilya SHARED src/Utils/Color.cpp src/Objects/Ray.hpp src/Objects/Hittable.cpp src/Objects/Hittable.hpp src/Core.hpp src/Objects/Camera.hpp src/Objects/Material.cpp src/Objects/Material.hpp src/Objects/Bounds.hpp src/Objects/Bounds.cpp src/Objects/Texture.hpp src/Utils/Perlin.hpp src/Objects/Instances.cpp src/Objects/Instances.hpp src/Core/Renderer.cpp src/Core/Renderer.hpp src/Core/Image.cpp src/Core/Image.hpp src/ilpch.hpp src/Utils/Random.cpp src/Utils/Random.hpp src/Utils/PDF.hpp src/Utils/Transform.cpp src/Utils/Transform.hpp src/Utils/Math.cpp src/Utils/Math.hpp)

add_library(stb_image STATIC lib/stb_image/stb_image.cpp lib/stb_image/stb_image.h)
target_include_directories(stb_image PUBLIC lib/stb_image)
target_link_libraries(Ilya PUBLIC stb_image fmt)

add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)

set(ILYA_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib/stb_image ${CMAKE_SOURCE_DIR}/lib/fmt/include ${CMAKE_SOURCE_DIR}/lib/glm/)
target_include_directories(Ilya PUBLIC ${ILYA_INCLUDE_DIRS})

# Config #

target_precompile_headers(Ilya PUBLIC src/ilpch.hpp)

add_subdirectory(app)