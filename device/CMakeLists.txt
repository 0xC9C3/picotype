cmake_minimum_required(VERSION 3.13)

set(CYW43_ENABLE_BLUETOOTH 1)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(sdk/pico_sdk_init.cmake)

project(picotype)

set(PICO_BOARD pico_w)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(
        firmware
)

target_sources(
        firmware
        PUBLIC
        # bluetooth
        ${CMAKE_CURRENT_LIST_DIR}/bluetooth/service/base_service.c
        ${CMAKE_CURRENT_LIST_DIR}/bluetooth/service/typing/service.c
        ${CMAKE_CURRENT_LIST_DIR}/bluetooth/connection.c
        ${CMAKE_CURRENT_LIST_DIR}/bluetooth/bluetooth.c

        # hardware
        ${CMAKE_CURRENT_LIST_DIR}/hardware/hardware.c

        # usb
        ${CMAKE_CURRENT_LIST_DIR}/usb/usb.c
        ${CMAKE_CURRENT_LIST_DIR}/usb/usb_descriptors.c

        # entry
        ${CMAKE_CURRENT_LIST_DIR}/firmware.c
)

# add btstack_config.h
target_include_directories(
        firmware
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/config
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(
        firmware
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/usb
)


# @todo: remove this once the following issue is fixed
# currently the following symbols are redeclared by btstack and tinyusb
# HID_REPORT_TYPE_INPUT
# HID_REPORT_TYPE_OUTPUT
# HID_REPORT_TYPE_FEATURE
# hid_report_type_t
# rename them inside sdk/lib/btstack/* using sed
# find sdk/lib/btstack/src -type f -exec sed -i 's/HID_REPORT_TYPE_INPUT/HID_REPORT_TYPE_INPUT_BTSTACK/g' {} \;
# find sdk/lib/btstack/src -type f -exec sed -i 's/HID_REPORT_TYPE_OUTPUT/HID_REPORT_TYPE_OUTPUT_BTSTACK/g' {} \;
# find sdk/lib/btstack/src -type f -exec sed -i 's/HID_REPORT_TYPE_FEATURE/HID_REPORT_TYPE_FEATURE_BTSTACK/g' {} \;
# find sdk/lib/btstack/src -type f -exec sed -i 's/hid_report_type_t/hid_report_type_btstack_t/g' {} \;
# @see https://github.com/raspberrypi/pico-sdk/issues/1449

set(replace_1 "find ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/btstack/src -type f -exec sed -i 's/HID_REPORT_TYPE_INPUT/HID_REPORT_TYPE_INPUT_BTSTACK/g' {} \;")
set(replace_2 "find ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/btstack/src -type f -exec sed -i 's/HID_REPORT_TYPE_OUTPUT/HID_REPORT_TYPE_OUTPUT_BTSTACK/g' {} \;")
set(replace_3 "find ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/btstack/src -type f -exec sed -i 's/HID_REPORT_TYPE_FEATURE/HID_REPORT_TYPE_FEATURE_BTSTACK/g' {} \;")
set(replace_4 "find ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/btstack/src -type f -exec sed -i 's/hid_report_type_t/hid_report_type_btstack_t/g' {} \;")

message(STATUS "Renaming HID_REPORT_TYPE_* and hid_report_type_t in btstack")

execute_process(
        COMMAND "/bin/bash" "-c" "${replace_1}"
        COMMAND_ECHO STDOUT
)
message(STATUS "out: ${out}")
message(STATUS "res: ${res}")

execute_process(
        COMMAND "/bin/bash" "-c" "${replace_2}"
        COMMAND_ECHO STDOUT
)
message(STATUS "out: ${out}")
message(STATUS "res: ${res}")

execute_process(
        COMMAND "/bin/bash" "-c" "${replace_3}"
        COMMAND_ECHO STDOUT
)
message(STATUS "out: ${out}")
message(STATUS "res: ${res}")

execute_process(
        COMMAND "/bin/bash" "-c" "${replace_4}"
        COMMAND_ECHO STDOUT
)
message(STATUS "out: ${out}")
message(STATUS "res: ${res}")

message(STATUS "Renaming HID_REPORT_TYPE_* and hid_report_type_t in btstack done")

target_link_libraries(
        firmware
        pico_stdlib
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_none
        pico_unique_id
        tinyusb_device
        tinyusb_board
)

pico_btstack_make_gatt_header(firmware PRIVATE "${CMAKE_CURRENT_LIST_DIR}/bluetooth/picotype.gatt")
pico_add_extra_outputs(firmware)