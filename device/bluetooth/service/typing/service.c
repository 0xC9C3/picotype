#include <stdio.h>
#include <string.h>

#include "btstack_run_loop.h"
#include "pico/stdlib.h"
#include "btstack_event.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"

#include "hardware/sync.h"

#include "../../connection.h"
#include "../../../hardware/hardware.h"


#define BUTTON   15
#define BLUE_LED 16
#define REPORT_INTERVAL_MS 3000

#define BUTTON_BOOTSEL
#define BUTTON_STATE_ACTIVE   0


const uint8_t primary_service_uuid[] = {
        0x00, 0x00, 0xFF, 0x10, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};
const uint8_t rx_type_characteristic_uuid[] = {
        0x00, 0x00, 0xFF, 0x12, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};

const uint8_t tx_type_characteristic_uuid[] = {
        0x00, 0x00, 0xFF, 0x12, 0x00, 0x00, 0x10, 0x00,
        0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};

static att_service_handler_t picotype_service_handler;
static uint16_t picotype_rx_value_handle;
static uint16_t picotype_tx_value_handle;
static uint16_t picotype_tx_client_configuration_handle;
static uint16_t picotype_tx_client_configuration_value;
static btstack_packet_handler_t client_packet_handler;

static char button_pressed[18] = "pressed ";
static char button_released[18] = "released";
static int toggle = true;


/*
 * @section Track throughput
 * @text We calculate the throughput by setting a start time and measuring the amount of
 * data sent. After a configurable REPORT_INTERVAL_MS, we print the throughput in kB/s
 * and reset the counter and start time.
 */
static void test_reset(picotype_le_streamer_connection_t *context) {
    context->test_data_start = btstack_run_loop_get_time_ms();
    context->test_data_sent = 0;
}

static void test_track_sent(picotype_le_streamer_connection_t *context, int bytes_sent) {
    context->test_data_sent += bytes_sent;
    // evaluate
    uint32_t now = btstack_run_loop_get_time_ms();
    uint32_t time_passed = now - context->test_data_start;
    if (time_passed < REPORT_INTERVAL_MS) return;
    // print speed
    int bytes_per_second = context->test_data_sent * 1000 / time_passed;
    // printf("%c: %"PRIu32" bytes sent-> %u.%03u kB/s\n", context->name, context->test_data_sent, bytes_per_second / 1000, bytes_per_second % 1000);

    // restart
    context->test_data_start = now;
    context->test_data_sent = 0;
}


int service_server_send(hci_con_handle_t con_handle, const uint8_t *data, uint16_t size) {
    return att_server_notify(con_handle, picotype_tx_value_handle, data, size);
}


void service_server_request_can_send_now(btstack_context_callback_registration_t *request,
                                         hci_con_handle_t con_handle) {
    att_server_request_to_send_notification(request, con_handle);
}

/*
 * @section Streamer
 *
 * @text The streamer function checks if notifications are enabled and if a notification can be sent now.
 * It creates some test data - a single letter that gets increased every time - and tracks the data sent.
 */
static void can_send(void *some_context) {
    UNUSED(some_context);

    // find next active streaming connection
    int old_connection_index = connection_index;
    while (1) {
        // active found?
        if ((picotype_le_streamer_connections[connection_index].connection_handle != HCI_CON_HANDLE_INVALID) &&
            (picotype_le_streamer_connections[connection_index].le_notification_enabled))
            break;

        // check next
        next_connection_index();

        // none found
        if (connection_index == old_connection_index) return;
    }

    picotype_le_streamer_connection_t *context = &picotype_le_streamer_connections[connection_index];

    // create test data
    //context->counter++;
    //if (context->counter > 'Z') context->counter = 'A';
    //memset(context->test_data, context->counter, context->test_data_len);

    /**********************************************************************************************/
    /****************** put your own function here to send notification to android app ************/
    /**********************************************************************************************/
    if (!gpio_get(BUTTON)) memcpy(context->test_data, button_pressed, strlen(button_pressed));
    else memcpy(context->test_data, button_released, strlen(button_released));


    // send
    service_server_send(
            context->connection_handle,
            (uint8_t *) context->test_data,
            context->test_data_len
    );

    // track
    test_track_sent(context, context->test_data_len);

    // request next send event
    service_server_request_can_send_now(&context->send_request, context->connection_handle);

    // check next
    next_connection_index();
}

