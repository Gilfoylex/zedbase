
cmake_minimum_required(VERSION 3.10)
project(zedbase)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/project-defaults.cmake)

file(GLOB HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h)

file(GLOB SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cc)

add_library(zedbase STATIC
    ${HEADERS}
    ${SOURCES})


# Winmm.lib
target_link_libraries(zedbase PRIVATE Winmm)

# 添加头文件目录
target_include_directories(zedbase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)