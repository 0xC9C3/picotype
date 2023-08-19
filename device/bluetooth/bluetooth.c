#include <stdio.h>
#include <string.h>

#include "btstack_run_loop.h"
#include "btstack_event.h"

#include "service/typing/service.h"

#include "picotype.h"
#include "connection.h"


#define BUTTON   15
#define BLUE_LED 16
#define REPORT_INTERVAL_MS 3000

#define BUTTON_BOOTSEL
#define BUTTON_STATE_ACTIVE   0

const uint8_t adv_data[] = {
        2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
        9, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'p', 'i', 'c', 'o', 't', 'y', 'p', 'e',
        17, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS, 0x9e, 0xca, 0xdc, 0x24, 0xe, 0xe5, 0xa9,
        0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x1, 0x0, 0x40, 0x6e,
};
const uint8_t adv_data_len = sizeof(adv_data);

static btstack_packet_callback_registration_t hci_event_callback_registration;

/* LISTING_END(tracking): Tracking throughput */


/*
 * @section HCI Packet Handler
 *
 * @text The packet handler prints the welcome message and requests a connection parameter update for LE Connections
 */

/* LISTING_START(packetHandler): Packet Handler */
static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    uint16_t conn_interval;
    hci_con_handle_t con_handle;

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            // BTstack activated, get started
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                printf("To start the streaming, please run nRF Toolbox -> UART to connect.\n");
            }
            break;
        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                    // print connection parameters (without using float operations)
                    con_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                    conn_interval = hci_subevent_le_connection_complete_get_conn_interval(packet);
                    printf("LE Connection - Connection Interval: %u.%02u ms\n", conn_interval * 125 / 100,
                           25 * (conn_interval & 3));
                    printf("LE Connection - Connection Latency: %u\n",
                           hci_subevent_le_connection_complete_get_conn_latency(packet));

                    // request min con interval 15 ms for iOS 11+
                    printf("LE Connection - Request 15 ms connection interval\n");
                    gap_request_connection_parameter_update(con_handle, 12, 12, 0, 0x0048);
                    break;
                case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
                    // print connection parameters (without using float operations)
                    con_handle = hci_subevent_le_connection_update_complete_get_connection_handle(packet);
                    conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(packet);
                    printf("LE Connection - Connection Param update - connection interval %u.%02u ms, latency %u\n",
                           conn_interval * 125 / 100,
                           25 * (conn_interval & 3),
                           hci_subevent_le_connection_update_complete_get_conn_latency(packet));
                    break;
                default:
                    printf("LE Meta Event 0x%02x\n", hci_event_le_meta_get_subevent_code(packet));
                    break;
            }
            break;
        default:
            break;
    }
}

/* LISTING_END */

/*
 * @section ATT Packet Handler
 *
 * @text The packet handler is used to setup and tear down the spp-over-gatt connection and its MTU
 */

/* LISTING_START(packetHandler): Packet Handler */
static void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;

    int mtu;
    picotype_le_streamer_connection_t *context;

    switch (hci_event_packet_get_type(packet)) {
        case ATT_EVENT_CONNECTED:
            // setup new
            context = connection_for_conn_handle(HCI_CON_HANDLE_INVALID);
            if (!context) break;
            context->test_data_len = ATT_DEFAULT_MTU - 4;
            context->connection_handle = att_event_connected_get_handle(packet);
            break;
        case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
            mtu = att_event_mtu_exchange_complete_get_MTU(packet) - 3;
            context = connection_for_conn_handle(att_event_mtu_exchange_complete_get_handle(packet));
            if (!context) break;
            context->test_data_len = btstack_min(mtu - 3, sizeof(context->test_data));
            printf("%c: ATT MTU = %u => use test data of len %u\n", context->name, mtu, context->test_data_len);
            break;
        case ATT_EVENT_DISCONNECTED:
            context = connection_for_conn_handle(att_event_disconnected_get_handle(packet));
            if (!context) break;
            // free connection
            printf("%c: Disconnect\n", context->name);
            context->le_notification_enabled = 0;
            context->connection_handle = HCI_CON_HANDLE_INVALID;
            break;
        default:
            break;
    }
}

/* LISTING_END */



/* LISTING_END */



void bluetooth_init() {
    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();
    sm_init();
    att_server_init(profile_data, NULL, NULL);

    // init services
    typing_service_server_init(&typing_packet_handler);


    att_server_register_packet_handler(att_packet_handler);

    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t *) adv_data);
    gap_advertisements_enable(1);
    init_connections();
    hci_power_control(HCI_POWER_ON);
}