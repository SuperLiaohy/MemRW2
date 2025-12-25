cmake_minimum_required(VERSION 3.28)
project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DwarfCpp)
file(GLOB LIB_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
target_sources(DwarfCpp PRIVATE
        ${LIB_SOURCE}
)
target_include_directories(DwarfCpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
find_package(libdwarf CONFIG REQUIRED)
target_link_libraries(DwarfCpp PUBLIC libdwarf::dwarf)



