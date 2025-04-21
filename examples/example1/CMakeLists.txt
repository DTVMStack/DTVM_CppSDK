cmake_minimum_required(VERSION 3.22)
project(DTVM_CppSDK_demo)

set(CMAKE_CXX_STANDARD 17)

include_directories(../..)

add_executable(DTVM_CppSDK_demo
        ../../contractlib/v1/contractlib.cpp
        my_token.cpp)
