#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sara_u2.h"
#include "board_define.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/PIN.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/ioc.h>

#define SARA_UART_WRITE_READ_DATA_SIZE 256

static volatile uint8_t sara_ack = 0x00;
static volatile sara_state_t sara_evt_state = SARA_NONE_EVT;

static volatile uint8_t sara_signal_qual = 0x00;
static volatile uint8_t sara_signal_power = 0x00;

static volatile uint8_t sara_gatt_state = 0x00;

static volatile uint8_t sara_pdp_state = 0x00;

static volatile uint8_t sara_tcp_socket_number = 0x00;
static volatile uint8_t sara_tcp_server_ip_address_size = 0;

static uint8_t sara_tcp_server_ip_address[20] = { 0x00 };

static uint8_t sara_tcp_read_socket_number = 0;
static uint16_t sara_tcp_read_socket_data_size = 0;

static volatile uint8_t sara_sim_number_size = 0;
static char sara_sim_serial_number[32] = { 0x00 };

static char sara_write_command[128] = { 0x00 };
static uint16_t sara_write_command_size = 0;

static volatile uint8_t sara_tcp_read_data_step = 0x00;
static uint8_t sara_tcp_read_data[255] = { 0x00 };
static volatile uint8_t sara_tcp_read_data_size = 0;

static uint16_t data_array_index = 0;
static uint8_t data_array[SARA_UART_WRITE_READ_DATA_SIZE] = { 0x00 };

static sara_u2_data_t sara_data;
static sara_u2_callback sara_callback;

static UART_Handle uart;
static PIN_Handle pinHandle;

void set_sara_u2_gpio_instance(PIN_Handle pinHandleInstance)
{

    pinHandle = pinHandleInstance;

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 0);
    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 0);

}

void set_sara_u2_uart_instance(UART_Handle uartInstance)
{

    uart = uartInstance;

}

static void wait_us(uint32_t wait)
{

    Task_sleep(wait / Clock_tickPeriod);

}

static void wait_100_ms()
{

    Task_sleep(100 * 1000 / Clock_tickPeriod);

}

static void wait_100_ms_timer(uint32_t wait)
{

    for (uint32_t i = 0; i < wait; i++)
    {

        wait_100_ms();

    }

}

