cmake_minimum_required (VERSION 3.12)

project(DxLib CXX)

set(TARGET_NAME DxLib)

file(GLOB UNIVERSAL_CPP *.cpp)
file(GLOB UNIVERSAL_H *.h)
file(GLOB WIN_CPP Windows/*.cpp)
file(GLOB WIN_H Windows/*.h)

add_library(${TARGET_NAME} STATIC
  ${UNIVERSAL_CPP}
  ${UNIVERSAL_H}
  ${WIN_CPP}
  ${WIN_H}
)

target_compile_options(${TARGET_NAME} PRIVATE /source-charset:utf-8)

target_include_directories(${TARGET_NAME} INTERFACE
  .
)
