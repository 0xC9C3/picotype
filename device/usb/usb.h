void usb_step();

void usb_init();

void ascii_to_hid_keyboard(uint8_t chr, uint8_t report[8]);

void keyboard_report_enqueue(uint8_t const *report);

bool keyboard_report_queue_full();

bool keyboard_report_queue_has_entries();

void keyboard_release_all();

void keyboard_press_t();

void send_hid_keyboard_report(uint8_t report[8]);