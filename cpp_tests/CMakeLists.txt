cmake_minimum_required(VERSION 3.22)
project(DTVM_CppSDK_tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(../)

ADD_SUBDIRECTORY (googletest)
enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# Unit Tests
################################
# Add test cpp file
add_executable( runUnitTests 
     ../contractlib/v1/contractlib.cpp
     test_main.cpp test_encoding.cpp
     test_math.cpp test_storage.cpp hostapi_mock.cpp)
# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests gtest gtest_main)
add_test( runUnitTests runUnitTests )