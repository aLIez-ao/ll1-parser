cmake_minimum_required(VERSION 3.15)
project(LL1-Parser CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(WIN32 AND NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

add_subdirectory(lexer)
add_subdirectory(grammar)
add_subdirectory(parser)

add_executable(ll1-parser main.cpp)
target_link_libraries(ll1-parser PRIVATE lexer grammar parser)
target_include_directories(ll1-parser PRIVATE ${CMAKE_SOURCE_DIR})

set_target_properties(ll1-parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)