
/*
 *  ======== ti_easylink_config.c ========
 *  Configured EasyLink module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC1312R1F3RGZ
 *  by the SysConfig tool.
 */

/***** Includes *****/
#include "easylink/EasyLink.h"
#include <stdint.h>

/* TI Drivers */
#include <ti/drivers/rf/RF.h>

/* Radio Config */
#include <ti_radio_config.h>

EasyLink_RfSetting EasyLink_supportedPhys[] = {
    {
        .EasyLink_phyType = EasyLink_Phy_5kbpsSlLr,
        .RF_pProp = &RF_prop_sl_lr,
        .RF_uCmdPropRadio.RF_pCmdPropRadioDivSetup = &RF_cmdPropRadioDivSetup_sl_lr,
        .RF_pCmdFs = &RF_cmdFs_sl_lr,
        .RF_pCmdPropTx = &RF_cmdPropTx_sl_lr,
        .RF_pCmdPropTxAdv = NULL,
        .RF_pCmdPropRxAdv = &RF_cmdPropRxAdv_sl_lr,
        .RF_pTxPowerTable = PROP_RF_txPowerTable_sl_lr,
        .RF_txPowerTableSize = RF_PROP_TX_POWER_TABLE_SIZE_SL_LR,
    },
};

const uint8_t EasyLink_numSupportedPhys = sizeof(EasyLink_supportedPhys)/sizeof(EasyLink_RfSetting);
