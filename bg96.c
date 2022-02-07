#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bg96.h"
#include "board_define.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/PIN.h>

#define BG96_UART_WRITE_READ_DATA_SIZE 256

static volatile uint8_t bg96_ack = 0x00;
static volatile bg96_state_t bg96_evt_state = BG96_NONE_EVT;

static volatile uint8_t bg96_signal_qual = 0x00;
static volatile uint8_t bg96_signal_power = 0x00;

static volatile uint8_t bg96_2g_3g_4g_network_state = 0x00;
static volatile uint8_t bg96_tcp_ip_pdp_state = 0x00;

static volatile uint8_t bg96_tcp_socket_connection_state = 0x00;

static volatile uint8_t bg96_tcp_socket_number = 0x00;
static volatile uint8_t bg96_tcp_server_ip_address_size = 0;

static volatile uint8_t bg96_sim_ready_state_size = 0;
static char bg96_sim_ready_state[32] = { 0x00 };

static volatile uint8_t bg96_sim_number_size = 0;
static char bg96_sim_serial_number[32] = { 0x00 };

static char bg96_write_command[BG96_UART_WRITE_READ_DATA_SIZE] = { 0x00 };
static uint16_t bg96_write_command_size = 0;

static volatile uint8_t bg96_tcp_read_data_step = 0x00;
static uint8_t bg96_tcp_read_data[BG96_UART_WRITE_READ_DATA_SIZE] = { 0x00 };

static volatile uint8_t bg96_tcp_read_data_index = 0;
static volatile uint8_t bg96_tcp_read_data_size = 0;

static uint16_t data_array_index = 0;
static uint8_t data_array[BG96_UART_WRITE_READ_DATA_SIZE] = { 0x00 };

static bg96_data_t bg96_data;
static bg96_callback bg96_callback_receiver;

static UART_Handle uart;
static PIN_Handle pinHandle;

void set_bg96_gpio_instance(PIN_Handle pinHandleInstance)
{

    pinHandle = pinHandleInstance;

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 0);
    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 0);

}

void set_bg96_uart_instance(UART_Handle uartInstance)
{

    uart = uartInstance;

}

static void wait_100_ms()
{

    Task_sleep(100 * 1000 / Clock_tickPeriod);

}

static void wait_100_ms_timer(uint32_t wait)
{

    for (uint16_t i = 0; i < wait; i++)
    {

        wait_100_ms();

    }

}

