#include "connection.h"

#include <stdio.h>

#include "btstack_run_loop.h"


void init_connections(void) {
    // track connections
    for (int i = 0; i < MAX_NR_CONNECTIONS; i++) {
        picotype_le_streamer_connections[i].connection_handle = HCI_CON_HANDLE_INVALID;
        picotype_le_streamer_connections[i].name = 'A' + i;
    }
}

picotype_le_streamer_connection_t *connection_for_conn_handle(hci_con_handle_t conn_handle) {
    for (int i = 0; i < MAX_NR_CONNECTIONS; i++) {
        if (picotype_le_streamer_connections[i].connection_handle == conn_handle)
            return &picotype_le_streamer_connections[i];
    }
    return NULL;
}

void next_connection_index(void) {
    connection_index++;
    if (connection_index == MAX_NR_CONNECTIONS) {
        connection_index = 0;
    }
}