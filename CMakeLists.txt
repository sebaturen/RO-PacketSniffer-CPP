cmake_minimum_required(VERSION 3.12)

include(Config.cmake)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
include(C:/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(ro-packetsnifer)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/external)

add_subdirectory(src)