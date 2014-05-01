cmake_minimum_required(VERSION 2.8)
include( "export.cmake" )
set( CMAKE_C_FLAGS "-Wall -Wextra" )
include_directories( "${CMAKE_SOURCE_DIR}/include" )
add_subdirectory( src )


