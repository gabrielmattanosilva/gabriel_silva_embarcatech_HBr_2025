# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(contador_interrupcao C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(contador_interrupcao)

pico_set_program_name(contador_interrupcao "contador_interrupcao")
pico_set_program_version(contador_interrupcao "0.1")

# Add source codes to the executable.
target_sources(contador_interrupcao PRIVATE
    main.c
    inc/ssd1306_i2c.c
    inc/interrupt_counter.c
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(contador_interrupcao 0)
pico_enable_stdio_usb(contador_interrupcao 1)

# Add the standard library to the build
target_link_libraries(contador_interrupcao
    pico_stdlib
    hardware_gpio
    hardware_i2c)

# Add the standard include files to the build
target_include_directories(contador_interrupcao PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Add any user requested libraries
target_link_libraries(contador_interrupcao
    )

pico_add_extra_outputs(contador_interrupcao)
