cmake_minimum_required(VERSION 2.8)

list( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules/" )

set( CMAKE_C_FLAGS "-g -Wall -Wextra" )
set( CMAKE_CXX_FLAGS "-g -Wall -Wextra -std=c++11" )

find_package( OpenCV2 REQUIRED )
find_package( HAL REQUIRED )


include_directories(
    ${OpenCV2_INCLUDE_DIR} 
    ${HAL_INCLUDE_DIRS}
    )

add_executable( main main.cpp )

target_link_libraries( main
  ${OpenCV2_LIBRARIES} 
  ${HAL_LIBRARIES}
  )

