# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_modbus_rtu_client)

target_include_directories(app PRIVATE src/libmodbus)
target_sources(app PRIVATE src/libmodbus/modbus.c)
# target_sources(app PRIVATE src/libmodbus/errno.c)
target_sources(app PRIVATE src/libmodbus/modbus-data.c)
target_sources(app PRIVATE src/libmodbus/modbus-st-rtu.c)
target_sources(app PRIVATE src/libmodbus/uart_device.c)

target_sources(app PRIVATE src/main.c)
# target_sources(app PRIVATE src/libmodbus/)


