cmake_minimum_required(VERSION 3.5.1)
project(bigint)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -g")
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

#include_directories(${BIGINT_SOURCE_DIR})

add_executable(huffman_tree
        library/tree.h
        library/tree.cpp
        library/bit_sequence.h
        library/bit_sequence.cpp
        library/basic_coder.h
        library/basic_coder.cpp
        library/encoder.h
        library/encoder.cpp
        library/decoder.h
        library/decoder.cpp
#        main.cpp
#        utility/utility.cpp
        tests/gtest/gtest-all.cc
        tests/gtest/gtest.h
        tests/gtest/gtest_main.cc
        tests/testing.cpp)

#if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++11 -pedantic -g")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
#endif()

#target_link_libraries(big_integer_testing -lpthread)
