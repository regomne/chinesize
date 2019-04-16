cmake_minimum_required (VERSION 3.12)

project(exDxBin CXX C)

set(TARGET_NAME exDxBin)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#add_subdirectory(third_party/beaengine third_party/beaengine)
add_subdirectory(gflags-2.2.2 glags)
add_subdirectory(DxLib DxLib)

add_executable(${TARGET_NAME} main.cpp)

#set(LINKER_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/exp.def")
#list(JOIN LINKER_FLAGS " " CMAKE_SHARED_LINKER_FLAGS)

#add_library(${TARGET_NAME} SHARED ${CPP_FILE} ${H_FILE} exp.def)
#set_source_files_properties(exp.def PROPERTIES HEADER_FILE_ONLY TRUE)

target_link_libraries(${TARGET_NAME} PRIVATE
  gflags::gflags
  DxLib
)

target_compile_definitions(${TARGET_NAME} PRIVATE UNICODE _UNICODE)

target_compile_options(${TARGET_NAME} PRIVATE /source-charset:utf-8)

#set_target_properties(gflags::gflags PROPERTIES FOLDER "third_party")
