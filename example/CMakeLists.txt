
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/example)

find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)
#
add_compile_options(-O2 -Wall)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

add_executable(dump dump.c)
add_executable(wireframe wireframe.c)
add_executable(writepng writepng.c)

target_link_libraries(dump m )
target_link_libraries(wireframe m ${OPENGL_LIBRARIES} glfw)
target_link_libraries(writepng m ${OPENGL_LIBRARIES} glfw)

#
# if fuzzing is enabled
#
if (BUILD_FUZZ)
  include (ExternalProject)
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

  add_custom_target (fuzz
    COMMAND mkdir -p fuzz-data
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/svgs/nano.svg fuzz-data/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/svgs/23.svg fuzz-data/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/svgs/bug167.svg fuzz-data/
    COMMAND ${AFL_DIR}/afl-fuzz -i fuzz-data -o fuzz-out -x fuzz.dict $<TARGET_FILE:writepng> @@ ''
    )
  add_dependencies (fuzz writepng)

endif (BUILD_FUZZ)
