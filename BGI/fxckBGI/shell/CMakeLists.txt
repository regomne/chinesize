
set(TARGET_NAME shell)

set(FILE_H
    framework.h
    fxckBGI.h
    resource.h
    shell.h
    targetver.h
)

set(FILE_CPP
    inject.cpp
    shellDialog.cpp
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

add_executable(${TARGET_NAME} WIN32
    ${FILE_H}
    ${FILE_CPP}
    shell.rc
)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

target_link_libraries(${TARGET_NAME} PRIVATE
    ntdll
    shlwapi
)

target_compile_definitions(${TARGET_NAME} PRIVATE UNICODE _UNICODE)

target_compile_options(${TARGET_NAME} PRIVATE /source-charset:utf-8 /MT)
