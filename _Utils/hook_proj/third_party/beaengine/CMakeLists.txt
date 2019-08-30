project (BeaEngine C)

set(TARGET_NAME BeaEngine)

add_library(${TARGET_NAME} STATIC src/BeaEngine.c)

target_include_directories(${TARGET_NAME} PUBLIC include)

target_compile_options(${TARGET_NAME} PRIVATE /MT)