static uint8_t sara_data_write(uint8_t *data, uint16_t size, uint16_t wait)
{

    sara_ack = 0x00;

    data_array_index = 0;
    memset(data_array, 0x00, sizeof(data_array));

    UART_write(uart, data, size);
    UART_write(uart, "\r\n", 2);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (sara_ack)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

static uint8_t sara_tcp_buffer_write(uint8_t *data, uint16_t size,
                                     uint16_t wait)
{

    sara_ack = 0x00;

    data_array_index = 0;
    memset(data_array, 0x00, sizeof(data_array));

    UART_write(uart, data, size);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (sara_ack)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

void set_sara_callback(sara_u2_callback callback)
{

    sara_callback = callback;

}

uint8_t set_sara_power_on(uint8_t retry, uint16_t wait)
{

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 1);

    wait_100_ms_timer(60);

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 1);

    wait_us(65);

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 0);

    wait_100_ms_timer(30);

    for (uint8_t i = 0; i < retry; i++)
    {

        sara_evt_state = SARA_ECHO_TEST;

        if (sara_data_write((uint8_t*) SARA_AT_ECHO, strlen(SARA_AT_ECHO),
                            wait))
        {

            if (sara_evt_state == SARA_OK)
            {

                return 0x01;

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

void set_sara_power_off(uint8_t retry, uint16_t wait)
{

    for (uint8_t i = 0; i < retry; i++)
    {

        sara_evt_state = SARA_POWER_OFF;

        if (sara_data_write((uint8_t*) SARA_AT_POWER_OFF,
                            strlen(SARA_AT_POWER_OFF), wait))
        {

            if (sara_evt_state == SARA_OK)
            {

                break;

            }

        }

        wait_100_ms_timer(10);

    }

    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 0);

}

uint8_t set_sara_echo_off(uint16_t wait)
{
    sara_evt_state = SARA_ECHO_OFF;

    if (sara_data_write((uint8_t*) SARA_AT_ECHO_OFF, strlen(SARA_AT_ECHO_OFF),
                        wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_sara_power_save_mode_off(uint16_t wait)
{

    sara_evt_state = SARA_SET_POWER_SAVE_MODE;

    if (sara_data_write((uint8_t*) SARA_AT_POWER_SAVE_MODE_OFF,
                        strlen(SARA_AT_POWER_SAVE_MODE_OFF), wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_sara_gpio_1(uint16_t wait)
{

    sara_evt_state = SARA_GPIO_1_SETUP;

    if (sara_data_write((uint8_t*) SARA_AT_GPIO_1_NETWORK_STATE,
                        strlen(SARA_AT_GPIO_1_NETWORK_STATE), wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t get_sara_sim_id(uint16_t wait)
{

    sara_evt_state = SARA_READ_SIM_ID;

    if (sara_data_write((uint8_t*) SARA_AT_CHECK_SIM_ID,
                        strlen(SARA_AT_CHECK_SIM_ID), wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            sara_data.sara_sim_number_size = sara_sim_number_size;
            memcpy(sara_data.sara_sim_serial_number, sara_sim_serial_number,
                   sara_sim_number_size);

            sara_callback(sara_data);

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t get_sara_csq_network_state(uint16_t retry, uint16_t wait)
{

    for (uint16_t i = 0; i < retry; i++)
    {

        sara_evt_state = SARA_CSQ_READ;

        if (sara_data_write((uint8_t*) SARA_AT_NET_SIG_QUAL,
                            strlen(SARA_AT_NET_SIG_QUAL), wait))
        {

            if (sara_evt_state == SARA_OK)
            {

                sara_data.sara_signal_power = sara_signal_power;
                sara_data.sara_signal_qual = sara_signal_qual;
                sara_callback(sara_data);

                if (sara_signal_qual != 99)
                {

                    return 0x01;

                }

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

uint8_t get_sara_gatt_state(uint16_t retry, uint16_t wait)
{

    for (uint16_t i = 0; i < retry; i++)
    {

        sara_evt_state = SARA_GATT_READ;

        if (sara_data_write((uint8_t*) SARA_AT_NET_CHECK_GATT,
                            strlen(SARA_AT_NET_CHECK_GATT), wait))
        {

            if (sara_evt_state == SARA_OK)
            {

                sara_data.sara_gatt_state = sara_gatt_state;
                sara_callback(sara_data);

                if (sara_gatt_state != 0x00)
                {

                    return 0x01;

                }

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

uint8_t get_sara_pdp_state(uint16_t retry, uint16_t wait)
{

    for (uint16_t i = 0; i < retry; i++)
    {

        sara_evt_state = SARA_PDP_READ;

        if (sara_data_write((uint8_t*) SARA_AT_NET_CHECK_GPRS,
                            strlen(SARA_AT_NET_CHECK_GPRS), wait))
        {

            if (sara_evt_state == SARA_OK)
            {

                sara_data.sara_pdp_state = sara_pdp_state;
                sara_callback(sara_data);

                return 0x01;

            }

        }

        wait_100_ms_timer(10);

    }

    return 0x00;

}

uint8_t set_sara_gprs_state(uint16_t wait)
{

    if (sara_pdp_state == 0x00)
    {

        sara_evt_state = SARA_PDP_CONNECT;

        if (sara_data_write((uint8_t*) SARA_AT_NET_ACT_GPRS,
                            strlen(SARA_AT_NET_ACT_GPRS), wait))
        {

            if (sara_evt_state != SARA_OK)
            {

                return 0x00;

            }

        }

    }

    return 0x01;

}

uint8_t set_sara_socket_close(uint16_t wait)
{

    sara_write_command_size = 0;
    memset(sara_write_command, 0x00, sizeof(sara_write_command));

    sara_write_command_size = sprintf(sara_write_command, "AT+USOCL=%d",
                                      sara_tcp_socket_number);

    sara_evt_state = SARA_SOCKET_CLOSE;

    if (sara_data_write((uint8_t*) sara_write_command, sara_write_command_size,
                        wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_sara_socket_create(uint16_t wait)
{

    sara_evt_state = SARA_SOCKET_CREATE;

    if (sara_data_write((uint8_t*) SARA_AT_SOCKET_CMD_CREATE,
                        strlen(SARA_AT_SOCKET_CMD_CREATE), wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            sara_data.sara_tcp_socket_number = sara_tcp_socket_number;
            sara_callback(sara_data);

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_sara_socket_connect(char *ip, uint16_t port, uint16_t wait)
{

    sara_write_command_size = 0;
    memset(sara_write_command, 0x00, sizeof(sara_write_command));

    sara_write_command_size = sprintf(sara_write_command,
                                      "AT+USOCO=%d,\"%s\",%d",
                                      sara_tcp_socket_number, ip, port);

    sara_evt_state = SARA_SOCKET_CONNECT;

    if (sara_data_write((uint8_t*) sara_write_command, sara_write_command_size,
                        wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_sara_socket_write_data_size(uint16_t sara_tcp_data_size,
                                        uint16_t wait)
{

    sara_write_command_size = 0;
    memset(sara_write_command, 0x00, sizeof(sara_write_command));

    sara_write_command_size = sprintf(sara_write_command, "AT+USOWR=%d,%d",
                                      sara_tcp_socket_number,
                                      sara_tcp_data_size);

    sara_evt_state = SARA_TCP_WRITE_REQUEST;

    if (sara_data_write((uint8_t*) sara_write_command, sara_write_command_size,
                        wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            wait_100_ms();

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t set_sara_socket_write_data(uint8_t *sara_tcp_data,
                                   uint16_t sara_tcp_data_size, uint16_t wait)
{

    sara_write_command_size = 0;
    memset(sara_write_command, 0x00, sizeof(sara_write_command));

    memcpy(sara_write_command, sara_tcp_data, sara_tcp_data_size);
    sara_write_command_size = sara_tcp_data_size;

    sara_evt_state = SARA_TCP_WRITE_DATA;

    if (sara_tcp_buffer_write((uint8_t*) sara_write_command,
                              sara_write_command_size, wait))
    {

        if (sara_evt_state != SARA_OK)
        {

            return 0x00;

        }

    }

    return 0x01;

}

uint8_t get_sara_socket_wait_receive_data(uint16_t wait)
{

    sara_evt_state = SARA_TCP_READ_WAIT;

    data_array_index = 0;
    memset(data_array, 0x00, sizeof(data_array));

    for (uint16_t i = 0; i < wait; i++)
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }
        else if (sara_evt_state == SARA_ERROR)
        {

            return 0x00;

        }

        wait_100_ms();

    }

    return 0x00;

}

uint8_t get_sara_socket_receive_data(uint16_t wait)
{

    sara_evt_state = SARA_TCP_READ_DATA;

    sara_tcp_read_data_step = 0x00;
    sara_tcp_read_data_size = 0;
    memset(sara_tcp_read_data, 0x00, sizeof(sara_tcp_read_data));

    sara_write_command_size = 0;
    memset(sara_write_command, 0x00, sizeof(sara_write_command));

    sara_write_command_size = sprintf(sara_write_command, "AT+USORD=%d,%d",
                                      sara_tcp_read_socket_number,
                                      sara_tcp_read_socket_data_size);

    if (sara_data_write((uint8_t*) sara_write_command, sara_write_command_size,
                        wait))
    {

        if (sara_evt_state == SARA_OK)
        {

            return 0x01;

        }

    }

    return 0x00;

}

uint8_t sara_u2_init_config_setup(void)
{

    if (!set_sara_echo_off(10))
    {

        return 0x00;

    }

    if (!get_sara_sim_id(10))
    {

        return 0x00;

    }

    return 0x01;

}

uint8_t sara_u2_init_network_setup(void)
{

    if (!get_sara_csq_network_state(1000, 10))
    {

        return 0x00;

    }

    if (!get_sara_gatt_state(1000, 10))
    {

        return 0x00;

    }

    if (!get_sara_pdp_state(1000, 10))
    {

        return 0x00;

    }

    if (!set_sara_gprs_state(1000))
    {

        return 0x00;

    }

    return 0x01;

}

uint8_t sara_u2_tcp_data_upload(uint8_t *buffer, uint16_t size)
{

    if (!set_sara_socket_create(100))
    {

        return 0x00;

    }

    if (!set_sara_socket_connect("121.130.26.214", 8888, 100))
    {

        return 0x00;

    }

    if (!set_sara_socket_write_data_size(size, 10))
    {

        return 0x00;

    }

    if (!set_sara_socket_write_data(buffer, size, 100))
    {

        return 0x00;

    }

    if (!get_sara_socket_wait_receive_data(100))
    {

        return 0x00;

    }

    if (!get_sara_socket_receive_data(50))
    {

        return 0x00;

    }

    return 0x01;

}

uint16_t get_sarau2_socket_recevice_buffer_datat(uint8_t *buffer)
{

    memcpy(buffer, sara_tcp_read_data, sara_tcp_read_socket_data_size);

    return sara_tcp_read_socket_data_size;

}

void sara_u2_uart_input(uint8_t data)
{

    data_array[data_array_index++] = data;

    if (sara_evt_state == SARA_TCP_WRITE_REQUEST)
    {

        if (data_array[data_array_index - 1] == '@')
        {

            sara_ack = 0x01;
            sara_evt_state = SARA_OK;

        }

        data_array_index = 0;

    }
    else if (sara_evt_state == SARA_TCP_READ_DATA)
    {

        if (sara_tcp_read_data_step == 0x00)
        {

            if (data_array[data_array_index - 1] == '+')
            {

                sara_tcp_read_data_step = 0x01;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x01)
        {

            if (data_array[data_array_index - 1] == 'U')
            {

                sara_tcp_read_data_step = 0x02;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x02)
        {

            if (data_array[data_array_index - 1] == 'S')
            {

                sara_tcp_read_data_step = 0x03;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x03)
        {

            if (data_array[data_array_index - 1] == 'O')
            {

                sara_tcp_read_data_step = 0x04;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x04)
        {

            if (data_array[data_array_index - 1] == 'R')
            {

                sara_tcp_read_data_step = 0x05;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x05)
        {

            if (data_array[data_array_index - 1] == 'D')
            {

                sara_tcp_read_data_step = 0x06;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x06)
        {

            if (data_array[data_array_index - 1] == ':')
            {

                sara_tcp_read_data_step = 0x07;

            }
            else
            {

                sara_tcp_read_data_step = 0x00;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x07)
        {

            if (data_array[data_array_index - 1] == '\"')
            {

                sara_tcp_read_data_step = 0x08;

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x08)
        {

            if (sara_tcp_read_data_size >= sara_tcp_read_socket_data_size)
            {

                sara_tcp_read_data_step = 0x09;

            }
            else
            {

                sara_tcp_read_data[sara_tcp_read_data_size++] =
                        data_array[data_array_index - 1];

            }

            data_array_index = 0;

        }
        else if (sara_tcp_read_data_step == 0x09)
        {

            if ((data_array[data_array_index - 1] == '\n')
                    && (data_array[data_array_index - 2] == '\r'))
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }

                data_array_index = 0;

            }

        }

    }
    else
    {

        if ((data_array[data_array_index - 1] == '\n')
                && (data_array[data_array_index - 2] == '\r'))
        {

            if (sara_evt_state == SARA_ECHO_TEST)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }

            }
            else if (sara_evt_state == SARA_GPIO_1_SETUP)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_SET_POWER_SAVE_MODE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_ECHO_OFF)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_DISABLE_ETC)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_READ_SIM_ID)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("+CCID: ", (char*) data_array, 7) == 0)
                {

                    sara_sim_number_size = 0;

                    for (uint16_t i = 7; i < data_array_index - 2; i++)
                    {

                        sara_sim_serial_number[sara_sim_number_size++] =
                                data_array[i];

                    }

                }

            }
            else if (sara_evt_state == SARA_CSQ_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("+CSQ: ", (char*) data_array, 6) == 0)
                {

                    char *ptr = (char*) data_array;
                    sara_signal_power = strtol(ptr + 6, &ptr, 10);
                    sara_signal_qual = strtol(ptr + 1, &ptr, 10);

                }

            }
            else if (sara_evt_state == SARA_GATT_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("+CGATT: ", (char*) data_array, 8) == 0)
                {

                    char *ptr = (char*) data_array;
                    sara_gatt_state = strtol(ptr + 8, &ptr, 10);

                }

            }
            else if (sara_evt_state == SARA_PDP_READ)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("+UPSND: ", (char*) data_array, 8) == 0)
                {

                    char *ptr = (char*) data_array;
                    sara_pdp_state = strtol(ptr + 9, &ptr, 10);
                    sara_pdp_state = strtol(ptr + 1, &ptr, 10);
                    sara_pdp_state = strtol(ptr + 1, &ptr, 10);

                }

            }
            else if (sara_evt_state == SARA_PDP_CONNECT)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_SOCKET_CREATE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("+USOCR: ", (char*) data_array, 8) == 0)
                {

                    char *ptr = (char*) data_array;
                    sara_tcp_socket_number = strtol(ptr + 8, &ptr, 10);

                }

            }
            else if (sara_evt_state == SARA_DNS_RESOLUTION)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("+UDNSRN: ", (char*) data_array, 9) == 0)
                {

                    uint16_t i = 0;
                    sara_tcp_server_ip_address_size = 0;

                    for (i = 10; i < data_array_index; i++)
                    {

                        if (data_array[i] != '\"')
                        {

                            sara_tcp_server_ip_address[sara_tcp_server_ip_address_size++] =
                                    data_array[i];

                        }
                        else
                        {

                            break;

                        }

                    }

                }

            }
            else if (sara_evt_state == SARA_SOCKET_CONNECT)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_TCP_WRITE_DATA)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("+UUSOCL: ", (char*) data_array, 9) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_TCP_READ_WAIT)
            {

                if (strncmp("+UUSORD: ", (char*) data_array, 9) == 0)
                {

                    char *ptr = (char*) data_array;
                    sara_tcp_read_socket_number = strtol(ptr + 9, &ptr, 10);
                    sara_tcp_read_socket_data_size = strtol(ptr + 1, &ptr, 10);

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("+UUSOCL: ", (char*) data_array, 9) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }
            else if (sara_evt_state == SARA_SOCKET_CLOSE)
            {

                if (strncmp("OK", (char*) data_array, 2) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_OK;

                }
                else if (strncmp("ERROR", (char*) data_array, 5) == 0)
                {

                    sara_ack = 0x01;
                    sara_evt_state = SARA_ERROR;

                }

            }

            data_array_index = 0;

        }

    }

    if (data_array_index >= SARA_UART_WRITE_READ_DATA_SIZE)
    {

        data_array_index = 0;

    }

}

