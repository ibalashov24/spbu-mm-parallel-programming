project(spbu-mm-parallel-programming LANGUAGES CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(1_synchronization)
add_subdirectory(2_thread_pool)