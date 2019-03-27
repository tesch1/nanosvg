#
# CMake file for nanosvg dependencies
#

cmake_minimum_required (VERSION 3.1)
project (nanosvg_thirdparty CXX)

option (CMAKE_BUILD_TYPE "build type." "")
get_directory_property (hasParent PARENT_DIRECTORY)

# quiet warning messages
message ("${CMAKE_CONFIGURATION_TYPES} ${ANDROID}")

include (ExternalProject)

#
# if fuzzing is enabled
#
if (BUILD_FUZZ)
  #
  # Afl
  #
  ExternalProject_Add (aflX
    PREFIX aflX
    URL http://lcamtuf.coredump.cx/afl/releases/afl-2.52b.tgz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "make"
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
  ExternalProject_Get_Property (aflX source_dir)
  if (hasParent)
    set (AFL_DIR "${source_dir}" PARENT_SCOPE)
  endif (hasParent)

endif (BUILD_FUZZ)

#
# tinyxml2
#
ExternalProject_Add (tinyxml2X
  PREFIX tinyxml2X
  URL https://github.com/leethomason/tinyxml2/archive/7.0.1.tar.gz
  URL_HASH MD5=bc3c806033a2cb426db0fdb75e5b1bf2
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  )
ExternalProject_Get_Property (tinyxml2X source_dir)
set (TINYXML2_DIR "${source_dir}")
#add_library (tinyxml2 ${TINYXML2_DIR}/tinyxml2.cpp ${TINYXML2_DIR}/tinyxml2.h)

if (hasParent)
  add_library (tinyxml2 ${TINYXML2_DIR}/tinyxml2.cpp)
  add_dependencies (tinyxml2 tinyxml2X)
  set (TINYXML2_DIR "${TINYXML2_DIR}" PARENT_SCOPE)
endif (hasParent)
