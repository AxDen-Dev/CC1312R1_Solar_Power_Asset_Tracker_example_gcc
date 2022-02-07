
#include "ublox_gps.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ublox_define.h"
#include "board_define.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#define ZOE_M8Q_I2C_ADDRESS 0x42
#define UBX_WRITE_READ_DATA_SIZE 128

static volatile uint8_t ubx_step = 0x00;
static uint8_t ubx_message_class = 0x00;
static uint8_t ubx_message_id = 0x00;
static uint16_t ubx_payload_size = 0x00;
static uint8_t ubx_crc_a = 0x00;
static uint8_t ubx_crc_b = 0x00;

static uint16_t ubx_payload_index = 0;
static uint8_t ubx_payload[UBX_WRITE_READ_DATA_SIZE] = { 0x00 };

static ublox_gps_data_t ublox_gps_data;
static ublox_gps_callback ublox_callback;

static uint8_t txSize = 0;
static uint8_t rxSize = 0;
static uint8_t txBuffer[UBX_WRITE_READ_DATA_SIZE] = { 0x00 };
static uint8_t rxBuffer[UBX_WRITE_READ_DATA_SIZE] = { 0x00 };

static I2C_Handle i2c;
static I2C_Transaction i2cTransaction;

static UART2_Handle uart2;

static PIN_Handle pinHandle;

void set_ublox_callback(ublox_gps_callback callback)
{

    ublox_callback = callback;

}

void set_ublox_gps_gpio_instance(PIN_Handle pinHandleInstance)
{

    pinHandle = pinHandleInstance;

}

void set_ublox_gps_uart2_instance(UART2_Handle uart)
{

    uart2 = uart;

}

void set_ublox_gps_i2c_instance(I2C_Handle i2cInstance)
{

    i2c = i2cInstance;

    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.writeCount = sizeof(txBuffer);
    i2cTransaction.readCount = sizeof(rxBuffer);

}

static void wait_100_ms()
{

    Task_sleep(100 * 1000 / Clock_tickPeriod);

}

static void wait_100_ms_timer(uint16_t wait)
{

    for (uint16_t i = 0; i < wait; i++)
    {

        wait_100_ms();

    }

}

