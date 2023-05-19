cmake_minimum_required(VERSION 3.22)

set(CMAKE_BUILD_TYPE Release)

project(corner VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(libcornerinclude "${PROJECT_SOURCE_DIR}/libcorner/include")
set(libcornersource "${PROJECT_SOURCE_DIR}/libcorner/src")

find_package(OpenCV REQUIRED)

add_library(libcorner SHARED "${libcornerinclude}/libcorner.hpp" "${libcornersource}/libcorner.cpp")
set_target_properties(libcorner PROPERTIES PREFIX "lib")
set_target_properties(libcorner PROPERTIES OUTPUT_NAME "corner-${PROJECT_VERSION}")
target_include_directories(libcorner PRIVATE ${libcornerinclude})
target_link_libraries(libcorner PRIVATE ${OpenCV_LIBS})

install(TARGETS libcorner DESTINATION lib)
install(FILES "${libcornerinclude}/libcorner.hpp" DESTINATION innclude)

add_executable(corner corner.cpp)
target_include_directories(corner PRIVATE ${libcornerinclude})
target_link_libraries(corner PRIVATE ${OpenCV_LIBS} libcorner)

install(TARGETS corner DESTINATION bin)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(packages)
