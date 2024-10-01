cmake_minimum_required(VERSION 3.25)
project(cache_characteristics)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-O0)

add_executable(cache_spec src/main.cpp)
