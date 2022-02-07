#ifndef SARA_U2_H_
#define SARA_U2_H_

#include <stdint.h>
#include <stdbool.h>
#include "board_define.h"
#include "sara_define.h"

#include <ti/drivers/UART.h>
#include <ti/drivers/PIN.h>
#include "ti_drivers_config.h"

typedef enum
{
    SARA_ERROR = 0,
    SARA_OK,
    SARA_NONE_EVT,
    SARA_WAIT_RESPONSE,
    SARA_ECHO_TEST,
    SARA_ECHO_OFF,
    SARA_SET_POWER_SAVE_MODE,
    SARA_DISABLE_ETC,
    SARA_READ_SIM_ID,
    SARA_CSQ_READ,
    SARA_GATT_READ,
    SARA_PDP_READ,
    SARA_PDP_CONNECT,
    SARA_SOCKET_CREATE,
    SARA_SOCKET_CLOSE,
    SARA_DNS_RESOLUTION,
    SARA_SOCKET_CONNECT,
    SARA_TCP_WRITE_REQUEST,
    SARA_TCP_WRITE_DATA,
    SARA_TCP_READ_WAIT,
    SARA_TCP_READ_DATA,
    SARA_UHTTP,
    SARA_TOKEN_DOMAIN,
    SARA_TOKEN_PORT,
    SARA_DELETE_PREV_FILE,
    SARA_DELETE_POST_FILE,
    SARA_HTTP_FILE_WRITE,
    SARA_HTTP_FILE_DATA_WRITE,
    SARA_TOKEN_SERVER_WRITE,
    SARA_TOKEN_RESPONSE_WAIT,
    SARA_READ_TOKEN,
    SARA_POST_DATA_UPLOAD,
    SARA_POST_DATA_UPLOAD_WAIT,
    SARA_HTTP_RESPONSE_DATA_READ,
    SARA_PROFILE_SET,
    SARA_STORE_PROFILE,
    SARA_SHOW_PROFILE,
    SARA_SAVE_PROFILE_AND_RESET,
    SARA_WAIT,
    SARA_ERROR_RESET_WAIT,
    SARA_FORCE_LIPO_CHARGE,
    SARA_SIM_VALUE,
    SARA_CSQ_LEVEL,
    SARA_SIM_CSQ_LEVEL,
    SARA_GPIO_1_SETUP,
    SARA_POWER_OFF
} sara_state_t;

typedef struct
{

    uint8_t sara_signal_qual;
    uint8_t sara_signal_power;

    uint8_t sara_gatt_state;

    uint8_t sara_pdp_state;

    uint8_t sara_tcp_socket_number;

    uint8_t sara_sim_number_size;
    uint8_t sara_sim_serial_number[32];

    uint8_t sara_setup_data_state;
    uint8_t sara_setup_data_device_type;

} sara_u2_data_t;

typedef void (*sara_u2_callback)(sara_u2_data_t sara_u2_data);

void set_sara_u2_gpio_instance(PIN_Handle pinHandle);

void set_sara_u2_uart_instance(UART_Handle uart);

void set_sara_callback(sara_u2_callback callback);

uint8_t set_sara_power_on(uint8_t retry, uint16_t wait);

void set_sara_power_off(uint8_t retry, uint16_t wait);

uint8_t set_sara_echo_off(uint16_t wait);

uint8_t set_sara_gpio_1(uint16_t wait);

uint8_t get_sara_sim_id(uint16_t wait);

uint8_t get_sara_csq_network_state(uint16_t retry, uint16_t wait);

uint8_t get_sara_gatt_state(uint16_t retry, uint16_t wait);

uint8_t get_sara_pdp_state(uint16_t retry, uint16_t wait);

uint8_t set_sara_gprs_state(uint16_t wait);

uint8_t set_sara_socket_close(uint16_t wait);

uint8_t set_sara_power_save_mode_off(uint16_t wait);

uint8_t set_sara_socket_create(uint16_t wait);

uint8_t set_sara_socket_connect(char *ip, uint16_t port, uint16_t wait);

uint8_t set_sara_socket_write_data_size(uint16_t sara_tcp_data_size,
                                        uint16_t wait);

uint8_t set_sara_socket_write_data(uint8_t *sara_tcp_data,
                                   uint16_t sara_tcp_data_size, uint16_t wait);

uint8_t get_sara_socket_wait_receive_data(uint16_t wait);

uint8_t get_sara_socket_receive_data(uint16_t wait);

uint8_t sara_u2_init_config_setup(void);

uint8_t sara_u2_init_network_setup(void);

uint16_t get_sarau2_socket_recevice_buffer_datat(uint8_t *buffer);

uint8_t sara_u2_tcp_data_upload(uint8_t *buffer, uint16_t size);

void sara_u2_uart_input(uint8_t data);

#endif /* SARA_U2_H_ */
