#include "base.h"
#include "../../../packet/packet.h"

#include "pico/multicore.h"
#include "usb.h"
#include "device/usbd.h"
#include "btstack_util.h"

void srv_typing_packet_handler(uint8_t packet_type, uint16_t size, uint8_t *packet) {
    printf("srv_typing_packet_handler: %x %x\n", packet_type, size);
    printf_hexdump(packet, size);

    // packet contains utf-16 characters which needs to be converted to hid keycodes
    uint8_t report[8] = {0};
    // iterate utf-16 characters
    for (int i = 0; i < size; i += 2) {
        // convert utf-16 to utf-8 big-endian
        printf("utf16: %x %x\n", packet[i], packet[i + 1]);
        uint16_t chr = packet[i + 1] | (packet[i] << 8);

        // convert utf-8 to hid keycode
        printf("utf8: %x\n", chr);
        ascii_to_hid_keyboard(chr, report);

        keyboard_report_enqueue(report);

        // queue empty report to release all keys
        uint8_t empty_report[8] = {0};
        keyboard_report_enqueue(empty_report);
    }
    printf("srv_typing_packet_handler done\n");
}

void srv_typing_register_packet_handler() {
    register_packet_handler(PACKET_TYPE_SRV_TYPING_TYPE, srv_typing_packet_handler);
}