void typing_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    hci_con_handle_t con_handle;
    picotype_le_streamer_connection_t *context;
    switch (packet_type) {
        case HCI_EVENT_PACKET:
            if (hci_event_packet_get_type(packet) != HCI_EVENT_GATTSERVICE_META) break;
            switch (hci_event_gattservice_meta_get_subevent_code(packet)) {
                case GATTSERVICE_SUBEVENT_SPP_SERVICE_CONNECTED:
                    con_handle = gattservice_subevent_spp_service_connected_get_con_handle(packet);
                    // get authentication level
                    context = connection_for_conn_handle(con_handle);
                    if (!context) break;
                    context->le_notification_enabled = 1;
                    test_reset(context);
                    context->send_request.callback = &can_send;
                    service_server_request_can_send_now(&context->send_request,
                                                        context->connection_handle);
                    break;
                case GATTSERVICE_SUBEVENT_SPP_SERVICE_DISCONNECTED:
                    con_handle = HCI_CON_HANDLE_INVALID;
                    context = connection_for_conn_handle(con_handle);
                    if (!context) break;
                    context->le_notification_enabled = 0;
                    break;
                default:
                    break;
            }
            break;

            /********************************************************************************/
            /***** put your own function here to process received data from android app *****/
            /********************************************************************************/
        case RFCOMM_DATA_PACKET:
            if (packet[0] == 0x74) {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, toggle);
                // print board_button_read()
                printf("Button state: %lu\n", bootsel_board_button_read());
                toggle = !toggle;
            }
            printf("RECV: ");
            printf_hexdump(packet, size);
            context = connection_for_conn_handle((hci_con_handle_t) channel);
            if (!context) break;
            test_track_sent(context, size);
            break;
        default:
            break;
    }
}

static uint16_t
service_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset,
                      uint8_t *buffer, uint16_t buffer_size) {
    printf("service_read_callback\n");
    UNUSED(con_handle);
    UNUSED(offset);
    UNUSED(buffer_size);

    if (attribute_handle == picotype_tx_client_configuration_handle) {
        if (buffer != NULL) {
            little_endian_store_16(buffer, 0, picotype_tx_client_configuration_value);
        }
        return 2;
    }
    return 0;
}

static void service_emit_state(hci_con_handle_t con_handle, bool enabled) {
    uint8_t event[5];
    uint8_t pos = 0;
    event[pos++] = HCI_EVENT_GATTSERVICE_META;
    event[pos++] = sizeof(event) - 2;
    event[pos++] = enabled ? GATTSERVICE_SUBEVENT_SPP_SERVICE_CONNECTED : GATTSERVICE_SUBEVENT_SPP_SERVICE_DISCONNECTED;
    little_endian_store_16(event, pos, (uint16_t) con_handle);
    pos += 2;
    (*client_packet_handler)(HCI_EVENT_PACKET, 0, event, pos);
}

static int
service_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode,
                       uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    printf("service_write_callback\n");


    printf("offset: %x\n", offset);
    printf("buffer_size: %x\n", buffer_size);
    printf("buffer: ");
    printf_hexdump(buffer, buffer_size);


    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);

    if (attribute_handle == picotype_rx_value_handle) {
        (*client_packet_handler)(RFCOMM_DATA_PACKET, (uint16_t) con_handle, buffer, buffer_size);
    }

    if (attribute_handle == picotype_tx_client_configuration_handle) {
        picotype_tx_client_configuration_value = little_endian_read_16(buffer, 0);
        bool enabled = (picotype_tx_client_configuration_value != 0);
        service_emit_state(con_handle, enabled);
    }

    return 0;
}

void typing_service_server_init(btstack_packet_handler_t packet_handler) {
    client_packet_handler = packet_handler;

    // get service handle range
    uint16_t start_handle = 0;
    uint16_t end_handle = 0xffff;

    int service_found = gatt_server_get_handle_range_for_service_with_uuid128(
            primary_service_uuid,
            &start_handle,
            &end_handle
    );
    btstack_assert(service_found != 0);
    UNUSED(service_found);
    // get characteristic value handle and client configuration handle
    picotype_rx_value_handle = gatt_server_get_value_handle_for_characteristic_with_uuid128(
            start_handle,
            end_handle,
            rx_type_characteristic_uuid
    );
    picotype_tx_value_handle = gatt_server_get_value_handle_for_characteristic_with_uuid128(
            start_handle,
            end_handle,
            tx_type_characteristic_uuid
    );

    picotype_tx_client_configuration_handle = gatt_server_get_client_configuration_handle_for_characteristic_with_uuid128(
            start_handle,
            end_handle,
            tx_type_characteristic_uuid
    );

    printf("picotype_rx_value_handle \t\t\t\t\t0x%02x \n", picotype_rx_value_handle);
    printf("picotype_tx_value_handle \t\t\t\t\t0x%02x \n", picotype_tx_value_handle);
    printf("picotype_tx_client_configuration_handle \t0x%02x \n", picotype_tx_client_configuration_handle);

    // register service with ATT Server
    picotype_service_handler.start_handle = start_handle;
    picotype_service_handler.end_handle = end_handle;
    picotype_service_handler.read_callback = &service_read_callback;
    picotype_service_handler.write_callback = &service_write_callback;
    att_server_register_service_handler(&picotype_service_handler);
}