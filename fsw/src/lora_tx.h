/*
** Copyright 2022 bitValence, Inc.
** All Rights Reserved.
**
** This program is free software; you can modify and/or redistribute it
** under the terms of the GNU Affero General Public License
** as published by the Free Software Foundation; version 3 with
** attribution addendums as found in the LICENSE.txt
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** Purpose:
**   Manage Lora Transmit demo
**
** Notes:
**   None
**
*/

#ifndef _lora_tx_
#define _lora_tx_

/*
** Includes
*/

#include "app_cfg.h"
#include "radio.h"


/***********************/
/** Macro Definitions **/
/***********************/


/*
** Event Message IDs
*/

#define LORA_TX_CONSTRUCTOR_EID           (LORA_TX_BASE_EID + 0)
#define LORA_TX_CONFIG_SUBSCRIPTIONS_EID  (LORA_TX_BASE_EID + 1)
#define LORA_TX_CHILD_TASK_EID            (LORA_TX_BASE_EID + 2)
#define LORA_TX_START_DEMO_EID            (LORA_TX_BASE_EID + 3)
#define LORA_TX_DEMO_SCRIPT_EID           (LORA_TX_BASE_EID + 4)
#define LORA_TX_STOP_DEMO_EID             (LORA_TX_BASE_EID + 5)

/**********************/
/** Type Definitions **/
/**********************/

typedef struct
{
   int32   RunStatus;
   uint32  WakeUpSemaphore;
   
   bool    DemoActive;
   uint32  PktCnt;
   uint32  PktErrCnt;
   
} LORA_TX_Class_t;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: LORA_TX_Constructor
**
** Initialize the LoRa Transmit object
**
** Notes:
**   1. This must be called prior to any other member functions.
**
*/
void LORA_TX_Constructor(LORA_TX_Class_t *LoraTxPtr, const char *SemName);


/******************************************************************************
** Function: LORA_TX_ChildTask
**
*/
bool LORA_TX_ChildTask(CHILDMGR_Class_t *ChildMgr);


/******************************************************************************
** Function: LORA_TX_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
*/
void LORA_TX_ResetStatus(void);


/******************************************************************************
** Function: LORA_TX_StartDemoCmd
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used
*/
bool LORA_TX_StartDemoCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: LORA_TX_StopDemoCmd
**
** Notes:
**   1. Signature must match CMDMGR_CmdFuncPtr_t
**   2. DataObjPtr is not used
*/
bool LORA_TX_StopDemoCmd(void* DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _lora_tx_ */
