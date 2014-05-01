
add_library( apriltags
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

# Make possible calling find_package( apriltags 1.1 )
#list( APPEND EXPORT_INCLUDES /opt/local/include /usr/include )
list( APPEND EXPORT_LIBRARIES pthread m )
#list( APPEND EXPORT_LIBRARY_DIRS /opt/lib )

export_library( apriltags 1 2 )

