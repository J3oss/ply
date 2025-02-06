cmake_minimum_required(VERSION 3.5.0)

project(test VERSION 0.0.1 LANGUAGES C)

add_executable(test main.c)
target_link_libraries(test ply)

target_include_directories(test PRIVATE ..)
target_include_directories(test PRIVATE ../thirdparty)

file(COPY cube_little.ply DESTINATION .)
file(COPY cube_ascii.ply DESTINATION .)