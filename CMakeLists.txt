cmake_minimum_required(VERSION 2.8)
project(clang-example)

set(CMAKE_MODULE_PATH cmake)

FIND_PACKAGE(LLVM REQUIRED)

if(LLVM_FOUND)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
endif()
FIND_PACKAGE(Clang REQUIRED)
if(CLANG_FOUND)
include_directories(${CLANG_INCLUDE_DIRS})
endif()


set(LLVM_LINK_COMPONENTS
  Support
  )

add_executable(tool-template
  ToolTemplate.cpp
  )

target_link_libraries(tool-template
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangTooling
  )
