cmake_minimum_required(VERSION 3.13)
include(pico-sdk/pico_sdk_init.cmake)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(433 CXX C ASM)

set(CMAKE_CSTANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(spinner)