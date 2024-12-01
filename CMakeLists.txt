cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
set(CMAKE_BUILD_TYPE Debug)


project(Wordle)
add_executable(main main.cpp)
add_executable(tester tester.cpp)