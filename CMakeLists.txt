cmake_minimum_required(VERSION 3.22)

project(corner VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 4.7.0 EXACT REQUIRED PATHS /home/policratus/.opencv-4.7.0)

add_library(cornerlib cornerlib/cornerlib.hpp cornerlib/cornerlib.cpp)
target_link_libraries(cornerlib ${OpenCV_LIBS})
add_executable(corner corner.cpp)
target_link_libraries(corner PRIVATE ${OpenCV_LIBS} cornerlib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
