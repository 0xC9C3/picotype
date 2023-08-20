#include <stdio.h>
#include <string.h>

#include "btstack_run_loop.h"
#include "btstack_event.h"

#include "service/typing/service.h"

#include "picotype.h"
#include "connection.h"
#include "../hardware/hardware.h"
#include "bsp/board.h"
#include "pico/cyw43_arch.h"


#define BUTTON   15
#define BLUE_LED 16
#define REPORT_INTERVAL_MS 3000

#define BUTTON_BOOTSEL
#define BUTTON_STATE_ACTIVE   0

const uint8_t adv_data[] = {
        2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
        9, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'p', 'i', 'c', 'o', 't', 'y', 'p', 'e',
        17, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS, 0x00, 0x00, 0xFF, 0x10, 0x00, 0x00, 0x10,
        0x00,
        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};
const uint8_t adv_data_len = sizeof(adv_data);

static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_packet_callback_registration_t sm_event_callback_registration;


/*
 * @section HCI Packet Handler
 *
 * @text The packet handler prints the welcome message and requests a connection parameter update for LE Connections
 */
static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    uint16_t conn_interval;
    hci_con_handle_t con_handle;

    //printf("hci_packet_handler: packet_type %02x, event %02x\n", packet_type, hci_event_packet_get_type(packet));
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

/*
 * @section ATT Packet Handler
 *
 * @text The packet handler is used to setup and tear down the spp-over-gatt connection and its MTU
 */
static void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    printf("att_packet_handler: packet_type %02x, event %02x\n", packet_type, hci_event_packet_get_type(packet));
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

            // print connection parameters (without using float operations)
            printf("LE Connection established: ");
            printf("handle %u, ", context->connection_handle);
            bd_addr_t address;
            att_event_connected_get_address(
                    packet,
                    address
            );
            printf("address %s, \n", bd_addr_to_str(
                           address
                   )
            );

            // disable advertisements
            gap_advertisements_enable(0);

            // request pairing
            sm_send_security_request(context->connection_handle);
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

            // begin advertising again
            gap_advertisements_enable(1);
            break;
        default:
            printf("att_packet_handler: event not handled %02x\n", hci_event_packet_get_type(packet));
            break;
    }
}

static void sm_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    switch (packet_type) {

        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) {
                case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
                case SM_EVENT_JUST_WORKS_REQUEST:
                    // if already bonded decline request
                    printf("devices: %d\n", le_device_db_count());
                    if (le_device_db_count() >= MAX_NR_BONDS) {
                        printf("Max number of bonded devices reached %d\n", le_device_db_count());
                        sm_bonding_decline(sm_event_just_works_request_get_handle(packet));
                        gap_disconnect(sm_event_just_works_request_get_handle(packet));
                        break;
                    }

                    // activate led to indicate pairing and wait for confirmation via button for 30 seconds
                    uint32_t start_time = board_millis();
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
                    while (board_millis() - start_time < 30000) {
                        if (bootsel_board_button_read()) {
                            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
                            printf("Just Works Confirmed\n");
                            break;
                        }
                    }

                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
                    break;
                case SM_EVENT_PAIRING_COMPLETE:
                    // print bonding information
                    printf("Pairing Complete, status %02x\n", sm_event_pairing_complete_get_status(packet));
                    break;

                default:
                    printf("sm_event_handler: event not handled %02x\n", hci_event_packet_get_type(packet));
                    break;
            }

        default:
            printf("sm_event_handler: packet_type %02x, event %02x\n", packet_type, hci_event_packet_get_type(packet));
            break;
    }
}


void clear_bonding_info() {
    for (int i = 0; i < le_device_db_max_count(); i++) {
        le_device_db_remove(i);
    }
}

uint32_t button_down_time = 0;

void bluetooth_step() {
    // if button is pressed for 5 seconds, clear bonding info
    if (bootsel_board_button_read() && button_down_time == 0) {
        button_down_time = board_millis();
    }

    if (!bootsel_board_button_read()) {
        button_down_time = 0;
    }

    if (button_down_time > 0 && board_millis() - button_down_time > 5000) {
        printf("Clearing bonding info\n");
        clear_bonding_info();

        // blink three times to indicate that bonding info has been cleared
        for (int i = 0; i < 3; i++) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
            sleep_ms(100);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
            sleep_ms(100);
        }
    }
}

void bluetooth_init() {
    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    sm_init();

    // @todo sm ir / er? https://bluekitchen-gmbh.com/btstack/v1.0/protocols/ smp

    // require protection
    sm_set_authentication_requirements(
            SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING | SM_AUTHREQ_KEYPRESS
    );

    // set capability button
    sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);


    att_server_init(profile_data, NULL, NULL);

    // init services
    typing_service_server_init(&typing_packet_handler);

    sm_event_callback_registration.callback = &sm_event_handler;
    sm_add_event_handler(&sm_event_callback_registration);


    att_server_register_packet_handler(att_packet_handler);

    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);

    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t *) adv_data);

    // initially begin sending advertisements
    gap_advertisements_enable(1);

    init_connections();
    hci_power_control(HCI_POWER_ON);
}