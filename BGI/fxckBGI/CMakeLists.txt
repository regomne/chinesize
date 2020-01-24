cmake_minimum_required (VERSION 3.12)

project(fxckBGI CXX C RC)

set(TARGET_NAME hook)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(shell shell)
add_subdirectory(extractor extractor)