static uint8_t ublox_uart_data_send(char *data, uint16_t size, uint16_t wait)
{

    uint8_t ck_a = 0;
    uint8_t ck_b = 0;

    uint8_t data_size = 0;

    txSize = 0;
    rxSize = 0;

    memset(txBuffer, 0x00, sizeof(txBuffer));
    memset(rxBuffer, 0x00, sizeof(rxBuffer));

    for (uint8_t i = 0; i < size; i++)
    {

        txBuffer[i] = data[i];

        if (i >= 2 && i < size)
        {

            ck_a = ck_a + data[i];
            ck_b = ck_b + ck_a;

        }

    }

    data_size = size;

    ck_a = ck_a & 0xFF;
    txBuffer[data_size++] = ck_a;

    ck_b = ck_b & 0xFF;
    txBuffer[data_size++] = ck_b;

    ubx_step = 0;
    ublox_gps_data.ubx_ack = 0x00;

    UART2_write(uart2, txBuffer, data_size, NULL);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (ublox_gps_data.ubx_ack)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

static uint8_t ublox_i2c_data_send(char *data, uint8_t size)
{

    uint8_t ck_a = 0;
    uint8_t ck_b = 0;
    uint8_t data_size = 0;

    memset(txBuffer, 0x00, sizeof(txBuffer));
    memset(rxBuffer, 0x00, sizeof(rxBuffer));

    for (uint8_t i = 0; i < size; i++)
    {

        txBuffer[i] = data[i];

        if (i >= 2 && i < size)
        {

            ck_a = ck_a + data[i];
            ck_b = ck_b + ck_a;

        }

    }

    data_size = size;

    ck_a = ck_a & 0xFF;
    txBuffer[data_size++] = ck_a;

    ck_b = ck_b & 0xFF;
    txBuffer[data_size++] = ck_b;

    txSize = data_size;
    rxSize = UBX_I2C_ACK_SIZE + 2;

    i2cTransaction.slaveAddress = ZOE_M8Q_I2C_ADDRESS;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.writeCount = txSize;
    i2cTransaction.readCount = rxSize;

    return I2C_transfer(i2c, &i2cTransaction);

}

uint8_t set_ublox_i2c_nmea_off(void)
{

    uint8_t error = 0x01;

    error = error & ublox_i2c_data_send((char*) RMC_OFF, sizeof(RMC_OFF));

    error = error & ublox_i2c_data_send((char*) VTG_OFF, sizeof(VTG_OFF));

    error = error & ublox_i2c_data_send((char*) GSA_OFF, sizeof(GSA_OFF));

    error = error & ublox_i2c_data_send((char*) GSV_OFF, sizeof(GSV_OFF));

    error = error & ublox_i2c_data_send((char*) GLL_OFF, sizeof(GLL_OFF));

    error = error & ublox_i2c_data_send((char*) GGA_OFF, sizeof(GGA_OFF));

    return error;

}

uint8_t set_ublox_uart_nmea_off(void)
{

    uint8_t error = 0x01;

    error = error & ublox_uart_data_send((char*) RMC_OFF, sizeof(RMC_OFF), 10);

    error = error & ublox_uart_data_send((char*) VTG_OFF, sizeof(VTG_OFF), 10);

    error = error & ublox_uart_data_send((char*) GSA_OFF, sizeof(GSA_OFF), 10);

    error = error & ublox_uart_data_send((char*) GSV_OFF, sizeof(GSV_OFF), 10);

    error = error & ublox_uart_data_send((char*) GLL_OFF, sizeof(GLL_OFF), 10);

    error = error & ublox_uart_data_send((char*) GGA_OFF, sizeof(GGA_OFF), 10);

    return error;

}

uint8_t set_ublox_i2c_pvt_enable(void)
{

    uint8_t error = ublox_i2c_data_send((char*) NAV_PVT_ENABLE_1HZ,
                                        sizeof(NAV_PVT_ENABLE_1HZ));

    return error;

}

uint8_t set_ublox_uart_pvt_enable(void)
{

    uint8_t error = ublox_uart_data_send((char*) NAV_PVT_ENABLE_1HZ,
                                         sizeof(NAV_PVT_ENABLE_1HZ), 10);

    return error;

}

uint8_t set_ublox_i2c_pm2(void)
{

    uint8_t error = ublox_i2c_data_send((char*) UBX_PM2, sizeof(UBX_PM2));

    return error;

}

uint8_t set_ublox_uart_pm2(void)
{

    uint8_t error = ublox_uart_data_send((char*) UBX_PM2, sizeof(UBX_PM2), 10);

    return error;

}

uint8_t set_ublox_i2c_cfg_cfg_load(void)
{

    uint8_t error = ublox_i2c_data_send((char*) CFG_CFG_LOAD,
                                        sizeof(CFG_CFG_LOAD));

    return error;

}

uint8_t set_ublox_uart_cfg_cfg_load(void)
{

    uint8_t error = ublox_uart_data_send((char*) CFG_CFG_LOAD,
                                         sizeof(CFG_CFG_LOAD), 10);

    return error;

}

uint8_t set_ublox_i2c_cfg_cfg_save(void)
{

    uint8_t error = ublox_i2c_data_send((char*) CFG_CFG_SAVE,
                                        sizeof(CFG_CFG_SAVE));

    return error;

}

uint8_t set_ublox_uart_cfg_cfg_save(void)
{

    uint8_t error = ublox_uart_data_send((char*) CFG_CFG_SAVE,
                                         sizeof(CFG_CFG_SAVE), 10);

    return error;

}

uint8_t set_ublox_zoe_m8q_dcdc_enable(void)
{

    uint8_t error = ublox_i2c_data_send((char*) ZOE_M8Q_DCDC_ENABLE,
                                        sizeof(ZOE_M8Q_DCDC_ENABLE));

    wait_100_ms_timer(10);

    return error;

}

void set_ublox_gps_power_on(void)
{

    PIN_setOutputValue(pinHandle, GPIO_GPS_EN, 1);

    wait_100_ms_timer(10);

}

void set_ublox_gps_power_off(void)
{

    PIN_setOutputValue(pinHandle, GPIO_GPS_EN, 0);

    wait_100_ms_timer(10);

}

uint8_t set_ublox_exit_backup_mode(void)
{

    uint8_t error = false;

#ifdef GPS_INT_ENABLE



    PIN_setOutputValue(pinHandle, GPS_INT, 1);

    wait_ms(200);

    error = set_ublox_cfg_cfg_load();

    wait_ms(100);

#endif

    return error;

}

uint8_t set_ublox_backup_mode(void)
{

    uint8_t error = 0x01;

#ifdef GPS_INT_ENABLE



    error = error & set_ublox_cfg_cfg_save();

    error = error & set_ublox_pm2();

    wait_ms(100);

    PIN_setOutputValue(pinHandle, GPS_INT, 0);

#endif

    return error;

}

uint16_t get_ublox_i2c_read_buffer_size(void)
{

    memset(txBuffer, 0x00, sizeof(txBuffer));
    memset(rxBuffer, 0x00, sizeof(rxBuffer));

    txBuffer[0] = 0xFD;

    i2cTransaction.slaveAddress = ZOE_M8Q_I2C_ADDRESS;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readCount = 2;

    if (I2C_transfer(i2c, &i2cTransaction))
    {

        uint16_t read_buffer_size = (uint16_t) rxBuffer[0] << 8 | rxBuffer[1];

        return read_buffer_size;

    }

    return 0;

}

uint8_t get_ublox_i2c_data(uint16_t size)
{

    if (size > 0)
    {

        if (size < sizeof(rxBuffer))
        {

            memset(txBuffer, 0x00, sizeof(txBuffer));
            memset(rxBuffer, 0x00, sizeof(rxBuffer));

            txBuffer[0] = 0xFF;

            i2cTransaction.slaveAddress = ZOE_M8Q_I2C_ADDRESS;
            i2cTransaction.writeBuf = txBuffer;
            i2cTransaction.readBuf = rxBuffer;
            i2cTransaction.writeCount = 1;
            i2cTransaction.readCount = size;

            if (I2C_transfer(i2c, &i2cTransaction))
            {

                ubx_step = 0;

                for (uint16_t i = 0; i < size; i++)
                {

                    ublox_gps_ubx_input(rxBuffer[i]);

                }

                return 0x01;

            }

        }

    }

    return 0x00;

}

static void ubx_crc_update(uint8_t data)
{

    ubx_crc_a = ubx_crc_a + data;
    ubx_crc_b = ubx_crc_b + ubx_crc_a;

}

void ublox_gps_ubx_input(uint8_t data)
{

    if (ubx_step == 0)
    {

        if (data == SYNC_WORD_1)
        {

            ubx_step++;

        }

    }
    else if (ubx_step == 1)
    {

        if (data != SYNC_WORD_2)
        {

            ubx_step = 0x00;

        }
        else
        {

            ubx_crc_a = 0;
            ubx_crc_b = 0;

            ubx_step++;

        }

    }
    else if (ubx_step == 2)
    {

        ubx_message_class = data;

        ubx_crc_update(data);

        ubx_step++;

    }
    else if (ubx_step == 3)
    {

        ubx_message_id = data;

        ubx_crc_update(data);

        ubx_step++;

    }
    else if (ubx_step == 4)
    {

        ubx_payload_size = data;

        ubx_crc_update(data);

        ubx_step++;

    }
    else if (ubx_step == 5)
    {

        ubx_payload_size += (data << 8);

        ubx_crc_update(data);

        ubx_payload_index = 0;
        memset(ubx_payload, 0x00, sizeof(ubx_payload));

        ubx_step++;

    }
    else if (ubx_step == 6)
    {

        ubx_payload[ubx_payload_index++] = data;

        if (ubx_payload_index >= UBX_WRITE_READ_DATA_SIZE)
        {

            ubx_step = 0;

        }
        else if (ubx_payload_index <= ubx_payload_size)
        {

            ubx_crc_update(data);

        }
        else
        {

            if ((ubx_crc_a & 0xFF) == data)
            {

                ubx_step++;

            }
            else
            {

                ubx_step = 0;

            }

        }

    }
    else if (ubx_step == 7)
    {

        if ((ubx_crc_b & 0xFF) == data)
        {

            if (ubx_message_class == 0x05 && ubx_message_id == 0x01)
            {
                //ACK Output UBX
                ublox_gps_data.ubx_ack = 0x01;

            }
            else if (ubx_message_class == 0x05 && ubx_message_id == 0x00)
            {
                //ACK Output Error UBX
                ublox_gps_data.ubx_ack = 0x00;

            }
            else if (ubx_message_class == 0x13 && ubx_message_id == 0x60)
            {
                //ACK Output Error UBX
                ublox_gps_data.ubx_ack = 0x00;

            }
            else if (ubx_message_class == 0x01 && ubx_message_id == 0x07)
            {

                ublox_gps_data.year = (ubx_payload[5] << 8) + ubx_payload[4];

                ublox_gps_data.month = ubx_payload[6];

                ublox_gps_data.day = ubx_payload[7];

                ublox_gps_data.hour = ubx_payload[8];

                ublox_gps_data.minute = ubx_payload[9];

                ublox_gps_data.second = ubx_payload[10];

                ublox_gps_data.navigation_statue = ubx_payload[20];

                ublox_gps_data.sv_fix_count = ubx_payload[23];

                ublox_gps_data.lon = (ubx_payload[27] << 24)
                        + (ubx_payload[26] << 16) + (ubx_payload[25] << 8)
                        + ubx_payload[24];

                ublox_gps_data.lat = (ubx_payload[31] << 24)
                        + (ubx_payload[30] << 16) + (ubx_payload[29] << 8)
                        + ubx_payload[28];

                ublox_gps_data.height = (ubx_payload[35] << 24)
                        + (ubx_payload[34] << 16) + (ubx_payload[33] << 8)
                        + ubx_payload[32];
                ublox_gps_data.height /= 10;

                ublox_gps_data.hAcc = (ubx_payload[43] << 24)
                        + (ubx_payload[42] << 16) + (ubx_payload[41] << 8)
                        + ubx_payload[40];

                ublox_gps_data.vAcc = (ubx_payload[47] << 24)
                        + (ubx_payload[46] << 16) + (ubx_payload[45] << 8)
                        + ubx_payload[44];

            }

            if (ublox_callback)
            {

                ublox_callback(ublox_gps_data);

            }

        }

        ubx_step = 0;

    }

}
