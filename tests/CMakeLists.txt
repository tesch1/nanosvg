#
# CMake for EigenLab
#

cmake_minimum_required (VERSION 3.1)

add_test (NAME basic COMMAND writepng ${CMAKE_SOURCE_DIR}/example/svgs/23.svg)
