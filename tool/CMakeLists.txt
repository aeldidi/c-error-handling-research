cmake_minimum_required(VERSION 3.20)
project(analyze-error-handling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

add_executable(analyze-error-handling main.cpp sqlite3.c)
execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-resource-dir
    OUTPUT_VARIABLE CLANG_RESOURCE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_compile_definitions(analyze-error-handling
    PRIVATE
    CLANG_RESOURCE_DIR="${CLANG_RESOURCE_DIR}"
)

target_link_libraries(analyze-error-handling
    PRIVATE
    clangTooling
    clangFrontend
    clangAST
    clangBasic
    clangRewrite
    LLVM
)

# TODO: write tests
