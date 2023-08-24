#include <stdlib.h>
#include "pico/cyw43_arch.h"

// @todo might want to rather stream data and then process it asap, in case a lot of text is sent
typedef struct {
    uint8_t type;
    uint16_t size;
    uint8_t *data;
    uint16_t cursor;
} current_packet_t;

static current_packet_t current_packet;

typedef void (*picotype_packet_handler_type)(uint8_t packet_type, uint16_t size, uint8_t *packet);

static picotype_packet_handler_type picotype_packet_handler_callback;


// if we currently have a packet copy the data into the packet buffer
// otherwise update the current packet type and size
// if we have a packet and the packet is complete call the callback
static void process_picotype_packet_data(uint8_t *data, uint16_t size) {
    if (current_packet.data) {
        printf("process_data append: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);
        memcpy(current_packet.data + current_packet.cursor, data + 2, size);
        current_packet.cursor += size;
    } else {
        printf("process_data new: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);
        current_packet.type = data[0];
        // read uint16_t size big endian
        current_packet.size = (data[1] << 8) | data[2];
        current_packet.data = malloc(current_packet.size);
        current_packet.cursor = 0;

        uint16_t bytes_to_copy = size;
        memcpy(current_packet.data, data + 2, bytes_to_copy);
        current_packet.cursor += bytes_to_copy;
    }

    if (current_packet.cursor >= current_packet.size) {
        printf("process_data done: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);

        if (picotype_packet_handler_callback) {
            printf("process_data callback: %x %x %x\n", current_packet.size, current_packet.cursor,
                   current_packet.type);
            (*picotype_packet_handler_callback)(current_packet.type, current_packet.size, current_packet.data);
        }

        current_packet.data = NULL;
        current_packet.cursor = 0;
        current_packet.size = 0;
        current_packet.type = 0;
    }

    printf("process_data exit: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);
}
