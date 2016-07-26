cmake_minimum_required(VERSION 2.8)
project(clang-example)

set(CMAKE_MODULE_PATH cmake)

FIND_PACKAGE(LLVM REQUIRED)
FIND_PACKAGE(Clang REQUIRED)


set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_executable(tool-template
  ToolTemplate.cpp
  )

target_link_libraries(tool-template
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangTooling
  )
