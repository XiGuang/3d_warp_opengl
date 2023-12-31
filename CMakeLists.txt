cmake_minimum_required(VERSION 3.26)
project(3d_warp_opengl)

set(CMAKE_CXX_STANDARD 20)

set(GL_Dir "C:/Program Files (x86)/GLFW")
find_package(OpenGL REQUIRED)

set(OpenCV_DIR "C:\\Program Files\\opencv\\sources\\mingw-win32-build\\install\\x64\\mingw\\staticlib")
set(yaml-cpp_DIR "C:\\Program Files (x86)\\YAML_CPP\\lib\\cmake\\yaml-cpp")
find_package(assimp CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(tinyxml2 REQUIRED)

file(GLOB_RECURSE srcs module/*/*.cpp)

# 定义要链接的库
set(LINK_LIBRARIES
        "C:\\Program Files (x86)\\GLFW\\lib\\libglfw3dll.a"
        assimp::assimp
        gflags pthread
        glog::glog
        ${OpenCV_LIBS}
        Eigen3::Eigen
        yaml-cpp::yaml-cpp
        tinyxml2::tinyxml2
)

set(LINK_DICT
        ${GL_Dir}/lib
)

set(INCLUDE_DICT
        ${GL_Dir}/include
        "C:\\Program Files (x86)\\glm"
)

# 链接库到 3d_warp_opengl
add_executable(3d_warp_opengl ${GL_Dir}/src/glad.c main.cpp ${srcs})
target_include_directories(3d_warp_opengl PRIVATE ${INCLUDE_DICT})
target_link_directories(3d_warp_opengl PRIVATE ${LINK_DICT})
target_link_libraries(3d_warp_opengl PRIVATE ${LINK_LIBRARIES})

# 链接库到 test
add_executable(test ${GL_Dir}/src/glad.c test.cpp ${srcs})
target_include_directories(test PRIVATE ${INCLUDE_DICT})
target_link_directories(test PRIVATE ${LINK_DICT})
target_link_libraries(test PRIVATE ${LINK_LIBRARIES})

add_executable(process_data process_data.cpp)
target_link_libraries(process_data PRIVATE tinyxml2::tinyxml2 glog::glog)
target_link_libraries(process_data PRIVATE  gflags pthread)