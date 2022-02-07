
#include "si7051.h"
#include "string.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#define Si7051_ADDRESS 0x40

static I2C_Handle i2c;
static I2C_Transaction i2cTransaction;

static void wait_100_ms()
{

    Task_sleep(100 * 1000 / Clock_tickPeriod);

}

void set_si7051_i2c_instance(I2C_Handle i2cInstance)
{

    i2c = i2cInstance;

}

static uint8_t i2c_write8(uint8_t address, uint8_t reg)
{

    uint8_t i2c_state = 0x00;
    uint8_t txBuffer = reg;

    if (i2c != NULL)
    {

        i2cTransaction.writeBuf = &txBuffer;
        i2cTransaction.readBuf = NULL;
        i2cTransaction.slaveAddress = address;

        i2cTransaction.writeCount = 1;
        i2cTransaction.readCount = 0;
        i2c_state = I2C_transfer(i2c, &i2cTransaction);

    }

    return i2c_state;

}

static uint8_t i2c_read16(uint8_t address, uint8_t *read_value)
{

    uint8_t i2c_state = 0x00;
    uint8_t rxBuffer[2] = { 0x00 };

    if (i2c != NULL)
    {

        i2cTransaction.writeBuf = NULL;
        i2cTransaction.readBuf = rxBuffer;
        i2cTransaction.slaveAddress = address;

        i2cTransaction.writeCount = 0;
        i2cTransaction.readCount = 2;
        i2c_state = I2C_transfer(i2c, &i2cTransaction);

        memcpy(read_value, rxBuffer, 2);

    }

    return i2c_state;

}

uint8_t init_si7051(void)
{

    return i2c_write8(Si7051_ADDRESS, 0xFE);

}

uint8_t get_si7051_temperature(int16_t *temperature_output)
{

    uint8_t i2c_state = 0x00;
    uint8_t read_buffer[2] = { 0x00 };

    i2c_state = i2c_write8(Si7051_ADDRESS, 0xF3);

    for (uint8_t i = 0; i < 2; i++)
    {

        wait_100_ms();

    }

    i2c_state = i2c_state & i2c_read16(Si7051_ADDRESS, read_buffer);

    uint16_t temperature = (read_buffer[0] << 8) + read_buffer[1];
    float temperature_si7051 =
            ((((float) temperature * 175.72) / 65536) - 46.85);
    *temperature_output = (int16_t) (temperature_si7051 * 10);

    return i2c_state;

}

