#include "quectel_gps.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "quectel_define.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#define NEMA_DATA_RX_BUFFER_SIZE 255
#define NMEA_WRITE_READ_DATA_SIZE 255

static UART2_Handle uart2;
static PIN_Handle pinHandle;

static quectel_gps_data_t quectel_gps_data;
static quectel_gps_callback quectel_callback;

static uint8_t rxSize = 0;
static char rxBuffer[NMEA_WRITE_READ_DATA_SIZE] = { 0x00 };

static uint8_t nmeaDataRxSize = 0;
static char nemaDataRxBuffer[NEMA_DATA_RX_BUFFER_SIZE] = { 0x00 };

void set_quectel_gps_callback(quectel_gps_callback callback)
{

    quectel_callback = callback;

}

void set_quectel_gps_gpio_instance(PIN_Handle pinHandleInstance)
{

    pinHandle = pinHandleInstance;
    PIN_setOutputValue(pinHandle, GPIO_GPS_EN, 0);

}

void set_quectel_gps_uart_instance(UART2_Handle uart)
{

    uart2 = uart;

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

static uint8_t quectel_uart_data_send(char *data, uint16_t size, uint16_t wait)
{

    quectel_gps_data.ack = 0x00;

    UART2_write(uart2, data, size, NULL);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (quectel_gps_data.ack)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

uint8_t set_quectel_gps_power_on(uint16_t wait)
{

    quectel_gps_data.gps_start_state = 0x00;

    PIN_setOutputValue(pinHandle, GPIO_GPS_EN, 1);

    for (uint16_t i = 0; i < wait; i++)
    {

        if (quectel_gps_data.gps_start_state)
        {

            return 0x01;

        }

        wait_100_ms();

    }

    return 0x00;

}

void set_quectel_gps_power_off()
{

    quectel_gps_data.gps_start_state = 0x00;

    PIN_setOutputValue(pinHandle, GPIO_GPS_EN, 0);

    wait_100_ms_timer(10);

}

uint8_t set_quectel_gps_sbas_disable(uint16_t wait)
{

    return quectel_uart_data_send(PMTK_SBAS_DISABLE, sizeof(PMTK_SBAS_DISABLE),
                                  wait);

}

uint8_t set_quectel_gps_nmea_off_gga_only(uint16_t wait)
{

    return quectel_uart_data_send(PMTK_NEMA_OUTPUT_DISABLE_GGA_ONLY,
                                  sizeof(PMTK_NEMA_OUTPUT_DISABLE_GGA_ONLY),
                                  wait);

}

static uint8_t nmea_crc_update(char *data, uint16_t size)
{

    uint8_t crc = 0;
    char convert_crc[4];

    for (uint16_t i = 1; i < size - 3; i++)
    {

        crc ^= data[i];

    }

    sprintf(convert_crc, "%02X", crc);

    if (strncmp(convert_crc, data + (size - 2), 2) == 0)
    {

        return 0x01;

    }

    return 0x00;

}

void quectel_gps_nmea_input(char data)
{

    rxBuffer[rxSize++] = data;

    if (rxSize > 2)
    {

        if ((rxBuffer[rxSize - 1] == '\n') && (rxBuffer[rxSize - 2] == '\r'))
        {

            if (nmea_crc_update(rxBuffer, rxSize - 2))
            {

                if (strncmp("$GPGGA", (char*) rxBuffer, 5) == 0)
                {

                    uint8_t step = 0;

                    nmeaDataRxSize = 0;
                    memset(nemaDataRxBuffer, 0x00, sizeof(nemaDataRxBuffer));

                    for (uint16_t i = 0; i < rxSize; i++)
                    {

                        if (rxBuffer[i] == ',')
                        {

                            if (step == 2)
                            {

                                float lat = atof(nemaDataRxBuffer);
                                quectel_gps_data.latitude = lat * 100000;

                            }
                            else if (step == 3)
                            {

                                if (nemaDataRxBuffer[0] == 'S')
                                {

                                    quectel_gps_data.latitude *= -1;

                                }

                            }
                            else if (step == 4)
                            {

                                float lon = atof(nemaDataRxBuffer);
                                quectel_gps_data.longitude = lon * 100000;

                            }
                            else if (step == 5)
                            {

                                if (nemaDataRxBuffer[0] == 'W')
                                {

                                    quectel_gps_data.longitude *= -1;

                                }

                            }
                            else if (step == 6)
                            {

                                quectel_gps_data.navigation_statue = atoi(
                                        nemaDataRxBuffer);

                            }
                            else if (step == 7)
                            {

                                quectel_gps_data.satellites_number = atoi(
                                        nemaDataRxBuffer);

                            }
                            else if (step == 8)
                            {

                                float HDOP = atof(nemaDataRxBuffer);
                                HDOP *= 10;
                                quectel_gps_data.HDOP = (uint8_t) HDOP;

                            }
                            else if (step == 9)
                            {

                                quectel_gps_data.height = atof(
                                        nemaDataRxBuffer);

                            }
                            else if (step >= 10)
                            {

                                if (quectel_callback)
                                {

                                    quectel_gps_data.gps_start_state = 0x01;

                                    quectel_callback(quectel_gps_data);

                                }

                                break;

                            }

                            nmeaDataRxSize = 0;
                            memset(nemaDataRxBuffer, 0x00,
                                   sizeof(nemaDataRxBuffer));

                            step++;

                        }
                        else
                        {

                            nemaDataRxBuffer[nmeaDataRxSize++] = rxBuffer[i];

                            if (nmeaDataRxSize >= sizeof(nemaDataRxBuffer))
                            {

                                nmeaDataRxSize = 0;

                            }

                        }

                    }

                }
                else if (strncmp("$PMTK001,314,3", (char*) rxBuffer, 14) == 0)
                {

                    quectel_gps_data.ack = 0x01;

                    if (quectel_callback)
                    {

                        quectel_callback(quectel_gps_data);

                    }

                }

            }

            rxSize = 0;

        }

    }

    if (rxSize >= sizeof(rxBuffer))
    {

        rxSize = 0;

    }

}
