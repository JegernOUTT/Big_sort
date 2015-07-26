project(sort)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
cmake_minimum_required(VERSION 2.8)

find_package(Threads)
set(HEADER FileAdapter.h MergeSort.h QuickSort.h stdafx.h)
set(SOURCE FileAdapter.cpp Sort.cpp)

add_executable(sort ${HEADER}  ${SOURCE} )
target_link_libraries (sort ${CMAKE_THREAD_LIBS_INIT})
