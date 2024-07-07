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
**   Manage Lora Receive demo
**
** Notes:
**   None
**
*/

#ifndef _lora_rx_
#define _lora_rx_

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

#define LORA_RX_CONSTRUCTOR_EID           (LORA_RX_BASE_EID + 0)
#define LORA_RX_CONFIG_SUBSCRIPTIONS_EID  (LORA_RX_BASE_EID + 1)
#define LORA_RX_CHILD_TASK_EID            (LORA_RX_BASE_EID + 2)
#define LORA_RX_START_DEMO_EID            (LORA_RX_BASE_EID + 3)
#define LORA_RX_STOP_DEMO_EID             (LORA_RX_BASE_EID + 4)

/**********************/
/** Type Definitions **/
/**********************/

typedef struct
{

   uint32  PktCnt;
   uint32  PktErrCnt;
   
} LORA_RX_Class_t;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: LORA_RX_Constructor
**
** Initialize the LoRa Receive object
**
** Notes:
**   1. This must be called prior to any other member functions.
**
*/
void LORA_RX_Constructor(LORA_RX_Class_t *LoraRxPtr);


/******************************************************************************
** Function: LORA_RX_ChildTask
**
*/
bool LORA_RX_ChildTask(CHILDMGR_Class_t *ChildMgr);


/******************************************************************************
** Function: LORA_RX_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
*/
void LORA_RX_ResetStatus(void);


#endif /* _lora_rx_ */
