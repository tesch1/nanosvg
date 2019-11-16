
include(ExternalProject)
add_compile_options(-O2 -Wall)

# Provide glfw3
if (NOT NSVG_BUILD_GLFW)
  message(STATUS "Looking for glfw3 package...")
  find_package(GLFW3 3.3 REQUIRED)
endif ()

if (NOT "${GLFW3_FOUND}")
  set(NSVG_BUILD_GLFW ON CACHE STRING "Didn't find GLFW3, so download it.")
endif ()

# Provide OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

#################################################################################
#
# Add thirdparty dependencies
#
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty")

# Generator name
if (NOT "${CMAKE_EXTRA_GENERATOR}" STREQUAL "")
  set (GENERATOR_STRING "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else ()
  set (GENERATOR_STRING "${CMAKE_GENERATOR}")
endif ()

# Configure the thirdparty build directory
execute_process (
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty"
  COMMAND ${CMAKE_COMMAND} -G${GENERATOR_STRING} -DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DNSVG_BUILD_GLFW=${NSVG_BUILD_GLFW} -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
  RESULT_VARIABLE THIRDPARTY_CONFIG_RESULT
  OUTPUT_VARIABLE THIRDPARTY_OUTPUT
  ERROR_VARIABLE THIRDPARTY_OUTPUT
)
if (THIRDPARTY_CONFIG_RESULT)
  message (FATAL_ERROR "Configuring dependencies failed: ${THIRDPARTY_CONFIG_RESULT}, ${THIRDPARTY_OUTPUT}")
endif ()

# Build the thirdparty directory
execute_process (
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty"
  COMMAND "${CMAKE_COMMAND}" --build .
  RESULT_VARIABLE THIRDPARTY_BUILD_RESULT
  )
if (THIRDPARTY_BUILD_RESULT)
  message (FATAL_ERROR "Building dependencies failed: ${THIRDPARTY_BUILD_RESULT}")
endif ()

# Include the now-built thirdparty dependencies
add_subdirectory (thirdparty EXCLUDE_FROM_ALL)

######################################################################

#
# Example programs
#
add_executable(dump dump.c)
add_executable(wireframe wireframe.c)
add_executable(writepng writepng.c)

target_link_libraries(dump m nanosvg)
target_link_libraries(wireframe m nanosvg glfw OpenGL::GL)
target_link_libraries(writepng m nanosvg)

#
# if fuzzing is enabled
#
if (NSVG_BUILD_FUZZ)
  if (NOT WIN32)
    set (DOWNLOAD_DIR "$ENV{HOME}/Downloads")
  else (NOT WIN32)
    set (DOWNLOAD_DIR "C:/Downloads")
  endif (NOT WIN32)

  #
  # Afl
  #
  ExternalProject_Add (aflX
    PREFIX aflX
    URL http://lcamtuf.coredump.cx/afl/releases/afl-2.52b.tgz
    URL_HASH MD5=d4fa778e6c2221aee4f5326f22e1983d
    DOWNLOAD_DIR ${DOWNLOAD_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "make"
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
  ExternalProject_Get_Property (aflX source_dir)
  set (AFL_DIR "${source_dir}")
  message ("American Fuzzy Lop in ${AFL_DIR}")
  #set (CMAKE_C_COMPILER ${AFL_DIR}/afl-gcc)
  #set (CMAKE_CXX_COMPILER ${AFL_DIR}/afl-g++)
  set (CMAKE_C_COMPILER ${AFL_DIR}/afl-clang)
  set (CMAKE_CXX_COMPILER ${AFL_DIR}/afl-clang++)

  add_dependencies (wireframe aflX)
  add_dependencies (writepng aflX)
  add_dependencies (dump aflX)

  add_custom_target (fuzz
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND mkdir -p fuzz-data
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/fuzz.dict ./
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/svgs/nano.svg fuzz-data/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/svgs/23.svg fuzz-data/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/svgs/bug167.svg fuzz-data/
    COMMAND ${AFL_DIR}/afl-fuzz -i fuzz-data -o fuzz-out -x fuzz.dict $<TARGET_FILE:writepng> @@ ''
    )
  add_dependencies (fuzz writepng)

endif (NSVG_BUILD_FUZZ)
