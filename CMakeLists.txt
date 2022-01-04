cmake_minimum_required(VERSION 3.2)

project(bitusk
    VERSION 1.0
    DESCRIPTION "c++ biterent downloader"
    LANGUAGES CXX)


add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(apps)

set(CMAKE_EXE_LINKER_FLAGS "-lpthread")


