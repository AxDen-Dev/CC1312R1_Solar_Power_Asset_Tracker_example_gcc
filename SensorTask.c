#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/NVS.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/Watchdog.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)

#include <ti/devices/cc13x2_cc26x2/driverlib/aux_adc.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* Application Header files */
#include "SensorTask.h"
#include "Protocol.h"

#include "board_define.h"

#include "quectel_gps.h"
#include "ublox_gps.h"

#include "bg96.h"
#include "sara_u2.h"

#include "si7051.h"

#define SENSOR_TASK_STACK_SIZE 2048
#define SENSOR_TASK_TASK_PRIORITY   3

#define SENSOR_TASK_EVENT_ALL                         0xFFFFFFFF
#define SENSOR_TASK_SENSOR_UPDATE    (uint32_t)(1 << 0)
#define SENSOR_TASK_GPS_UPDATE    (uint32_t)(2 << 0)

#define BMA400_ADDRESS 0x14
#define HDC1080_ADDRESS 0x40
#define PCA9685_ADDRESS 0x40
#define MAX30102_ADDRESS 0x57
#define CCS811_ADDRESS 0x5B

#define CELLUAR_SOCKET_READ_DATA_SIZE 512

#define TIMER_TIMEOUT 1000
#define WATCHDOG_TIMEOUT_MS 10000

#define BUTTON_PRESS_TIMER_COUNT 5
#define BUTTON_RPESS_COUNT 3

#define BMA400_SCALE 0.9765625

PIN_Config pinTable[] = {

GPIO_LED_0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,

GPIO_LED_1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL
| PIN_DRVSTR_MAX,

GPIO_BAT_EN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL
| PIN_DRVSTR_MAX,

GPIO_CELLUAR_EN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL
| PIN_DRVSTR_MAX,

GPIO_CELLUAR_PWR | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL
| PIN_DRVSTR_MAX,

GPIO_GPS_EN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL
| PIN_DRVSTR_MAX,

PIN_TERMINATE

};

PIN_Config buttonTable[] = {

GPIO_BTN | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,

                             PIN_TERMINATE

};

static Task_Params sensorTaskParams;
Task_Struct sensorTask; /* not static so you can see in ROV */
static uint8_t sensorTaskStack[SENSOR_TASK_STACK_SIZE];

Event_Struct sensorEvent; /* not static so you can see in ROV */
static Event_Handle sensorEventHandle;

Clock_Struct sensorTimerClock;
Watchdog_Handle watchdogHandle;

static PacketSendRequestCallback packetSendRequestCallback;

static char uart_read_buf = 0x00;
static char uart2_read_buf = 0x00;

static uint16_t tcp_socket_buffer_size;
static uint8_t tcp_socket_buffer[CELLUAR_SOCKET_READ_DATA_SIZE];

static PIN_State pinState;
static PIN_Handle pinHandle;

static PIN_State buttonState;
static PIN_Handle buttonHandle;

static I2C_Handle i2c;
static I2C_Params i2cParams;

static UART_Handle uart;
static UART2_Handle uart2;

static uint8_t init_state = 0x00;
static int16_t temperature = 0;
static int16_t ax = 0;
static int16_t ay = 0;
static int16_t az = 0;
static uint8_t battery_voltage = 0;
static int32_t lat = 0;
static int32_t lon = 0;

static uint8_t transmission_mode = 0x01;

static uint8_t button_press_state = 0x00;
static uint8_t button_press_count = 0;
static int8_t button_press_timer_count = 0;

static uint8_t gps_data_update_state = 0x00;
static quectel_gps_data_t quectel_gps_data;
static ublox_gps_data_t ublox_gps_data;

static sara_u2_data_t sara_u2_data;
static bg96_data_t bg96_data;

static uint8_t sensor_task_release_state = 0x00;

static uint8_t packet_payload_length = 0;
static uint8_t payload_buffer_size = 0;
static uint8_t payload_buffer[115] = { 0x00 };

extern uint8_t radio_init;
extern uint8_t mac_address[8];
extern uint32_t collection_cycle_timeout_count;
extern uint32_t collection_cycle_timer_count;
extern radio_packet_protocol_t radio_packet_protocol;

