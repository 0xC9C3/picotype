#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "btstack_util.h"

// key report queue
#define KEYBOARD_REPORT_QUEUE_SZ 1024
static uint8_t keyboard_report_queue[KEYBOARD_REPORT_QUEUE_SZ][8];
static uint8_t keyboard_report_queue_head = 0;
static uint8_t keyboard_report_queue_tail = 0;

void keyboard_report_enqueue(uint8_t const *report) {
    uint8_t next_head = (keyboard_report_queue_head + 1) % KEYBOARD_REPORT_QUEUE_SZ;

    if (next_head != keyboard_report_queue_tail) {
        memcpy(keyboard_report_queue[keyboard_report_queue_head], report, 8);
        keyboard_report_queue_head = next_head;
    }
}

bool keyboard_report_queue_full() {
    return (keyboard_report_queue_head + 1) % KEYBOARD_REPORT_QUEUE_SZ == keyboard_report_queue_tail;
}

bool keyboard_report_queue_has_entries() {
    return keyboard_report_queue_head != keyboard_report_queue_tail;
}

bool keyboard_report_dequeue(uint8_t *report) {
    if (keyboard_report_queue_head == keyboard_report_queue_tail) return false;

    memcpy(report, keyboard_report_queue[keyboard_report_queue_tail], 8);
    keyboard_report_queue_tail = (keyboard_report_queue_tail + 1) % KEYBOARD_REPORT_QUEUE_SZ;

    return true;
}

void keyboard_release_all() {
    uint8_t report[8] = {0};
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, report + 2);
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
    // TODO not Implemented
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

void ascii_to_hid_keyboard(uint8_t chr, uint8_t report[8], bool swap_z_y) {
    uint8_t const conv_table[128][2] = {HID_ASCII_TO_KEYCODE};

    uint8_t keycode[6] = {0};
    uint8_t modifier = 0;

    if (conv_table[chr][0]) modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
    keycode[0] = conv_table[chr][1];

    // bandaids for swapped z and y (german keyboard)
    if (swap_z_y) {
        if (keycode[0] == HID_KEY_Z) keycode[0] = HID_KEY_Y;
        else if (keycode[0] == HID_KEY_Y) keycode[0] = HID_KEY_Z;
    }

    report[0] = REPORT_ID_KEYBOARD;
    report[1] = modifier;
    memcpy(report + 2, keycode, 6);
}

void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    if (!tud_hid_ready()) return;

    // if events are in queue wake up
    if (keyboard_report_queue_has_entries() && tud_suspended()) {
        tud_remote_wakeup();
        return;
    }

    // check if events are in queue and if yes, send next one
    uint8_t report[8];
    if (keyboard_report_dequeue(report)) {
        // print debug report
        printf("send hid report %u\n", report[0]);
        printf_hexdump(report, 8);
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report[1], report + 2);
        return;
    }
}

void tud_hid_set_report_cb(
        uint8_t instance,
        uint8_t report_id,
        hid_report_type_t report_type,
        uint8_t const *buffer,
        uint16_t bufsize
) {
    (void) instance;

    printf("set_report %u, type %u, size %u\r\n", report_id, report_type, bufsize);

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            // bufsize should be (at least) 1
            if (bufsize < 1) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
                // Capslock On: disable blink, turn led on
                //blink_interval_ms = 0;
                //board_led_write(true);
            } else {
                // Caplocks Off: back to normal blink
                //board_led_write(false);
                //blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}

void usb_init() {
    board_init();
    tusb_init();
}

void usb_step() {
    // tinyusb host task
    tud_task();

    // application task
    hid_task();
}