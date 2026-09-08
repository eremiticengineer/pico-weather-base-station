cmake_minimum_required(VERSION 3.13)

set(PROJECT pico-weather-base-station)

set(PICO_BOARD pico2_w CACHE STRING "Board type" FORCE)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(${PROJECT} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

message(STATUS "DEVICE=${DEVICE}")
message(STATUS "PICO_BOARD=${PICO_BOARD}")
message(STATUS "PICO_PLATFORM=${PICO_PLATFORM}")

set(FREERTOS_CONFIG_FILE_DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/include
)

set(FREERTOS_KERNEL_PATH
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel
)

include(
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake
)

add_executable(${PROJECT}
    src/pico-weather-base-station.cpp
    src/WeatherPayload/WeatherPayload.cpp
)

target_include_directories(${PROJECT} PRIVATE
    include/
    src/WeatherPayload
)

add_subdirectory(lib/pico-https)
add_subdirectory(lib/pico-lora)

target_link_libraries(${PROJECT}
    pico_stdlib
    hardware_spi
    pico_lwip
    pico_mbedtls
    pico_cyw43_arch_lwip_sys_freertos
    FreeRTOS-Kernel-Heap4
    pico_https
    pico_lora
)

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 1)

pico_add_extra_outputs(${PROJECT})
