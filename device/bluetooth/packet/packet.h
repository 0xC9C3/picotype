#include <stdlib.h>
#include "pico/cyw43_arch.h"

typedef struct {
    uint8_t type;
    uint16_t size;
    uint8_t *data;
    uint16_t cursor;
} current_packet_t;

enum PACKET_TYPE {
    PACKET_TYPE_SRV_TYPING_TYPE = 0x02
};


typedef void (*picotype_packet_handler_type)(uint8_t packet_type, uint16_t size, uint8_t *packet);

// make it possible to register a callback for incoming packets by type
typedef struct {
    picotype_packet_handler_type callback;
} picotype_packet_handler_t;


void register_packet_handler(uint8_t packet_type, picotype_packet_handler_type callback);

void process_picotype_packet_data(uint8_t *data, uint16_t size);
