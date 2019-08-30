
set(TARGET_NAME ilhook)

add_library(${TARGET_NAME} STATIC asmhelper.cpp ilhook.cpp ilhook.h)

target_include_directories(${TARGET_NAME} INTERFACE .)

target_link_libraries(${TARGET_NAME} PUBLIC BeaEngine)

target_compile_options(${TARGET_NAME} PRIVATE /MT)