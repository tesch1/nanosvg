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


if (NSVG_BUILD_GLFW)
  ExternalProject_Add (GLFWEXT
    PREFIX GLFWEXT
    URL https://github.com/glfw/glfw/archive/3.3.tar.gz
    URL_HASH MD5=5be03812f5d109817e6558c3fab7bbe1
    DOWNLOAD_DIR "${DOWNLOAD_DIR}"
    #GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    #GIT_TAG "3.3"
    #GIT_SHALLOW 1
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
    add_subdirectory (${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)
  endif ()
endif ()
