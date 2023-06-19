cmake_minimum_required(VERSION 2.6)
project(android_updater)
set(MML_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(MML_SSL 1)

set (MML_CORE ${CMAKE_CURRENT_SOURCE_DIR}/jni/mml)
set (CORE_THIRDPARTY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/jni/mml/thirdparty)
set (ANDROID_CORE ${CMAKE_CURRENT_SOURCE_DIR}/android)

include(${MML_CORE}/core/mml.cmake)

add_subdirectory(jni/mml)
add_subdirectory(android)
add_subdirectory(jni/updater)
add_subdirectory(jni/runner)

