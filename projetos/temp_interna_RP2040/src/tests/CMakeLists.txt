cmake_minimum_required(VERSION 3.13)

# Inclui o pico_sdk_import.cmake do diretório principal
include(../pico_sdk_import.cmake)

project(test_internal_temperature)

pico_sdk_init()

# Adiciona o executável de teste
add_executable(test_internal_temperature
    test_internal_temperature.c
    ../inc/internal_temperature.c
)

# Inclui os diretórios necessários
target_include_directories(test_internal_temperature PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../inc
    ${CMAKE_CURRENT_LIST_DIR}/../lib/unity/src
)

# Linka as bibliotecas do Pico e Unity (se estiver usando)
target_link_libraries(test_internal_temperature
    pico_stdlib
    hardware_adc
    unity
)

# Habilita saída USB para os testes
pico_enable_stdio_usb(test_internal_temperature 1)  # USB habilitado
pico_enable_stdio_uart(test_internal_temperature 0)  # UART desabilitado

# Gera arquivos .uf2, .hex, etc.
pico_add_extra_outputs(test_internal_temperature)