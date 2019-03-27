
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

include_directories("${TINYXML2_DIR}")
add_executable(ext_viewer ext_viewer.cpp bridge_poke.cpp ${TINYXML2_DIR}/tinyxml2.cpp)
target_link_libraries(ext_viewer m GL ${GLFW_STATIC_LIBRARIES})
