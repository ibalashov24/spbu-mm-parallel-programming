cmake_minimum_required(VERSION 3.10)

project(ConsumerProducer)

set(CMAKE_CXX_COMPILER g++-11)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(Main main.cpp)
target_link_libraries(Main Threads::Threads)
