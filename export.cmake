
include( CMakePackageConfigHelpers )

macro( export_library LIBRARY_NAME VERSION_MAJOR VERSION_MINOR )

  set( VERSION ${VERSION_MAJOR}.${VERSION_MINOR} )
  message( STATUS "Exporting lib${LIBRARY_NAME} v${VERSION}" )
  message( STATUS "   with include directores: '${EXPORT_INCLUDES}'" )
  message( STATUS "   library dependencies: '${EXPORT_LIBRARIES}'" )
  message( STATUS "   and library locations: '${EXPORT_LIBRARY_DIRS}'" )

  string(TOLOWER ${LIBRARY_NAME} PKGNAME )
  message( STATUS "Creating cmake config files: '${PKGNAME}Config.cmake' etc." )
  message( STATUS "Use find_package( ${PKGNAME} ${VERSION_MAJOR} ${VERSION_MINOR} ) to import lib${LIBRARY_NAME}.\n\n" )

  # Export library for easy inclusion from other cmake projects.
  export( TARGETS ${LIBRARY_NAME}
      APPEND FILE ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}Targets.cmake )

  # Version information
  write_basic_package_version_file( ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}ConfigVersion.cmake
      VERSION ${VERSION} COMPATIBILITY AnyNewerVersion )

  list( APPEND ${PKGNAME}_LIBRARIES ${LIBRARY_NAME} )
  list( APPEND ${PKGNAME}_LIBRARIES ${EXPORT_LIBRARIES} )
  list( APPEND ${PKGNAME}_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include )
  list( APPEND ${PKGNAME}_INCLUDE_DIRS ${EXPORT_INCLUDES} )

  message( "got ${${PKGNAME}_LIBRARIES}") 

  SET( LIBRARY_NAME ${LIBRARY_NAME} CACHE INTERNAL "lib${LIBRARY_NAME}" FORCE )
  set( CfgTxt 
    "# Compute paths\n"
    "get_filename_component( ${LIBRARY_NAME}_CMAKE_DIR \"\${CMAKE_CURRENT_LIST_FILE}\" PATH )\n"
    "SET( ${LIBRARY_NAME}_INCLUDE_DIRS \"@${PKGNAME}_INCLUDE_DIRS@\" )\n"
    "\n"
    "# Library dependencies (contains definitions for IMPORTED targets)\n"
    "if( NOT TARGET @LIBRARY_NAME@ AND NOT ${LIBRARY_NAME}_BINARY_DIR )\n"
    "  include( \"\${${LIBRARY_NAME}_CMAKE_DIR}/${LIBRARY_NAME}Targets.cmake\" )\n"
    "endif()\n"
    "\n"
    "SET( ${LIBRARY_NAME}_LIBRARIES    \"@LIBRARY_NAME@\" )\n"
    "SET( ${LIBRARY_NAME}_LIBRARY      \"@LIBRARY_NAME@\" )\n"
  )
 
  # write the *Config.cmake.in file once 
  FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}Config.cmake.in ${CfgTxt} )

  #######################################################
  # Build tree config
  CONFIGURE_FILE( ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}Config.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}Config.cmake @ONLY IMMEDIATE )

  # Install tree config
  configure_file( ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}Config.cmake.in
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PKGNAME}Config.cmake"
      @ONLY )

  # Add package to CMake package registery for use from the build tree
  option( EXPORT_${PKGNAME} "Should lib${LIBRARY_NAME} be exported?" ON )
  if( EXPORT_${PKGNAME} )
    export( PACKAGE ${PKGNAME} )
  endif()

  #######################################################
  ## Install headers / targets
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.h"
      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${LIBRARY_NAME}
      )
  install(FILES ${HEADERS}
      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${LIBRARY_NAME}
      )
  install(TARGETS ${LIBRARY_NAME}
      EXPORT "${LIBRARY_NAME}Targets"
      RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
      LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
      ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
      )

  #######################################################
  ## Install CMake config
  INSTALL(
      FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PKGNAME}Config.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}ConfigVersion.cmake"
      DESTINATION lib/cmake/${LIBRARY_NAME}  )
  install( EXPORT "${LIBRARY_NAME}Targets" DESTINATION lib/cmake/${LIBRARY_NAME}  )

  # cleanup
  file( REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}ConfigVersion.cmake.in )
  file( REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}Config.cmake.in )

endmacro()

