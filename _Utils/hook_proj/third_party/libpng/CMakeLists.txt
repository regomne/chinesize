
set(TARGET_NAME libpng)

file(GLOB C_FILES *.c)
file(GLOB H_FILES *.h)

add_library(${TARGET_NAME} STATIC ${C_FILES} ${H_FILES})

target_link_libraries(${TARGET_NAME} PUBLIC zlibstatic)

target_compile_definitions(${TARGET_NAME} PRIVATE _CRT_SECURE_NO_DEPRECATE)

target_compile_options(${TARGET_NAME} PRIVATE /MT)

target_include_directories(${TARGET_NAME} INTERFACE .)
