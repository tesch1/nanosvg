
option (BUILD_FUZZ "Build binaries with tooling for fuzzing." OFF)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/example)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
add_compile_options(-O2 -Wall)

add_executable(dump dump.c)
add_executable(example1 example1.c)
#add_executable(example2 example2.c)
add_executable(example2 example2.c bridge_poke.cpp)

target_link_libraries(dump m )
target_link_libraries(example1 m GL ${GLFW_STATIC_LIBRARIES})
target_link_libraries(example2 m GL ${GLFW_STATIC_LIBRARIES} tinyxml2)

#
# if fuzzing is enabled
#
if (BUILD_FUZZ)
  message ("American Fuzzy Lop in ${AFL_DIR}")
  #set (CMAKE_C_COMPILER ${AFL_DIR}/afl-gcc)
  #set (CMAKE_CXX_COMPILER ${AFL_DIR}/afl-g++)
  set (CMAKE_C_COMPILER ${AFL_DIR}/afl-clang)
  set (CMAKE_CXX_COMPILER ${AFL_DIR}/afl-clang++)

  add_dependencies (example1 aflX)
  add_dependencies (example2 aflX)

  add_custom_target (fuzz
    COMMAND mkdir -p fuzz-data
    COMMAND cp nano.svg fuzz-data/
    COMMAND ${AFL_DIR}/afl-fuzz -i fuzz-data -o fuzz-out -x fuzz.dict $<TARGET_FILE:example2> @@ ''
    )
  add_dependencies (fuzz example1)

endif (BUILD_FUZZ)

include_directories("${TINYXML2_DIR}")
add_executable(ext_viewer ext_viewer.cpp bridge_poke.cpp ${TINYXML2_DIR}/tinyxml2.cpp)
target_link_libraries(ext_viewer m GL ${GLFW_STATIC_LIBRARIES})