static void uartReadCallback(UART_Handle handle, void *rxBuf, size_t size);

void uart2ReadCallback(UART2_Handle handle, void *rxBuf, size_t size,
                       void *userArg, int_fast16_t status);

void scCtrlReadyCallback(void)
{

}

void scTaskAlertCallback(void)
{

}

void watchdogCallback(uintptr_t watchdogHandle)
{

    while (1)
    {

    }

}

static void buttonCallback(PIN_Handle handle, PIN_Id pinId)
{

    if (init_state)
    {

        button_press_timer_count = BUTTON_PRESS_TIMER_COUNT;

        PIN_setOutputValue(pinHandle, GPIO_LED_1, 1);
        button_press_state = 0x01;

    }

}

static void sensorTimerClockCallBack(UArg arg0)
{

    if (init_state)
    {

        collection_cycle_timeout_count++;

        if (collection_cycle_timeout_count >= collection_cycle_timer_count)
        {

            if (sensor_task_release_state == 0x00)
            {

                sensor_task_release_state = 0x01;
                Event_post(sensorEventHandle, SENSOR_TASK_SENSOR_UPDATE);

            }

            collection_cycle_timeout_count = 0;

        }

        if (collection_cycle_timer_count - GPS_UPDATE_TIME
                == collection_cycle_timeout_count)
        {

            if (gps_data_update_state == 0x00
                    && sensor_task_release_state == 0x00)
            {

                gps_data_update_state = 0x01;
                sensor_task_release_state = 0x01;

                Event_post(sensorEventHandle, SENSOR_TASK_GPS_UPDATE);

            }

        }

        if (button_press_state)
        {

            PIN_setOutputValue(pinHandle, GPIO_LED_1, 0);
            button_press_state = 0x00;

            button_press_count++;

        }

        button_press_timer_count -= 1;

        if (button_press_timer_count > 0)
        {

            if (button_press_count >= BUTTON_RPESS_COUNT)
            {

                if (transmission_mode == 0x00)
                {

                    transmission_mode = 0x01;

                }
                else
                {

                    transmission_mode = 0x00;

                }

                collection_cycle_timer_count = 0;

                PIN_setOutputValue(pinHandle, GPIO_LED_0, 1);

                button_press_timer_count = 0;
                button_press_count = 0;

            }

        }
        else
        {

            PIN_setOutputValue(pinHandle, GPIO_LED_0, 0);

            button_press_count = 0;

        }

    }

    Watchdog_clear(watchdogHandle);

}

static void wait_ms(uint32_t wait)
{

    Task_sleep(wait * 1000 / Clock_tickPeriod);

}

static void initErrorUpdate(void)
{

    for (uint8_t i = 0; i < 4; i++)
    {

        PIN_setOutputValue(pinHandle, GPIO_LED_0,
                           !PIN_getOutputValue(GPIO_LED_0));
        PIN_setOutputValue(pinHandle, GPIO_LED_1,
                           !PIN_getOutputValue(GPIO_LED_1));

        wait_ms(500);

    }

    PIN_setOutputValue(pinHandle, GPIO_LED_0, 1);
    PIN_setOutputValue(pinHandle, GPIO_LED_1, 1);

}

static bool si7051_temperature_read(int16_t *temperature_output)
{

    uint8_t i2c_state = 0x00;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);

    set_si7051_i2c_instance(i2c);

    i2c_state = i2c_state & get_si7051_temperature(temperature_output);

    I2C_close(i2c);

    return i2c_state;

}

