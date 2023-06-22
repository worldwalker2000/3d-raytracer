cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(3d-raytracer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O2")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(GLM REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

file(GLOB proj_source_files "${source_dir}/*.cpp" "${source_dir}/*/*.cpp")
file(GLOB proj_include_files "${source_dir}/*.hpp" "${source_dir}/*/*.hpp")

add_executable(3d-raytracer ${proj_source_files} ${proj_include_files})

target_link_libraries(3d-raytracer ${SDL2_LIBRARIES})
