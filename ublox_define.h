#ifndef UBLOX_DEFINE_H_
#define UBLOX_DEFINE_H_

#define SYNC_WORD_1 0xB5
#define SYNC_WORD_2 0x62

#define UBX_I2C_ACK_SIZE 10
#define UBX_I2C_POSLLH_ACK_SZIE 38
#define UBX_I2C_NAV_SAT_ACK_SIZE 253
#define UBX_I2C_NAV_AOP_ACK_SIZE 26
#define UBX_I2C_NAV_STATUS_ACK_SIZE 26
#define UBX_I2C_NAV_PVT_ACK_SIZE 98

const char RMC_OFF[] = { 0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X04, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00 };

const char VTG_OFF[] = { 0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X05, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00 };

const char GSA_OFF[] = { 0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X02, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00 };

const char GSV_OFF[] = { 0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X03, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00 };

const char GLL_OFF[] = { 0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X01, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00 };

const char GGA_OFF[] = { 0XB5, 0X62, 0X06, 0X01, 0X08, 0X00, 0XF0, 0X00, 0X00,
                         0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0X23 };

const char UBX_PM2[] = {
        0XB5, 0X62, 0X06, 0x3B, 0x30, 0x00, //Header
        0x02, //version
        0x00, //res
        0x00, //maxStartupStateDur
        0x00, //res
        0b11100000, 0b00010000, 0b00000000,
        0x00, //flag
        0x00, 0x00, 0x00,
        0x00, //updatePeriod
        0x00, 0x00, 0x00,
        0x00, //searchPeriod
        0x00, 0x00, 0x00,
        0x00, //gridOffset
        0x00,
        0x00, //onTime
        0x00,
        0x00, //minAcqTime
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //res
        0x00, 0x00, 0x00, 0x00 //extintInactivityMs
        };

const char CFG_CFG_CLEAR[] = { 0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, //Header
                               0x1F, 0x1F, 0x00, 0x00, //clearMask
                               0x00, 0x00, 0x00, 0x00, //saveMask
                               0x00, 0x00, 0x00, 0x00, //loadMask
                               0x01 //deviceMask
        };

const char CFG_CFG_SAVE[] = { 0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, //Header
                              0x00, 0x00, 0x00, 0x00, //clearMask
                              0x1F, 0x1F, 0x00, 0x00, //saveMask
                              0x00, 0x00, 0x00, 0x00, //loadMask
                              0x01 //deviceMask
        };

const char CFG_CFG_LOAD[] = { 0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, //Header
                              0x00, 0x00, 0x00, 0x00, //clearMask
                              0x00, 0x00, 0x00, 0x00, //saveMask
                              0x1F, 0x1F, 0x00, 0x00, //loadMask
                              0x01 //deviceMask
        };

const char NAV_PVT_ENABLE_1HZ[] = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, //Header
                                    0x01, 0x07, 0x01 };

const char ZOE_M8Q_DCDC_ENABLE[] = { 0xB5, 0x62, 0x06, 0x41, 0x0C, 0x00, 0x00,
                                     0x00, 0x03, 0x1F, 0xC5, 0x90, 0xE1, 0x9F,
                                     0xFF, 0xFF, 0xFE, 0xFF };

#endif /* UBLOX_DEFINE_H_ */
