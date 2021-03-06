cmake_minimum_required(VERSION 3.0)

project(nanosvg LANGUAGES C VERSION "0.0.0")

option (NSVG_BUILD_EXAMPLES "Build example programs." OFF)
option (NSVG_BUILD_TEST "Build testing suite." OFF)
option (NSVG_BUILD_FUZZ "Build binaries with tooling for fuzzing using afl." OFF)
option (NSVG_BUILD_GLFW "Download and build our own glfw" OFF)

# Default CMAKE_BUILD_TYPE, if not Xcode or VS
set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Set cmake build type")
set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set (CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

# Testing and Fuzzing depends on Examples
if (NSVG_BUILD_TEST)
  set (NSVG_BUILD_EXAMPLES ON CACHE STRING "Testing requires Examples" FORCE)
endif ()
if (NSVG_BUILD_FUZZ)
  set (NSVG_BUILD_EXAMPLES ON CACHE STRING "Fuzzing requires Examples" FORCE)
endif ()

# The header "library"
set(NANOSVG_HEADERS "src/nanosvg.h" "src/nanosvgrast.h")
add_library(nanosvg INTERFACE)
target_compile_features(nanosvg INTERFACE c_std_11)

target_include_directories(
    nanosvg
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
)

# Build examples
if (NSVG_BUILD_EXAMPLES)
  add_subdirectory(example)
endif ()

if (NSVG_BUILD_TEST)
  enable_testing()
  add_subdirectory(tests)
endif ()

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
