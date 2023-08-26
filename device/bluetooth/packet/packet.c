#include "packet.h"
#include "btstack_util.h"


picotype_packet_handler_t picotype_packet_handlers[256];

current_packet_t current_packet;

// @todo rather stream data and then process it asap
// if we currently have a packet copy the data into the packet buffer
// otherwise update the current packet type and size
// if we have a packet and the packet is complete call the callback
void process_picotype_packet_data(uint8_t *data, uint16_t size) {
    if (current_packet.data) {
        printf("process_data append: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);
        memcpy(current_packet.data + current_packet.cursor, data + 3, size);
        current_packet.cursor += size;
    } else {
        current_packet.type = data[0];
        // read uint16_t size big endian
        printf("process_data size: %x, %x, %x\n", size, data[1], data[2]);
        current_packet.size = (data[1] << 8) | data[2];
        current_packet.data = malloc(current_packet.size);
        current_packet.cursor = 0;

        uint16_t bytes_to_copy = size;
        memcpy(current_packet.data, data + 3, bytes_to_copy);
        current_packet.cursor += bytes_to_copy;

        printf("process_data new: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);
    }

    if (current_packet.cursor >= current_packet.size) {
        printf("process_data done: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);

        picotype_packet_handler_type cb = picotype_packet_handlers[current_packet.type].callback;
        if (cb) {
            printf("process_data callback: %x %x %x\n", current_packet.size, current_packet.cursor,
                   current_packet.type);
            (*cb)(current_packet.type, current_packet.size, current_packet.data);
        }

        free(current_packet.data);
        current_packet.data = NULL;
        current_packet.cursor = 0;
        current_packet.size = 0;
        current_packet.type = 0;
    }

    printf("process_data exit: %x %x %x\n", current_packet.size, current_packet.cursor, current_packet.type);
}

void register_packet_handler(uint8_t packet_type, picotype_packet_handler_type callback) {
    picotype_packet_handlers[packet_type].callback = callback;
}