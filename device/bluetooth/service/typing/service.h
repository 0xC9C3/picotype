#include "btstack.h"

void typing_service_server_init(btstack_packet_handler_t packet_handler);

void typing_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);