static bool bma400_acceleration_read(int16_t *ax, int16_t *ay, int16_t *az)
{

    I2C_Transaction i2cTransaction;

    bool i2c_state = true;
    uint8_t txBuffer[2] = { 0x00 };
    uint8_t rxBuffer[6] = { 0x00 };

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);

    if (i2c != NULL)
    {
        i2cTransaction.writeBuf = txBuffer;
        i2cTransaction.readBuf = rxBuffer;

        i2cTransaction.slaveAddress = BMA400_ADDRESS;

        txBuffer[0] = 0x04;
        i2cTransaction.writeCount = 1;
        i2cTransaction.readCount = 6;
        i2c_state = i2c_state & I2C_transfer(i2c, &i2cTransaction);

        int16_t accel = (rxBuffer[1] << 8) | rxBuffer[0];

        if (accel > 2047)
        {

            accel -= 4096;

        }

        *ax = (int16_t) (accel * BMA400_SCALE);

        accel = (rxBuffer[3] << 8) | rxBuffer[2];

        if (accel > 2047)
        {

            accel -= 4096;

        }

        *ay = (int16_t) (accel * BMA400_SCALE);

        accel = (rxBuffer[5] << 8) | rxBuffer[4];

        if (accel > 2047)
        {

            accel -= 4096;

        }

        *az = (int16_t) (accel * BMA400_SCALE);

        I2C_close(i2c);

    }

    return i2c_state;

}

static uint8_t battery_value_read()
{

    ADC_Handle adc;
    ADC_Params ADCParams;
    ADC_Params_init(&ADCParams);

    adc = ADC_open(CONFIG_I2C_0, &ADCParams);

    if (adc != NULL)
    {

        uint16_t adc_value;

        PIN_setOutputValue(pinHandle, GPIO_BAT_EN, 1);

        wait_ms(20);

        int_fast16_t result = ADC_convert(adc, &adc_value);

        ADC_close(adc);

        PIN_setOutputValue(pinHandle, GPIO_BAT_EN, 0);

        if (result == ADC_STATUS_SUCCESS)
        {

            uint32_t microVolt = ADC_convertRawToMicroVolts(adc, adc_value);
            microVolt *= 3;
            microVolt /= 100000;

            return microVolt;

        }

    }

    return 0;

}

static void initSensor(void)
{

    I2C_Transaction i2cTransaction;

    bool i2c_state = true;
    uint8_t txBuffer[2] = { 0x00 };
    uint8_t rxBuffer[2] = { 0x00 };

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);

    if (i2c == NULL)
    {

        initErrorUpdate();

        SysCtrlSystemReset();

    }

    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;

    i2cTransaction.slaveAddress = BMA400_ADDRESS;

    txBuffer[0] = 0x7E;
    txBuffer[1] = 0xB6;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readCount = 0;
    i2c_state = i2c_state & I2C_transfer(i2c, &i2cTransaction);

    wait_ms(500);

    txBuffer[0] = 0x1A;
    txBuffer[1] = 0b00000101;
    i2c_state = i2c_state & I2C_transfer(i2c, &i2cTransaction);

    txBuffer[0] = 0x19;
    txBuffer[1] = 0b00000010;
    i2c_state = i2c_state & I2C_transfer(i2c, &i2cTransaction);

    wait_ms(100);

    set_si7051_i2c_instance(i2c);

    i2c_state = i2c_state & init_si7051();

    wait_ms(100);

    if (!i2c_state)
    {

        initErrorUpdate();

    }

    I2C_close(i2c);

}

void celluar_callback_bg96(bg96_data_t data)
{

    bg96_data = data;

}

void celluar_callback_sara_u2(sara_u2_data_t data)
{

    sara_u2_data = data;

}

static void init_Celluar()
{

    uint8_t error = 0x00;

    UART_Params uartParams;

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = uartReadCallback;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);

#ifdef BG96

    set_bg96_callback(celluar_callback_bg96);

    set_bg96_gpio_instance(pinHandle);

    set_bg96_uart_instance(uart);

    UART_read(uart, &uart_read_buf, 1);

    if (set_bg96_power_on(3, 10))
    {

        if (bg96_init_config_setup())
        {

            if (bg96_init_network_setup())
            {

                error = 0x01;

            }

        }

    }

    set_bg96_power_off(1, 10);

    if (!error)
    {

        initErrorUpdate();

    }

    UART_close(uart);

#endif

#ifdef SARA_U2

    set_sara_callback(celluar_callback_sara_u2);

    set_sara_u2_gpio_instance(pinHandle);

    set_sara_u2_uart_instance(uart);

    UART_read(uart, &uart_read_buf, 1);

    if (set_sara_power_on(3, 10))
    {

        if (sara_u2_init_config_setup())
        {

            if (sara_u2_init_network_setup())
            {

                error = 0x01;

            }

        }

    }

    set_sara_power_off(1, 10);

    if (!error)
    {

        initErrorUpdate();

    }

    UART_close(uart);

