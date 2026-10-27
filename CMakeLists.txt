# CMakeLists.txt —— 本工程的工程文件（CMake 工程描述）
#
# 用法：
#   cmake -S . -B build && cmake --build build
#   （已装 Visual Studio 时：cmake -S . -B build -G "Visual Studio 17 2022"）
# 本机没有 cmake，可直接运行同目录下的 build.ps1。

cmake_minimum_required(VERSION 3.16)

project(assignment_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 源码统一 UTF-8 保存，显式告知编译器，避免中文乱码
if(MSVC)
    add_compile_options(/utf-8 /W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic
                        -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

add_executable(assignment_project
    src/main.cpp
    src/Assignment.cpp
    src/Console.cpp
)

target_include_directories(assignment_project PRIVATE include)
