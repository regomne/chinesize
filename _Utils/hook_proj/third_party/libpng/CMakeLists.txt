
set(TARGET_NAME libpng)

file(GLOB C_FILES *.c)
file(GLOB H_FILES *.h)

add_library(${TARGET_NAME} STATIC ${C_FILES} ${H_FILES})

target_link_libraries(${TARGET_NAME} PUBLIC zlib)

target_compile_definitions(${TARGET_NAME} PRIVATE _CRT_SECURE_NO_DEPRECATE)