
#ifndef QUECTEL_GPS_H_
#define QUECTEL_GPS_H_

#include <stdint.h>
#include "board_define.h"

#include <ti/drivers/UART2.h>
#include <ti/drivers/PIN.h>
#include "ti_drivers_config.h"

typedef struct
{

    int32_t latitude;
    int32_t longitude;
    int32_t height;

    uint8_t satellites_number;
    uint8_t navigation_statue;

    uint8_t HDOP;

    uint8_t ack;

    uint8_t gps_start_state;

} quectel_gps_data_t;

typedef void (*quectel_gps_callback)(quectel_gps_data_t data);

void set_quectel_gps_callback(quectel_gps_callback callback);

void set_quectel_gps_gpio_instance(PIN_Handle pinHandleInstance);

void set_quectel_gps_uart_instance(UART2_Handle uart);

uint8_t set_quectel_gps_power_on(uint16_t wait);

void set_quectel_gps_power_off();

uint8_t set_quectel_gps_sbas_disable(uint16_t wait);

uint8_t set_quectel_gps_nmea_off_gga_only(uint16_t wait);

void quectel_gps_nmea_input(char data);

#endif /* QUECTEL_GPS_H_ */