#endif

}

static void update_setup_command()
{

    if (tcp_socket_buffer_size == 5)
    {

        uint8_t control_number = tcp_socket_buffer[0];

        if (radio_packet_protocol.Packet.control_number != control_number)
        {

            uint32_t collection_cycle = tcp_socket_buffer[1] << 24;
            collection_cycle |= tcp_socket_buffer[2] << 16;
            collection_cycle |= tcp_socket_buffer[3] << 8;
            collection_cycle |= tcp_socket_buffer[4];

            if (collection_cycle >= COLLECTION_CYCLE_TIMEOUT)
            {

                collection_cycle_timeout_count = 0;
                collection_cycle_timer_count = collection_cycle;

                radio_packet_protocol.Packet.control_number = control_number;

            }

        }

    }

}

static void update_Celluar()
{

    UART_Params uartParams;

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = uartReadCallback;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);

#ifdef BG96

    set_bg96_uart_instance(uart);

    UART_read(uart, &uart_read_buf, 1);

    if (set_bg96_power_on(3, 10))
    {

        if (bg96_init_config_setup())
        {

            if (bg96_init_network_setup())
            {

                if (bg96_tcp_data_upload(radio_packet_protocol.buffer,
                                         packet_payload_length))
                {

                    tcp_socket_buffer_size = 0;
                    memset(tcp_socket_buffer, 0x00, sizeof(tcp_socket_buffer));

                    tcp_socket_buffer_size =
                            get_bg96_socket_recevice_buffer_data(
                                    tcp_socket_buffer);

                    update_setup_command();

                }

            }

        }

    }

    set_bg96_power_off(1, 10);

#endif

#ifdef SARA_U2

    set_sara_u2_uart_instance(uart);

    UART_read(uart, &uart_read_buf, 1);

    if (set_sara_power_on(3, 10))
    {

        if (sara_u2_init_config_setup())
        {

            if (sara_u2_init_network_setup())
            {

                if (sara_u2_tcp_data_upload(radio_packet_protocol.buffer,
                                            packet_payload_length))
                {

                    tcp_socket_buffer_size = 0;
                    memset(tcp_socket_buffer, 0x00, sizeof(tcp_socket_buffer));

                    tcp_socket_buffer_size =
                            get_sarau2_socket_recevice_buffer_datat(
                                    tcp_socket_buffer);

                    update_setup_command();

                }

            }

        }

    }

    set_sara_power_off(1, 10);

#endif

    UART_close(uart);

}

void gps_callback_quectel(quectel_gps_data_t data)
{

    quectel_gps_data = data;

}

void gps_callback_ublox(ublox_gps_data_t data)
{

    ublox_gps_data = data;

}

static void init_GPS()
{

    UART2_Params uart2Params;

    UART2_Params_init(&uart2Params);
    uart2Params.readMode = UART2_Mode_CALLBACK;
    uart2Params.readCallback = uart2ReadCallback;
    uart2Params.baudRate = 9600;

    uart2 = UART2_open(CONFIG_UART2_0, &uart2Params);

#ifdef QUECTEL_GPS

    set_quectel_gps_callback(gps_callback_quectel);

    set_quectel_gps_gpio_instance(pinHandle);

    set_quectel_gps_uart_instance(uart2);

    UART2_read(uart2, &uart2_read_buf, 1, NULL);

    if (set_quectel_gps_power_on(10))
    {

        set_quectel_gps_nmea_off_gga_only(10);

        set_quectel_gps_sbas_disable(10);

    }

    set_quectel_gps_power_off();

#endif

#ifdef UBLOX_GPS

    set_ublox_callback(gps_callback_ublox);

    set_ublox_gps_gpio_instance(pinHandle);

    set_ublox_gps_uart2_instance(uart2);

    UART2_read(uart2, &uart2_read_buf, 1, NULL);

    set_ublox_gps_power_on();

    set_ublox_uart_nmea_off();

    set_ublox_uart_pvt_enable();

    set_ublox_gps_power_off();

#endif

    UART2_close(uart2);

}

