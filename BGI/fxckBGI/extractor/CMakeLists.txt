
set(TARGET_NAME extractor)

set(FILE_H
    framework.h
    resource.h
    extractor.h
    misc.h
	funcHelper.h
)

set(FILE_CPP
    dllmain.cpp
    ext.cpp
    misc.cpp
	funcHelper.cpp
	../shell/inject.cpp
)

add_subdirectory(../third_party/beaengine third_party/beaengine)
add_subdirectory(../third_party/ilhook third_party/ilhook)
add_subdirectory(../third_party/libpng third_party/libpng)
add_subdirectory(../third_party/zlib third_party/zlib)

add_library(${TARGET_NAME} SHARED
    ${FILE_H}
    ${FILE_CPP}
    extractor.rc
)

target_link_libraries(${TARGET_NAME} PRIVATE
  ilhook
  libpng
  zlib
  d3d9
  ntdll
)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

target_compile_definitions(${TARGET_NAME} PRIVATE UNICODE _UNICODE)

target_compile_options(${TARGET_NAME} PRIVATE /source-charset:utf-8 /MT)

set_target_properties(BeaEngine PROPERTIES FOLDER "third_party")
set_target_properties(ilhook PROPERTIES FOLDER "third_party")
set_target_properties(libpng PROPERTIES FOLDER "third_party")
set_target_properties(zlib PROPERTIES FOLDER "third_party")

