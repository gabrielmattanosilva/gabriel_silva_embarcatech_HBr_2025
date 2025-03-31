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
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o pico_sdk a partir da localização instalada
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

project(seguranca_senior C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Criação do executável principal
add_executable(seguranca_senior)

# Adiciona os arquivos de código-fonte ao executável
target_sources(seguranca_senior PRIVATE
    main.c
    inc/button_handler.c
    inc/buzzer_led.c
    inc/callmebot_whatsapp.c
    inc/display_oled.c
    inc/ssd1306_i2c.c
    inc/wifi.c
)

# Define o nome e a versão do programa
pico_set_program_name(seguranca_senior "seguranca_senior")
pico_set_program_version(seguranca_senior "0.1")

# Configuração de saída via UART/USB
pico_enable_stdio_uart(seguranca_senior 0)
pico_enable_stdio_usb(seguranca_senior 1)

# Vincula as bibliotecas necessárias
target_link_libraries(seguranca_senior
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_i2c
    hardware_pwm
    hardware_clocks
)

# Adiciona o diretório de cabeçalhos
target_include_directories(seguranca_senior PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Gera arquivos extras de saída 
pico_add_extra_outputs(seguranca_senior)