static uint8_t bg96_data_write(uint8_t *data, uint16_t size, uint16_t wait)
{

    bg96_ack = 0x00;

    data_array_index = 0;
    memset(data_array, 0x00, sizeof(data_array));

    UART_write(uart, data, size);
    UART_write(uart, "\r\n", 2);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (bg96_ack)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

static uint8_t bg96_tcp_buffer_write(uint8_t *data, uint16_t size,
                                     uint16_t wait)
{

    bg96_ack = 0x00;

    data_array_index = 0;
    memset(data_array, 0x00, sizeof(data_array));

    UART_write(uart, data, size);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (bg96_ack)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

void set_bg96_callback(bg96_callback callback)
{

    bg96_callback_receiver = callback;

}

uint8_t set_bg96_power_on(uint8_t retry, uint16_t wait)
{

    bg96_evt_state = BG96_NONE_EVT;

    memset(&bg96_data, 0x00, sizeof(bg96_data_t));
    bg96_data.bg96_signal_power = 99;
    bg96_data.bg96_signal_qual = 99;

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 1);

    wait_100_ms_timer(70);

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 1);

    wait_100_ms_timer(8);

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 0);

    wait_100_ms_timer(50);

    for (uint8_t i = 0; i < retry; i++)
    {

        bg96_evt_state = BG96_ECHO_TEST;

        if (bg96_data_write((uint8_t*) BG96_AT_ECHO, strlen(BG96_AT_ECHO),
                            wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                return 0x01;

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

void set_bg96_power_off(uint8_t retry, uint16_t wait)
{

    for (uint8_t i = 0; i < retry; i++)
    {

        bg96_evt_state = BG96_POWER_OFF;

        if (bg96_data_write((uint8_t*) BG96_AT_POWER_DOWN,
                            strlen(BG96_AT_POWER_DOWN), wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                break;

            }

        }

        wait_100_ms_timer(10);

    }

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 0);

}

uint8_t set_bg96_echo_off(uint16_t wait)
{
    bg96_evt_state = BG96_ECHO_OFF;

    if (bg96_data_write((uint8_t*) BG96_AT_ECHO_OFF, strlen(BG96_AT_ECHO_OFF),
                        wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t get_bg96_sim_state(uint8_t retry, uint16_t wait)
{

    for (uint8_t i = 0; i < retry; i++)
    {

        bg96_evt_state = BG96_SIM_READY_STATE_READ;

        if (bg96_data_write((uint8_t*) BG96_AT_SIM_READY_READ,
                            strlen(BG96_AT_SIM_READY_READ), wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                if (strncmp("READY", (char*) bg96_sim_ready_state, 5) == 0)
                {

                    return 0x01;

                }

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

uint8_t get_bg96_sim_id(uint16_t wait)
{

    bg96_evt_state = BG96_READ_SIM_ID;

    if (bg96_data_write((uint8_t*) BG96_AT_SIM_ICCID_READ,
                        strlen(BG96_AT_SIM_ICCID_READ), wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            bg96_data.bg96_sim_number_size = bg96_sim_number_size;
            memcpy(bg96_data.bg96_sim_serial_number, bg96_sim_serial_number,
                   bg96_sim_number_size);

            bg96_callback_receiver(bg96_data);

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t get_bg96_csq_network_state(uint16_t retry, uint16_t wait)
{

    for (uint16_t i = 0; i < retry; i++)
    {

        bg96_evt_state = BG96_CSQ_READ;

        if (bg96_data_write((uint8_t*) BG96_AT_CSQ_READ,
                            strlen(BG96_AT_CSQ_READ), wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                bg96_data.bg96_signal_power = bg96_signal_power;
                bg96_data.bg96_signal_qual = bg96_signal_qual;
                bg96_callback_receiver(bg96_data);

                if (bg96_signal_power != 99 || bg96_signal_qual != 99)
                {

                    return 0x01;

                }

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

uint8_t set_bg96_network_registration_enable(uint16_t wait)
{

    bg96_evt_state = BG96_NETWORK_REGISTRATION_ENABLE;

    if (bg96_data_write((uint8_t*) BG96_AT_NETWORK_REGISTRAION_ENABLE,
                        sizeof(BG96_AT_NETWORK_REGISTRAION_ENABLE), wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_bg96_disable_power_save_mode(uint8_t wait)
{

    bg96_evt_state = BG96_SET_POWER_SAVE_MODE;

    if (bg96_data_write((uint8_t*) BG96_AT_POWER_SAVE_MODE_DISABLE,
                        sizeof(BG96_AT_POWER_SAVE_MODE_DISABLE), wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t get_bg96_2g_3g_4g_network_registration(uint16_t retry, uint16_t wait)
{

    for (uint16_t i = 0; i < retry; i++)
    {

        //2G 3G Network State Read
        bg96_evt_state = BG96_2G_3G_4G_NETWORK_STATE_READ;

        if (bg96_data_write((uint8_t*) BG96_AT_2G_3G_NETWORK_STATE_READ,
                            strlen(BG96_AT_2G_3G_NETWORK_STATE_READ), wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                bg96_data.bg96_2g_3g_4g_network_state =
                        bg96_2g_3g_4g_network_state;
                bg96_callback_receiver(bg96_data);

                if (bg96_2g_3g_4g_network_state == 1
                        || bg96_2g_3g_4g_network_state == 5)
                {

                    return 0x01;

                }

            }

        }

        wait_100_ms_timer(10);

        //4G Network State Read
        bg96_evt_state = BG96_2G_3G_4G_NETWORK_STATE_READ;

        if (bg96_data_write((uint8_t*) BG96_AT_4G_NETWORK_STATE_READ,
                            strlen(BG96_AT_4G_NETWORK_STATE_READ), wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                bg96_data.bg96_2g_3g_4g_network_state =
                        bg96_2g_3g_4g_network_state;
                bg96_callback_receiver(bg96_data);

                if (bg96_2g_3g_4g_network_state == 1
                        || bg96_2g_3g_4g_network_state == 5)
                {

                    return 0x01;

                }

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

uint8_t set_bg96_tcp_ip_pdp_config(uint8_t context_id, uint8_t ip_protocol,
                                   uint8_t *apn_name, uint16_t wait)
{

    bg96_write_command_size = sprintf(bg96_write_command,
                                      "AT+QICSGP=%d,%d,%s,\"\",\"\",0",
                                      context_id, ip_protocol, apn_name);

    bg96_evt_state = BG96_TCP_IP_PDP_CONFIG;

    if (bg96_data_write((uint8_t*) bg96_write_command, bg96_write_command_size,
                        wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_bg96_tcp_ip_pdp_activate(uint16_t wait)
{

    bg96_evt_state = BG96_TCP_IP_PDP_ACTIVATE;

    if (bg96_data_write((uint8_t*) BG96_AT_PDP_ACTIVATE,
                        sizeof(BG96_AT_PDP_ACTIVATE), wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t get_bg96_tcp_ip_pdp_state(uint16_t retry, uint16_t wait)
{

    for (uint16_t i = 0; i < retry; i++)
    {

        bg96_evt_state = BG96_TCP_IP_PDP_READ;

        if (bg96_data_write((uint8_t*) BG96_AT_PDP_STATE_READ,
                            sizeof(BG96_AT_PDP_STATE_READ), wait))
        {

            if (bg96_evt_state == BG96_OK)
            {

                if (bg96_tcp_ip_pdp_state == 1)
                {

                    return 0x01;

                }

            }

        }

    }

    return 0x00;

}

uint8_t set_bg96_socket_close(uint16_t wait)
{

    bg96_write_command_size = 0;
    memset(bg96_write_command, 0x00, sizeof(bg96_write_command));

    bg96_write_command_size = sprintf(bg96_write_command, "AT+QICLOSE=%d",
                                      bg96_tcp_socket_number);

    bg96_evt_state = BG96_SOCKET_CLOSE;

    if (bg96_data_write((uint8_t*) bg96_write_command, bg96_write_command_size,
                        wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_bg96_socket_connect(uint8_t socket_number, char *ip, uint16_t port,
                                uint16_t wait)
{

    bg96_write_command_size = 0;
    memset(bg96_write_command, 0x00, sizeof(bg96_write_command));

    bg96_write_command_size = sprintf(bg96_write_command,
                                      "AT+QIOPEN=1,%d,\"TCP\",\"%s\",%d,0,1",
                                      socket_number, ip, port);

    bg96_evt_state = BG96_SOCKET_CONNECT;

    if (bg96_data_write((uint8_t*) bg96_write_command, bg96_write_command_size,
                        wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            if (bg96_tcp_socket_connection_state == 0)
            {

                return 0x01;

            }

        }

    }

    return 0x00;

}

uint8_t set_bg96_socket_write_data_size(uint16_t bg96_tcp_data_size,
                                        uint16_t wait)
{

    bg96_write_command_size = 0;
    memset(bg96_write_command, 0x00, sizeof(bg96_write_command));

    bg96_write_command_size = sprintf(bg96_write_command, "AT+QISEND=%d,%d",
                                      bg96_tcp_socket_number,
                                      bg96_tcp_data_size);

    bg96_evt_state = BG96_TCP_WRITE_REQUEST;

    if (bg96_data_write((uint8_t*) bg96_write_command, bg96_write_command_size,
                        wait))
    {

        if (bg96_evt_state == BG96_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_bg96_socket_write_data(uint8_t *bg96_tcp_data,
                                   uint16_t bg96_tcp_data_size, uint16_t wait)
{

    bg96_write_command_size = 0;
    memset(bg96_write_command, 0x00, sizeof(bg96_write_command));

    memcpy(bg96_write_command, bg96_tcp_data, bg96_tcp_data_size);
    bg96_write_command_size = bg96_tcp_data_size;

    bg96_evt_state = BG96_TCP_WRITE_DATA;

    if (bg96_tcp_buffer_write((uint8_t*) bg96_write_command,
                              bg96_write_command_size, wait))
    {

        if (bg96_evt_state != BG96_OK)
        {

            return 0x00;

        }

    }

    return 0x01;

}

uint8_t bg96_init_config_setup(void)
{

    if (!set_bg96_echo_off(10))
    {

        return 0x00;

    }

    if (!get_bg96_sim_state(30, 10))
    {

        return 0x00;

    }

    if (!get_bg96_sim_id(10))
    {

        return 0x00;

    }

    return 0x01;

}

uint8_t bg96_init_network_setup(void)
{

    if (!get_bg96_2g_3g_4g_network_registration(1000, 10))
    {

        return 0x00;

    }

    if (!set_bg96_tcp_ip_pdp_activate(1000))
    {

        return 0x00;

    }

    if (!get_bg96_csq_network_state(1000, 10))
    {

        return 0x00;

    }

    return 0x01;

}

uint8_t bg96_tcp_data_upload(uint8_t *buffer, uint16_t size)
{

    if (!set_bg96_socket_connect(0, "121.130.26.214", 8888, 100))
    {

        return 0x00;

    }

    if (!set_bg96_socket_write_data_size(size, 50))
    {

        return 0x00;

    }

    if (!set_bg96_socket_write_data(buffer, size, 100))
    {

        return 0x00;

    }

    return 0x01;

}

uint16_t get_bg96_socket_recevice_buffer_data(uint8_t *buffer)
{

    memcpy(buffer, bg96_tcp_read_data, bg96_tcp_read_data_size);

    return bg96_tcp_read_data_size;

}

void bg96_uart_input(uint8_t data)
{

    data_array[data_array_index++] = data;

    if (bg96_evt_state == BG96_TCP_WRITE_REQUEST)
    {

        if (data_array[data_array_index - 1] == '>')
        {

            bg96_ack = 0x01;
            bg96_evt_state = BG96_OK;

        }

        data_array_index = 0;

    }
    else if (bg96_evt_state == BG96_TCP_READ_DATA)
    {

        if (bg96_tcp_read_data_step == 0x00)
        {

            if ((data_array[data_array_index - 1] == '\n')
                    && (data_array[data_array_index - 2] == '\r'))
            {

                if (strncmp("+QIRD: ", (char*) data_array, 7) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_tcp_read_data_size = strtol(ptr + 6, &ptr, 10);
                    bg96_tcp_read_data_index = 0;

                    bg96_tcp_read_data_step = 0x01;

                }

                data_array_index = 0;

            }

        }
        else if (bg96_tcp_read_data_step == 0x01)
        {

            if (bg96_tcp_read_data_index >= bg96_tcp_read_data_size)
            {

                if ((data_array[data_array_index - 1] == '\n')
                        && (data_array[data_array_index - 2] == '\r'))
                {

                    if (strncmp("OK", (char*) data_array, 2) == 0)
                    {

                        bg96_ack = 0x01;
                        bg96_evt_state = BG96_OK;

                    }

                }

            }
            else
            {

                bg96_tcp_read_data[bg96_tcp_read_data_index++] =
                        data_array[data_array_index - 1];

            }

        }

    }
    else
    {

        if ((data_array[data_array_index - 1] == '\n')
                && (data_array[data_array_index - 2] == '\r'))
        {

            if (bg96_evt_state == BG96_ECHO_TEST)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }

            }
            else if (bg96_evt_state == BG96_SET_POWER_SAVE_MODE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_ECHO_OFF)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_SIM_READY_STATE_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+CPIN: ", (char*) data_array, 7) == 0)
                {

                    bg96_sim_ready_state_size = 0;

                    for (uint16_t i = 7; i < data_array_index - 2; i++)
                    {

                        bg96_sim_ready_state[bg96_sim_ready_state_size++] =
                                data_array[i];

                    }

                }

            }
            else if (bg96_evt_state == BG96_READ_SIM_ID)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+QCCID: ", (char*) data_array, 8) == 0)
                {

                    bg96_sim_number_size = 0;

                    for (uint16_t i = 8; i < data_array_index - 2; i++)
                    {

                        bg96_sim_serial_number[bg96_sim_number_size++] =
                                data_array[i];

                    }

                }

            }
            else if (bg96_evt_state == BG96_CSQ_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+CSQ: ", (char*) data_array, 6) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_signal_power = strtol(ptr + 6, &ptr, 10);
                    bg96_signal_qual = strtol(ptr + 1, &ptr, 10);

                }

            }
            else if (bg96_evt_state == BG96_NETWORK_REGISTRATION_ENABLE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_2G_3G_4G_NETWORK_STATE_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+CGREG: ", (char*) data_array, 7) == 0
                        || strncmp("+CEREG: ", (char*) data_array, 7) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_2g_3g_4g_network_state = strtol(ptr + 7, &ptr, 10);
                    bg96_2g_3g_4g_network_state = strtol(ptr + 1, &ptr, 10);

                }

            }
            else if (bg96_evt_state == BG96_TCP_IP_PDP_CONFIG)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_TCP_IP_PDP_ACTIVATE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_TCP_IP_PDP_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+QIACT: ", (char*) data_array, 8) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_tcp_ip_pdp_state = strtol(ptr + 8, &ptr, 10);
                    bg96_tcp_ip_pdp_state = strtol(ptr + 1, &ptr, 10);

                }

            }
            else if (bg96_evt_state == BG96_SOCKET_CREATE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+USOCR: ", (char*) data_array, 8) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_tcp_socket_number = strtol(ptr + 8, &ptr, 10);
                    bg96_tcp_socket_connection_state = strtol(ptr + 1, &ptr,
                                                              10);
                }

            }
            else if (bg96_evt_state == BG96_SOCKET_CONNECT)
            {

                if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("+QIOPEN: ", (char*) data_array, 9) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_tcp_socket_number = strtol(ptr + 9, &ptr, 10);

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }

            }
            else if (bg96_evt_state == BG96_TCP_WRITE_DATA)
            {

                if (strncmp("SEND OK", (char*) data_array, 7) == 0)
                {

                    bg96_evt_state = BG96_TCP_READ_WAIT;

                }
                else if (strncmp("SEND FAIL", (char*) data_array, 9) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_TCP_READ_WAIT)
            {

                if (strncmp("+QIURC: \"recv\",", (char*) data_array, 15) == 0)
                {

                    char *ptr = (char*) data_array;
                    bg96_tcp_socket_number = strtol(ptr + 15, &ptr, 10);
                    bg96_tcp_read_data_size = strtol(ptr + 1, &ptr, 10);
                    bg96_tcp_read_data_index = 0;

                    if (bg96_tcp_read_data_size > 0)
                    {

                        bg96_tcp_read_data_step = 0x01;
                        bg96_evt_state = BG96_TCP_READ_DATA;

                    }
                    else
                    {

                        bg96_ack = 0x01;
                        bg96_evt_state = BG96_OK;

                    }

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }
            else if (bg96_evt_state == BG96_SOCKET_CLOSE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    bg96_ack = 0x01;
                    bg96_evt_state = BG96_ERROR;

                }

            }

            data_array_index = 0;

        }

    }

    if (data_array_index >= BG96_UART_WRITE_READ_DATA_SIZE)
    {

        data_array_index = 0;

    }

}

