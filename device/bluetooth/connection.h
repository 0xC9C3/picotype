#include "btstack.h"
#include "../config/picotype_bluetooth.h"

typedef struct {
    char name;
    int le_notification_enabled;
    hci_con_handle_t connection_handle;
    char test_data[200];
    int test_data_len;
    uint32_t test_data_sent;
    uint32_t test_data_start;
    btstack_context_callback_registration_t send_request;
} picotype_le_streamer_connection_t;


// round robin sending
static int connection_index;
static picotype_le_streamer_connection_t picotype_le_streamer_connections[MAX_NR_CONNECTIONS];


void init_connections(void);

void next_connection_index(void);

picotype_le_streamer_connection_t *connection_for_conn_handle(hci_con_handle_t conn_handle);
