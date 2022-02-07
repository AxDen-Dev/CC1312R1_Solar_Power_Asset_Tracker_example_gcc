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

/* Drivers */
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/power/PowerCC26XX.h>

/* Board Header files */
#include "ti_drivers_config.h"

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

#include "RadioTask.h"
#include "SensorTask.h"
#include "Protocol.h"

#define RADIO_TASK_STACK_SIZE 1024
#define RADIO_TASK_TASK_PRIORITY   3

#define RADIO_TASK_EVENT_ALL                  0xFFFFFFFF
#define RADIO_TASK_EVENT_ACK (uint32_t)(1 << 1)
#define RADIO_TASK_EVENT_ACK_TIMEOUT (uint32_t)(1 << 2)

#define ACK_TIME_OUT 160

#define RECV_ERROR_MAX_COUNT 3

const uint8_t addressFilterValue[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x0F};

const int8_t TX_POWER = 10;

static Task_Params radioTaskParams;
Task_Struct radioTask; /* not static so you can see in ROV */
static uint8_t radioTaskStack[RADIO_TASK_STACK_SIZE];

Event_Struct radioEvent; /* not static so you can see in ROV */
static Event_Handle radioEventHandle;

Semaphore_Struct radioAccessSem; /* not static so you can see in ROV */
static Semaphore_Handle radioAccessSemHandle;

static uint8_t packet_payload_length = 0;
static EasyLink_TxPacket txPacket;

extern uint8_t radio_init;
extern uint8_t mac_address[8];
extern uint32_t collection_cycle_timeout_count;
extern uint32_t collection_cycle_timer_count;
extern radio_packet_protocol_t radio_packet_protocol;

static void sendPacketCallback(uint8_t *payload, uint8_t payload_size)
{

    memset(radio_packet_protocol.Packet.payload, 0x00,
           sizeof(radio_packet_protocol.Packet.payload));

    packet_payload_length = PACKET_HEADER_SIZE;
    packet_payload_length += payload_size;
    memcpy(radio_packet_protocol.Packet.payload, payload, payload_size);

    Semaphore_post(radioAccessSemHandle);

}

static void rxDoneCallback(EasyLink_RxPacket *rxPacket, EasyLink_Status status)
{

    if (status == EasyLink_Status_Success)
    {

        if (rxPacket->len == 5)
        {

            uint8_t control_number = rxPacket->payload[0];

            if (radio_packet_protocol.Packet.control_number != control_number)
            {

                uint32_t collection_cycle = rxPacket->payload[1] << 24;
                collection_cycle |= rxPacket->payload[2] << 16;
                collection_cycle |= rxPacket->payload[3] << 8;
                collection_cycle |= rxPacket->payload[4];

                if (collection_cycle >= COLLECTION_CYCLE_TIMEOUT)
                {

                    collection_cycle_timeout_count = 0;
                    collection_cycle_timer_count = collection_cycle;

                    radio_packet_protocol.Packet.control_number =
                            control_number;

                }

            }

        }

        Event_post(radioEventHandle, RADIO_TASK_EVENT_ACK);

    }
    else
    {

        Event_post(radioEventHandle, RADIO_TASK_EVENT_ACK_TIMEOUT);

    }

}

static void radio_sensor_data_packet_send(radio_packet_protocol_t radio_packet,
                                          uint8_t radio_packet_length)
{

    txPacket.len = 0;
    txPacket.absTime = 0;
    memset(txPacket.dstAddr, 0x00, sizeof(txPacket.dstAddr));
    memset(txPacket.payload, 0x00, sizeof(txPacket.payload));

    memcpy(txPacket.dstAddr, addressFilterValue, 8);

    memcpy(txPacket.payload, radio_packet.buffer, radio_packet_length);
    txPacket.len = radio_packet_length;

    EasyLink_setCtrl(EasyLink_Ctrl_AsyncRx_TimeOut,
                     EasyLink_ms_To_RadioTime(ACK_TIME_OUT));

    if (EasyLink_transmit(&txPacket) != EasyLink_Status_Success)
    {

        SysCtrlSystemReset();

    }

    if (EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success)
    {

        SysCtrlSystemReset();

    }

}

static void radioTaskFunction(UArg arg0, UArg arg1)
{

    uint8_t recv_error_count = 0;

    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);
    easyLink_params.ui32ModType = EasyLink_Phy_5kbpsSlLr;

    if (EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
    {

        SysCtrlSystemReset();

    }

    EasyLink_getIeeeAddr(mac_address);

    EasyLink_setFrequency(920000000);

    EasyLink_enableRxAddrFilter(mac_address, 8, 1);

    radio_packet_protocol.Packet.company_id[0] = COMPANY_ID >> 8;
    radio_packet_protocol.Packet.company_id[1] = COMPANY_ID;

    radio_packet_protocol.Packet.device_id[0] = DEVICE_TYPE >> 8;
    radio_packet_protocol.Packet.device_id[1] = DEVICE_TYPE;

    memcpy(radio_packet_protocol.Packet.mac_address, mac_address, 8);

    radio_packet_protocol.Packet.control_number = 0;

    SensorTask_registerPacketSendRequestCallback(sendPacketCallback);

    radio_init = 0x01;

    while (1)
    {

        Semaphore_pend(radioAccessSemHandle, BIOS_WAIT_FOREVER);

        EasyLink_setRfPower(TX_POWER);

        radio_sensor_data_packet_send(radio_packet_protocol,
                                      packet_payload_length);

        uint32_t events = Event_pend(radioEventHandle, 0,
        RADIO_TASK_EVENT_ALL,
                                     BIOS_WAIT_FOREVER);

        if (events == RADIO_TASK_EVENT_ACK)
        {

            recv_error_count = 0;

        }
        else if (events == RADIO_TASK_EVENT_ACK_TIMEOUT)
        {

            recv_error_count += 1;

            if (recv_error_count > RECV_ERROR_MAX_COUNT)
            {

                collection_cycle_timeout_count += 3;
                recv_error_count = 0;

            }

        }

    }

}

void RadioTask_init(void)
{

    Semaphore_Params semParam;
    Semaphore_Params_init(&semParam);
    Semaphore_construct(&radioAccessSem, 0, &semParam);
    radioAccessSemHandle = Semaphore_handle(&radioAccessSem);

    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&radioEvent, &eventParam);
    radioEventHandle = Event_handle(&radioEvent);

    Task_Params_init(&radioTaskParams);
    radioTaskParams.stackSize = RADIO_TASK_STACK_SIZE;
    radioTaskParams.priority = RADIO_TASK_TASK_PRIORITY;
    radioTaskParams.stack = &radioTaskStack;
    Task_construct(&radioTask, radioTaskFunction, &radioTaskParams, NULL);

}
