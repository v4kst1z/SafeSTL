cmake_minimum_required(VERSION 3.10.2)
project(SafeSTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(queue example/queue.cpp)
add_executable(stack example/stack.cpp)
add_executable(queuefg example/queue-fg.cpp)
add_executable(list example/list.cpp)
add_executable(map example/map.cpp)