static void update_GPS()
{

    UART2_Params uart2Params;

    UART2_Params_init(&uart2Params);
    uart2Params.readMode = UART2_Mode_CALLBACK;
    uart2Params.readCallback = uart2ReadCallback;
    uart2Params.baudRate = 9600;

    uart2 = UART2_open(CONFIG_UART2_0, &uart2Params);

    lat = 0;
    lon = 0;

#ifdef QUECTEL_GPS

    set_quectel_gps_uart_instance(uart2);

    UART2_read(uart2, &uart2_read_buf, 1, NULL);

    if (set_quectel_gps_power_on(10))
    {

        set_quectel_gps_nmea_off_gga_only(10);

        set_quectel_gps_sbas_disable(10);

        for (uint8_t i = 0; i < 90; i++)
        {

            lat = quectel_gps_data.latitude;
            lon = quectel_gps_data.longitude;

            if (quectel_gps_data.navigation_statue > 0
                    && quectel_gps_data.HDOP < 40)
            {

                break;

            }

            wait_ms(1000);

        }

        set_quectel_gps_power_off();

    }

#endif

#ifdef UBLOX_GPS

    set_ublox_gps_uart2_instance(uart2);

    UART2_read(uart2, &uart2_read_buf, 1, NULL);

    set_ublox_gps_power_on();

    set_ublox_uart_nmea_off();

    set_ublox_uart_pvt_enable();

    for (uint8_t i = 0; i < 60; i++)
    {

        lat = ublox_gps_data.lat;
        lon = ublox_gps_data.lon;

        if (ublox_gps_data.navigation_statue >= 2
                && ublox_gps_data.hAcc < 200000)
        {

            break;

        }

    }

    set_ublox_gps_power_off();

#endif

    UART2_close(uart2);

}

static void sensorTaskFunction(UArg arg0, UArg arg1)
{

    ADC_init();

    I2C_init();

    UART_init();

    pinHandle = PIN_open(&pinState, pinTable);

    if (!pinHandle)
    {

        SysCtrlSystemReset();

    }

    PIN_setOutputValue(pinHandle, GPIO_LED_0, 1);
    PIN_setOutputValue(pinHandle, GPIO_LED_1, 1);
    PIN_setOutputValue(pinHandle, GPIO_BAT_EN, 0);
    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_EN, 0);
    PIN_setOutputValue(pinHandle, GPIO_CELLUAR_PWR, 0);
    PIN_setOutputValue(pinHandle, GPIO_GPS_EN, 0);

    buttonHandle = PIN_open(&buttonState, buttonTable);

    if (!buttonHandle)
    {

        SysCtrlSystemReset();

    }

    PIN_registerIntCb(buttonHandle, &buttonCallback);

    while (radio_init == 0x00)
    {

        wait_ms(100);

    }

    battery_voltage = battery_value_read();

    initSensor();

    init_GPS();

    init_Celluar();

    PIN_setOutputValue(pinHandle, GPIO_LED_0, 0);
    PIN_setOutputValue(pinHandle, GPIO_LED_1, 0);

    wait_ms(100);

    init_state = 0x01;

    while (1)
    {

        uint32_t events = Event_pend(sensorEventHandle, 0,
        SENSOR_TASK_EVENT_ALL,
                                     BIOS_WAIT_FOREVER);

        if (events == SENSOR_TASK_SENSOR_UPDATE)
        {

            si7051_temperature_read(&temperature);

            bma400_acceleration_read(&ax, &ay, &az);

            battery_voltage = battery_value_read();

            payload_buffer_size = 0;
            memset(payload_buffer, 0x00, sizeof(payload_buffer));

            payload_buffer_size = sprintf((char*) payload_buffer, "%d.%d,",
                                          (battery_voltage / 10),
                                          (battery_voltage % 10));

            payload_buffer_size += sprintf(
                    (char*) payload_buffer + payload_buffer_size, "%d.%d,",
                    (temperature / 10), abs(temperature % 10));

            payload_buffer_size += sprintf(
                    (char*) payload_buffer + payload_buffer_size, "%d,", ax);

            payload_buffer_size += sprintf(
                    (char*) payload_buffer + payload_buffer_size, "%d,", ay);

            payload_buffer_size += sprintf(
                    (char*) payload_buffer + payload_buffer_size, "%d,", az);

            payload_buffer_size += sprintf(
                    (char*) payload_buffer + payload_buffer_size, "%ld,", lat);

            payload_buffer_size += sprintf(
                    (char*) payload_buffer + payload_buffer_size, "%ld", lon);

            if (transmission_mode == 0x00)
            {

                if (packetSendRequestCallback)
                {

                    packetSendRequestCallback(payload_buffer,
                                              payload_buffer_size);

                }

            }
            else
            {

                memset(radio_packet_protocol.Packet.payload, 0x00,
                       sizeof(radio_packet_protocol.Packet.payload));

                packet_payload_length = PACKET_HEADER_SIZE;
                packet_payload_length += payload_buffer_size;
                memcpy(radio_packet_protocol.Packet.payload, payload_buffer,
                       payload_buffer_size);

                update_Celluar();

            }

        }
        else if (events == SENSOR_TASK_GPS_UPDATE)
        {

            update_GPS();

        }

        sensor_task_release_state = 0x00;

    }

}

