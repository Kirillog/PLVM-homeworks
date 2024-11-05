cmake_minimum_required(VERSION 3.25)
project(lama_bytecode_analyzer)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-g)

add_executable(bytecode_analyzer 
    src/analyzer.cpp
    src/bytefile.cpp
    src/byteiter.cpp)

add_executable(disasm
    src/disasm.cpp
    src/bytefile.cpp)

