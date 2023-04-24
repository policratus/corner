cmake_minimum_required(VERSION 3.22)

project(tests VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include(CodeCoverage.cmake)
APPEND_COVERAGE_COMPILER_FLAGS()

set(libcornerinclude "${PROJECT_SOURCE_DIR}/../libcorner/include")
set(libcornersource "${PROJECT_SOURCE_DIR}/../libcorner/src")

find_package(OpenCV 4.7.0 EXACT REQUIRED PATHS /home/policratus/.opencv-4.7.0)

add_library(libcorner "${libcornerinclude}/libcorner.hpp" "${libcornersource}/libcorner.cpp")
target_include_directories(libcorner PRIVATE ${libcornerinclude})
target_link_libraries(libcorner PRIVATE ${OpenCV_LIBS})

find_package(Catch2 REQUIRED)
add_executable(tests tests.cpp)
target_include_directories(tests PRIVATE ${libcornerinclude})
target_link_libraries(tests libcorner ${OpenCV_LIBS} Catch2::Catch2WithMain)
