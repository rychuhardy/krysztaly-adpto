cmake_minimum_required(VERSION 3.2)
project(AdPTO)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror")

set(SOURCE_FILES main.cpp)
add_executable(AdPTO ${SOURCE_FILES})