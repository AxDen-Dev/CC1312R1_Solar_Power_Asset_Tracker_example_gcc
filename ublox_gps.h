#ifndef UBLOX_GPS_H_
#define UBLOX_GPS_H_

#include <stdint.h>
#include <stdbool.h>
#include "board_define.h"

#include <ti/drivers/I2C.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/PIN.h>
#include "ti_drivers_config.h"

typedef struct
{

    int32_t lat;
    int32_t lon;
    int32_t height;
    uint32_t hAcc;
    uint32_t vAcc;

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t sv_fix_count;

    uint8_t aop_status;

    uint8_t navigation_statue;

    uint8_t ubx_ack;

} ublox_gps_data_t;

typedef void (*ublox_gps_callback)(ublox_gps_data_t ublox_gps_data);

void set_ublox_callback(ublox_gps_callback callback);

void set_ublox_gps_gpio_instance(PIN_Handle pinHandleInstance);

void set_ublox_gps_uart2_instance(UART2_Handle uart);

void set_ublox_gps_i2c_instance(I2C_Handle i2cInstance);

void set_ublox_gps_power_on(void);

void set_ublox_gps_power_off(void);

uint8_t set_ublox_exit_backup_mode(void);

uint8_t set_ublox_backup_mode(void);

uint8_t set_ublox_i2c_nmea_off(void);

uint8_t set_ublox_uart_nmea_off(void);

uint8_t set_ublox_i2c_pvt_enable(void);

uint8_t set_ublox_uart_pvt_enable(void);

uint8_t set_ublox_i2c_pm2(void);

uint8_t set_ublox_uart_pm2(void);

uint8_t set_ublox_i2c_cfg_cfg_load(void);

uint8_t set_ublox_uart_cfg_cfg_load(void);

uint8_t set_ublox_i2c_cfg_cfg_save(void);

uint8_t set_ublox_uart_cfg_cfg_save(void);

uint8_t set_ublox_zoe_m8q_dcdc_enable(void);

uint16_t get_ublox_i2c_read_buffer_size(void);

uint8_t get_ublox_i2c_data(uint16_t size);

void ublox_gps_ubx_input(uint8_t data);

#endif /* UBLOX_GPS_H_ */
