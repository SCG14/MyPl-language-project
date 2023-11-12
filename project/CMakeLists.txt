
project(MyPL_2023)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0") 
set(CMAKE_BUILD_TYPE Debug)

include_directories("src")
# include_directories("test")

# locate gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})




# create mypl target
add_executable(mypl src/token.cpp src/mypl_exception.cpp src/lexer.cpp
  src/simple_parser.cpp src/ast_parser.cpp src/print_visitor.cpp src/c_sharp_print_visitor.cpp
  src/symbol_table.cpp src/semantic_checker.cpp src/vm_instr.cpp
  src/vm.cpp src/var_table.cpp src/code_generator.cpp src/mypl.cpp)

