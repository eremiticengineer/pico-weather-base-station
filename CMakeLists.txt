cmake_minimum_required(VERSION 3.13)

set(PROJECT pico-weather-base-station)

# Application configuration files.
# These were copied from the pico_https standalone example
# and are now owned by this application:
#
# include/
#   FreeRTOSConfig.h
#   lwipopts.h
#   mbedtls_config.h

# ---------------------------------------------------------
# Board
# ---------------------------------------------------------
if(DEVICE STREQUAL "pico_w")
    set(PICO_BOARD pico_w CACHE STRING "Board type" FORCE)
elseif(DEVICE STREQUAL "pico2_w")
    set(PICO_BOARD pico2_w CACHE STRING "Board type" FORCE)
else()
    message(FATAL_ERROR
        "Unknown DEVICE='${DEVICE}'. Use pico_w or pico2_w."
    )
endif()

# ---------------------------------------------------------
# Application configuration
# ---------------------------------------------------------
set(PICO_MBEDTLS_CONFIG_FILE
    "${CMAKE_CURRENT_LIST_DIR}/include/mbedtls_config.h"
)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# ---------------------------------------------------------
# Pico SDK
# ---------------------------------------------------------
project(${PROJECT} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

message(STATUS "DEVICE=${DEVICE}")
message(STATUS "PICO_BOARD=${PICO_BOARD}")
message(STATUS "PICO_PLATFORM=${PICO_PLATFORM}")

# ---------------------------------------------------------
# FreeRTOS
# ---------------------------------------------------------
set(FREERTOS_CONFIG_FILE_DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/include
)

set(FREERTOS_KERNEL_PATH
    ${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel
)

include(
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake
)

# ---------------------------------------------------------
# Application
# ---------------------------------------------------------
add_executable(${PROJECT}
    src/pico-weather-base-station.cpp
    src/tasks/wifi-tasks.cpp
    src/tasks/lora-tasks.cpp
    src/tasks/https-tasks.cpp
)

target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    ${CMAKE_CURRENT_LIST_DIR}/src/tasks
)

add_subdirectory(lib/pico-wifi)
add_subdirectory(lib/pico-https)
add_subdirectory(lib/pico-lora)

# Link a dependency directly from the application
# only if the application’s own source files use it.
# PRIVATE because there is no downstream target
# consuming the executable’s usage requirements.
target_link_libraries(${PROJECT} PRIVATE
    pico_stdlib
    hardware_spi
    FreeRTOS-Kernel-Heap4
    pico_wifi
    pico_https
    pico_lora
)

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 1)

pico_add_extra_outputs(${PROJECT})