void SensorTask_init(void)
{

    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&sensorEvent, &eventParam);
    sensorEventHandle = Event_handle(&sensorEvent);

    Task_Params_init(&sensorTaskParams);
    sensorTaskParams.stackSize = SENSOR_TASK_STACK_SIZE;
    sensorTaskParams.priority = SENSOR_TASK_TASK_PRIORITY;
    sensorTaskParams.stack = &sensorTaskStack;
    Task_construct(&sensorTask, sensorTaskFunction, &sensorTaskParams, NULL);

    Watchdog_init();

    Watchdog_Params watchdogParams;
    Watchdog_Params_init(&watchdogParams);
    watchdogParams.callbackFxn = (Watchdog_Callback) watchdogCallback;
    watchdogParams.debugStallMode = Watchdog_DEBUG_STALL_ON;
    watchdogParams.resetMode = Watchdog_RESET_ON;

    watchdogHandle = Watchdog_open(CONFIG_WATCHDOG_0, &watchdogParams);

    if (watchdogHandle == NULL)
    {

        SysCtrlSystemReset();

    }

    uint32_t reloadValue = Watchdog_convertMsToTicks(watchdogHandle,
    WATCHDOG_TIMEOUT_MS);

    if (reloadValue != 0)
    {

        Watchdog_setReload(watchdogHandle, reloadValue);

    }

    Clock_Params clockParams;
    Clock_Params_init(&clockParams);
    clockParams.period = TIMER_TIMEOUT * 1000 / Clock_tickPeriod;
    clockParams.startFlag = TRUE;
    Clock_construct(&sensorTimerClock, sensorTimerClockCallBack,
    TIMER_TIMEOUT * 1000 / Clock_tickPeriod,
                    &clockParams);

}

void SensorTask_registerPacketSendRequestCallback(
        PacketSendRequestCallback callback)
{

    packetSendRequestCallback = callback;

}

static void uartReadCallback(UART_Handle handle, void *rxBuf, size_t size)
{

    char *data = (char*) rxBuf;

#ifdef BG96

    bg96_uart_input(data[0]);

    UART_read(handle, &uart_read_buf, 1);

#endif

#ifdef SARA_U2

    sara_u2_uart_input(data[0]);

    UART_read(handle, &uart_read_buf, 1);

#endif

}

void uart2ReadCallback(UART2_Handle handle, void *rxBuf, size_t size,
                       void *userArg, int_fast16_t status)
{
    if (status == UART2_STATUS_SUCCESS)
    {

#ifdef QUECTEL_GPS

        char *data = (char*) rxBuf;
        quectel_gps_nmea_input(data[0]);

#endif

#ifdef UBLOX_GPS

        uint8_t *data = (uint8_t*) rxBuf;
        ublox_gps_ubx_input(data[0]);

#endif

        UART2_read(handle, &uart2_read_buf, 1, NULL);

    }

}

