#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

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

static void send_hid_report(uint8_t report_id, uint32_t btn) {
    // skip if hid is not ready yet
    if (!tud_hid_ready()) {
        //printf("skip hid report, not ready\n");
        return;
    }

    // printf("send hid report %u\n", report_id);

    switch (report_id) {
        case REPORT_ID_KEYBOARD: {
            // use to avoid send multiple consecutive zero report for keyboard
            static bool has_keyboard_key = false;

            if (btn) {
                uint8_t keycode[6] = {0};
                keycode[0] = HID_KEY_A;

                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
                has_keyboard_key = true;
            } else {
                // send empty key report if previously has key pressed
                if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                has_keyboard_key = false;
            }
        }
            break;

        case REPORT_ID_MOUSE: {
            int8_t const delta = 5;

            // no button, right + down, no scroll, no pan
            tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
        }
            break;

        case REPORT_ID_CONSUMER_CONTROL: {
            // use to avoid send multiple consecutive zero report
            static bool has_consumer_key = false;

            if (btn) {
                // volume down
                uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
                tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
                has_consumer_key = true;
            } else {
                // send empty key report (release key) if previously has key pressed
                uint16_t empty_key = 0;
                if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
                has_consumer_key = false;
            }
        }
            break;

        case REPORT_ID_GAMEPAD: {
            // use to avoid send multiple consecutive zero report for keyboard
            static bool has_gamepad_key = false;

            hid_gamepad_report_t report =
                    {
                            .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
                            .hat = 0, .buttons = 0
                    };

            if (btn) {
                report.hat = GAMEPAD_HAT_UP;
                report.buttons = GAMEPAD_BUTTON_A;
                tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

                has_gamepad_key = true;
            } else {
                report.hat = GAMEPAD_HAT_CENTERED;
                report.buttons = 0;
                if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
                has_gamepad_key = false;
            }
        }
            break;

        default:
            break;
    }
}

void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_KEYBOARD, btn);
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

__attribute__((noreturn)) void usb_loop() {
    while (1) {
        // tinyusb host task
        tud_task();

        // application task
        hid_task();
    }
}