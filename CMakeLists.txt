cmake_minimum_required(VERSION 3.18)
project(bftmp)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(bftmp main.cpp BFMachine.h BFExpression.h BFParser.h)
target_compile_options(bftmp PRIVATE -Werror -Wall -Wextra)
