cmake_minimum_required(VERSION 3.16)
project(test-task)
include(${CMAKE_CURRENT_LIST_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


add_subdirectory(server)
add_subdirectory(client)
