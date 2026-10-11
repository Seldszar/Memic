cmake_minimum_required(VERSION 3.15)
project(memic CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(memic main.cpp)

target_link_libraries(memic PRIVATE winmm ole32)
