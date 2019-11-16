#
# CMake file for dependencies
#

cmake_minimum_required (VERSION 3.4.3)
project (nanosvg_thirdparty NONE)

include (ExternalProject)
get_directory_property (hasParent PARENT_DIRECTORY)

if (NOT WIN32)
  set (DOWNLOAD_DIR "$ENV{HOME}/Downloads")
else (NOT WIN32)
  set (DOWNLOAD_DIR "C:/Downloads")
endif (NOT WIN32)

# Provide glfw3
if (NOT glfw3)
  ExternalProject_Add (GLFWEXT
    PREFIX GLFWEXT
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON)
  ExternalProject_Get_Property (GLFWEXT source_dir)
  ExternalProject_Get_Property (GLFWEXT binary_dir)
  if (hasParent)
    # Configure glfw & add_subdirectory()
    set (GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set (GLFW_BUILD_TESTS OFF CACHE BOOL "just want glfw library" FORCE)
    set (GLFW_BUILD_EXAMPLES OFF CACHE BOOL "just want glfw library" FORCE)
    set (GLFW_CLIENT_LIBRARY opengl CACHE STRING "client to use for glfw" FORCE) # glesv1, glesv2
    set (GLFW_INSTALL OFF CACHE BOOL "just want glfw library" FORCE)
    # set (GLFW_USE_OPTIMUS_HPG ON "just want glfw library")
    add_subdirectory (${source_dir} ${binary_dir})
    add_library(glfw3 ALIAS glfw)
  endif (hasParent)
endif (NOT glfw3)
