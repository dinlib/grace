cmake_minimum_required(VERSION 3.10)

project(grace)

set(CMAKE_CXX_STANDARD 14)
add_definitions(-D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)

include_directories(include)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(GraceParser ${CMAKE_SOURCE_DIR}/Parser.yy ${CMAKE_CURRENT_BINARY_DIR}/Parser.cc DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Parser.hh)
FLEX_TARGET(GraceScanner ${CMAKE_SOURCE_DIR}/Scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/Scanner.cc)

ADD_FLEX_BISON_DEPENDENCY(GraceScanner GraceParser)

find_package(LLVM)

if( NOT LLVM_FOUND )
  message(FATAL_ERROR "LLVM package can't be found. Set CMAKE_PREFIX_PATH variable to LLVM's installation prefix.")
endif()

include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(grace 
                ${BISON_GraceParser_OUTPUTS}
                ${FLEX_GraceScanner_OUTPUTS}
          main.cc
                Driver.cc 
                Dump.cc
        Codegen.cc Context.cc include/BinOp.hh include/location.hh Error.cc include/Error.hh include/Type.hh Type.cc SymbolTable.cc include/SymbolTable.hh)

target_link_libraries(grace PUBLIC "LLVM" ${llvm_libs} )