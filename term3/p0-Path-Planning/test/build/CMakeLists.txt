project(Path_Planning_Test)
cmake_minimum_required (VERSION 3.5)
add_definitions(-std=c++11)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
aux_source_directory(../src/ DIR_SRCS)
add_executable(executeTests ${DIR_SRCS})
target_link_libraries(executeTests ${GTEST_LIBRARIES} pthread)