
#ifndef SI7051_H_
#define SI7051_H_

#include <stdint.h>
#include "board_define.h"

#include <ti/drivers/I2C.h>
#include "ti_drivers_config.h"

void set_si7051_i2c_instance(I2C_Handle i2cInstance);

uint8_t init_si7051(void);

uint8_t get_si7051_temperature(int16_t *temperature_output);

#endif /* SI7051_H_ */
