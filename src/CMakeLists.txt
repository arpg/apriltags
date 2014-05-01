project( apriltag )
cmake_minimum_required(VERSION 2.8)
include(def_library)
include(export_package)

add_definitions(-msse4.2 -pthread  -g -std=gnu99 -Wall -Wno-unused-parameter -Wno-format-zero-length -O3)

set(APRIL_SRCS
  apriltag.c
  graymodel.c 
  image_f32.c 
  image_u8.c
  segment2.c 
  tag36h11.c 
  unionfind.c
  zarray.c 
  g2d.c 
  homography.c
  image_u32.c 
  matd.c
  tag36h10.c 
  workerpool.c
  zhash.c
  )

def_library(apriltags
  SOURCES ${APRIL_SRCS}
  LINK_LIBS m pthread
  PACKAGE apriltags
  )

export_package(apriltags
  TARGETS apriltags
  VERSION 0.92
  LIBRARY apriltags
  LIBRARIES apriltags
  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ) 