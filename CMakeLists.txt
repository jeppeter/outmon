cmake_minimum_required(VERSION 2.8)

project(outmon)

include(cmake/functions.cmake)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

CheckMSVC()
add_definitions(-DUNICODE -D_UNICODE)
add_subdirectory(outlib)
add_subdirectory(outmon)
