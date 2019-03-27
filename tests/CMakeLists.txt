
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/example)

#
# Basic parse/render tests:
#  parse and render an svg, then compare the image to the expected output
#

# download reference svg files
#execute_process (
#  COMMAND curl https://codeload.github.com/web-platform-tests/wpt/tar.gz/master | tar -xz --strip=1 wpt-master/svg
#  RESULT_VARIABLE DL_SVG_RESULT
#  )
#add_custom_target (
#  OUTPUT svg
#  COMMAND curl https://codeload.github.com/web-platform-tests/wpt/tar.gz/master | tar -xz --strip=1 wpt-master/svg
#  )

add_test (reftests ${CMAKE_CURRENT_SOURCE_DIR}/reftests.sh)

# cmake 3.10 adds support for "GoogleTest" which enumerates the tests
# inside of the code and adds them to ctest.
if (CMAKE_VERSION VERSION_LESS 3.10)
  message ("no gtest-based tests because cmake is too old")
else (CMAKE_VERSION VERSION_LESS 3.10)
  # Configure google-test as a downloadable library.
  include (External_GTest.cmake)

  # Configure google-test as a downloadable library.
  include (GoogleTest)

  foreach (TEST_FILE svg2x.c)
    string (REGEX REPLACE "\\.[^.]*$" "" TEST ${TEST_FILE})
    # test binaries
    add_executable (${TEST} ${TEST_FILE})
    target_link_libraries (${TEST} ${GTEST_LIBRARY})
    target_link_libraries (${TEST} m GL extxml)
    # discover individual tests
    gtest_discover_tests (${TEST} TEST_LIST ${TEST}_targets
      PROPERTIES TIMEOUT 2
      ENVIRONMENT "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
      )
  endforeach (TEST_FILE)

endif (CMAKE_VERSION VERSION_LESS 3.10)

#
# if fuzzing is enabled, fuzz testing
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

