cmake_minimum_required(VERSION 3.26)
project(3d_warp_opengl)

set(CMAKE_CXX_STANDARD 20)

set(GL_Dir "C:/Program Files (x86)/GLFW")
find_package(assimp CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

file(GLOB_RECURSE srcs module/*/*.cpp)
add_executable(3d_warp_opengl ${GL_Dir}/src/glad.c main.cpp ${srcs})

include_directories(${GL_Dir}/include) # 头文件和外部库的位置
link_directories(${GL_Dir}/lib)
target_link_libraries(3d_warp_opengl ${GL_Dir}/bin/glfw3.dll)

target_link_libraries(3d_warp_opengl assimp::assimp)
include_directories("C:\\Program Files (x86)\\glm")

target_link_libraries(3d_warp_opengl gflags pthread)
target_link_libraries(3d_warp_opengl glog::glog)
target_link_libraries(3d_warp_opengl freetype)