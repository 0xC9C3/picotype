

#include "btstack_run_loop.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"


#include "usb/usb.h"
#include "bluetooth/bluetooth.h"


#define BUTTON   15
#define BLUE_LED 16
#define REPORT_INTERVAL_MS 3000
#define MAX_NR_CONNECTIONS 3


int main() {

    gpio_init(BUTTON);
    gpio_init(BLUE_LED);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);
    gpio_set_dir(BLUE_LED, GPIO_OUT);
    gpio_put(BLUE_LED, 0);
    stdio_init_all();

    if (cyw43_arch_init()) return -1;

    usb_init();

    bluetooth_init();

    printf("Begin\n");

    while (1) {
        usb_step();
        bluetooth_step();
    }

    //btstack_run_loop_execute();

    return 0;
}