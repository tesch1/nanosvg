cmake_minimum_required(VERSION 3.0)

project(nanosvg LANGUAGES C CXX VERSION "0.0.0")
set(CMAKE_CXX_STANDARD 11)

set(NANOSVG_HEADERS "src/nanosvg.h" "src/nanosvgrast.h")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo") # "MinSizeRel"

add_library(nanosvg INTERFACE)

target_include_directories(
    nanosvg
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
)

#
# Add thirdparty dependencies
#
file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/thirdparty")

# Generator name
if (NOT "${CMAKE_EXTRA_GENERATOR}" STREQUAL "")
  set (GENERATOR_STRING "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else ()
  set (GENERATOR_STRING "${CMAKE_GENERATOR}")
endif ()

# Configure the thirdparty build directory
execute_process (
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/thirdparty"
  COMMAND ${CMAKE_COMMAND} -G${GENERATOR_STRING} -DANDROID=${ANDROID} -DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} ${CMAKE_SOURCE_DIR}/thirdparty
  RESULT_VARIABLE THIRDPARTY_CONFIG_RESULT
)
if (THIRDPARTY_CONFIG_RESULT)
  message (FATAL_ERROR "Configuring dependencies failed: ${THIRDPARTY_CONFIG_RESULT}")
endif ()

# Build the thirdparty directory
execute_process (
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/thirdparty"
  COMMAND "${CMAKE_COMMAND}" --build .
  RESULT_VARIABLE THIRDPARTY_BUILD_RESULT
  )
if (THIRDPARTY_BUILD_RESULT)
  message (FATAL_ERROR "Building dependencies failed: ${THIRDPARTY_BUILD_RESULT}")
endif ()

# Include the now-built thirdparty dependencies
add_subdirectory(thirdparty)

# Build examples
add_subdirectory(example)

# Introduce variables:
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(VERSION_CONFIG "${GENERATED_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(PROJECT_CONFIG "${GENERATED_DIR}/${PROJECT_NAME}Config.cmake")

set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(NAMESPACE "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${VERSION_CONFIG}" COMPATIBILITY ExactVersion
)

configure_package_config_file(
    "cmake/Config.cmake.in"
    "${PROJECT_CONFIG}"
    INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
)

install(
    TARGETS nanosvg
    EXPORT "${TARGETS_EXPORT_NAME}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    FILES ${NANOSVG_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
    FILES "${PROJECT_CONFIG}" "${VERSION_CONFIG}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
)

install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${NAMESPACE}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
)
