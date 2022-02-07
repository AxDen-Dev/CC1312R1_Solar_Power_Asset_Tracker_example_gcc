#ifndef BG96_H_
#define BG96_H_

#include <stdint.h>
#include <stdbool.h>
#include "bg96_define.h"
#include "board_define.h"

#ifdef CC1312R1

#include <ti/drivers/UART.h>
#include <ti/drivers/PIN.h>
#include "ti_drivers_config.h"

#endif

typedef enum
{
    BG96_ERROR = 0,
    BG96_OK,
    BG96_NONE_EVT,
    BG96_WAIT_RESPONSE,
    BG96_ECHO_TEST,
    BG96_ECHO_OFF,
    BG96_SET_POWER_SAVE_MODE,
    BG96_DISABLE_ETC,
    BG96_READ_SIM_ID,
    BG96_CSQ_READ,

    BG96_SIM_READY_STATE_READ,
    BG96_NETWORK_REGISTRATION_ENABLE,
    BG96_2G_3G_4G_NETWORK_STATE_READ,
    BG96_TCP_IP_PDP_CONFIG,
    BG96_TCP_IP_PDP_ACTIVATE,
    BG96_TCP_IP_PDP_READ,

    BG96_GATT_READ,
    BG96_PDP_READ,
    BG96_PDP_CONNECT,
    BG96_SOCKET_CREATE,
    BG96_SOCKET_CLOSE,
    BG96_DNS_RESOLUTION,
    BG96_SOCKET_CONNECT,
    BG96_TCP_WRITE_REQUEST,
    BG96_TCP_WRITE_DATA,
    BG96_TCP_READ_WAIT,
    BG96_TCP_READ_DATA,
    BG96_UHTTP,
    BG96_TOKEN_DOMAIN,
    BG96_TOKEN_PORT,
    BG96_DELETE_PREV_FILE,
    BG96_DELETE_POST_FILE,
    BG96_HTTP_FILE_WRITE,
    BG96_HTTP_FILE_DATA_WRITE,
    BG96_TOKEN_SERVER_WRITE,
    BG96_TOKEN_RESPONSE_WAIT,
    BG96_READ_TOKEN,
    BG96_POST_DATA_UPLOAD,
    BG96_POST_DATA_UPLOAD_WAIT,
    BG96_HTTP_RESPONSE_DATA_READ,
    BG96_PROFILE_SET,
    BG96_STORE_PROFILE,
    BG96_SHOW_PROFILE,
    BG96_SAVE_PROFILE_AND_RESET,
    BG96_WAIT,
    BG96_ERROR_RESET_WAIT,
    BG96_FORCE_LIPO_CHARGE,
    BG96_SIM_VALUE,
    BG96_CSQ_LEVEL,
    BG96_SIM_CSQ_LEVEL,
    BG96_GPIO_1_SETUP,
    BG96_POWER_OFF

} bg96_state_t;

typedef struct
{

    uint8_t bg96_signal_qual;
    uint8_t bg96_signal_power;

    uint8_t bg96_2g_3g_4g_network_state;

    uint8_t bg96_pdp_state;

    uint8_t bg96_tcp_socket_number;

    uint8_t bg96_tcp_server_ip_address_size;
    uint8_t bg96_tcp_server_ip_address[20];

    uint8_t bg96_sim_number_size;
    uint8_t bg96_sim_serial_number[32];

    uint8_t bg96_setup_data_state;
    uint8_t bg96_setup_data_device_type;

} bg96_data_t;

typedef void (*bg96_callback)(bg96_data_t data);

void set_bg96_gpio_instance(PIN_Handle pinHandle);

void set_bg96_uart_instance(UART_Handle uart);

void set_bg96_callback(bg96_callback callback);

uint8_t set_bg96_power_on(uint8_t retry, uint16_t wait);

void set_bg96_power_off(uint8_t retry, uint16_t wait);

uint8_t set_bg96_echo_off(uint16_t wait);

uint8_t get_bg96_sim_id(uint16_t wait);

uint8_t get_bg96_sim_state(uint8_t retry, uint16_t wait);

uint8_t get_bg96_csq_network_state(uint16_t retry, uint16_t wait);

uint8_t set_bg96_network_registration_enable(uint16_t wait);

uint8_t get_bg96_2g_3g_4g_network_registration(uint16_t retry, uint16_t wait);

uint8_t set_bg96_tcp_ip_pdp_config(uint8_t context_id, uint8_t ip_protocol,
                                   uint8_t *apn_name, uint16_t wait);

uint8_t set_bg96_tcp_ip_pdp_activate(uint16_t wait);

uint8_t set_bg96_socket_close(uint16_t wait);

uint8_t set_bg96_socket_connect(uint8_t socket_number, char *ip, uint16_t port,
                                uint16_t wait);

uint8_t set_bg96_socket_write_data_size(uint16_t bg96_tcp_data_size,
                                        uint16_t wait);

uint8_t set_bg96_socket_write_data(uint8_t *bg96_tcp_data,
                                   uint16_t bg96_tcp_data_size, uint16_t wait);

uint8_t bg96_init_config_setup(void);

uint8_t bg96_init_network_setup(void);

uint16_t get_bg96_socket_recevice_buffer_data(uint8_t *buffer);

uint8_t bg96_tcp_data_upload(uint8_t *buffer, uint16_t size);

void bg96_uart_input(uint8_t data);

#endif /* BG96_H_ */
