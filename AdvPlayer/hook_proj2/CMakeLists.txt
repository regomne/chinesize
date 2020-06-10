cmake_minimum_required (VERSION 3.12)

project(hook_project CXX C)

set(TARGET_NAME hook)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(../../_Utils/hook_proj/third_party/beaengine third_party/beaengine)
add_subdirectory(../../_Utils/hook_proj/third_party/ilhook third_party/ilhook)
add_subdirectory(../../_Utils/hook_proj/third_party/libpng third_party/libpng)
add_subdirectory(../../_Utils/hook_proj/third_party/zlib third_party/zlib)

file(GLOB CPP_FILE *.cpp)
file(GLOB H_FILE *.h)

set(LINKER_FLAGS "/SAFESEH:NO" "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/exp.def")
list(JOIN LINKER_FLAGS " " CMAKE_SHARED_LINKER_FLAGS)

add_library(${TARGET_NAME} SHARED ${CPP_FILE} ${H_FILE} exp.def)
set_source_files_properties(exp.def PROPERTIES HEADER_FILE_ONLY TRUE)

target_link_libraries(${TARGET_NAME} PRIVATE
  ilhook
  libpng

  d3d9
)

target_compile_definitions(${TARGET_NAME} PRIVATE UNICODE _UNICODE)

target_compile_options(${TARGET_NAME} PRIVATE /source-charset:utf-8 /MT)

set_target_properties(BeaEngine PROPERTIES FOLDER "third_party")
set_target_properties(ilhook PROPERTIES FOLDER "third_party")
set_target_properties(libpng PROPERTIES FOLDER "third_party")
set_target_properties(zlibstatic PROPERTIES FOLDER "third_party")
