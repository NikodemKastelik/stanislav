cmake_minimum_required(VERSION 3.13.1)

list(APPEND APP_SOURCES
    src/main.c
    src/state_machine.c
)

set(STM32DRV_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

include(../mcuenv/cmake/boilerplate_app.cmake)

project(app
    LANGUAGES C ASM)
