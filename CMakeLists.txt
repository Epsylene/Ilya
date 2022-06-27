cmake_minimum_required(VERSION 3.22)
project(Ilya)

set(CMAKE_CXX_STANDARD 20)

file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

add_library(stb_image STATIC lib/stb_image/stb_image.cpp lib/stb_image/stb_image.h)
target_include_directories(stb_image PUBLIC lib/stb_image)

add_library(Ilya SHARED src/Utils/Vector4.hpp src/Utils/Color.cpp src/Objects/Ray.hpp src/Utils/Vector3.cpp src/Objects/Hittable.cpp src/Objects/Hittable.hpp src/Ilya.hpp src/Objects/Camera.hpp src/Objects/Material.cpp src/Objects/Material.hpp src/Objects/BoundingBox.hpp src/Objects/BoundingBox.cpp src/Objects/Texture.hpp src/Utils/Perlin.hpp src/Objects/Instances.cpp src/Objects/Instances.hpp src/Core/Application.cpp src/Core/Application.h)

target_link_libraries(Ilya PUBLIC stb_image)

target_compile_definitions(Ilya PUBLIC -DILYA_RES_DIR=${CMAKE_SOURCE_DIR}/res)
target_precompile_headers(Ilya PUBLIC src/Ilya.hpp)

set(ILYA_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib/stb_image)
target_include_directories(Ilya PUBLIC ${ILYA_INCLUDE_DIRS})

add_subdirectory(app)