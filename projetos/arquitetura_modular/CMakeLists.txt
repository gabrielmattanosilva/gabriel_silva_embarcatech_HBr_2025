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

project(arquitetura_modular C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add executable with all source files
add_executable(arquitetura_modular
    app/main.c
    drivers/led_embutido.c
    hal/hal_led.c
)

# pull in common dependencies
target_link_libraries(arquitetura_modular
    pico_stdlib
    pico_cyw43_arch_none
)

# Enable USB output (optional)
pico_enable_stdio_usb(arquitetura_modular 1)
pico_enable_stdio_uart(arquitetura_modular 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(arquitetura_modular)

# add url via pico_set_program_url (opcional)
# pico_set_program_url(arquitetura_modular "http://example.com")