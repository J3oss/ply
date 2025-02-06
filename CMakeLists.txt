cmake_minimum_required(VERSION 3.15)

project(ply VERSION 0.0.1 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PLY_BUILD_TEST "Build Test" ON)
option(PLY_LIB "Build Lib" ON)

if(PLY_LIB)
    add_library(ply ply.c thirdparty/cvector/cvec.c)
    target_include_directories(ply PRIVATE thirdparty)
endif()

if(PLY_BUILD_TEST)
    add_subdirectory(test)
endif()