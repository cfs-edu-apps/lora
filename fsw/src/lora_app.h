/*
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Lesser General Public License as
**  published by the Free Software Foundation, either version 3 of the
**  License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this program.  If not, see <https://www.gnu.org/licenses/>.
**
**  Purpose:
**    Define the LoRa application
**
**  Notes:
**    1. This app serves as a ground interface for the LoRa Demos defined 
**       in the lora_rx and lora_tx objects.
**
*/
#ifndef _lora_app_
#define _lora_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "radio_if.h"
#include "lora_rx.h"
#include "lora_tx.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define LORA_APP_INIT_APP_EID    (LORA_APP_BASE_EID + 0)
#define LORA_APP_NOOP_EID        (LORA_APP_BASE_EID + 1)
#define LORA_APP_EXIT_EID        (LORA_APP_BASE_EID + 2)
#define LORA_APP_INVALID_MID_EID (LORA_APP_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/

// Command and Tlemetry packets are defined in lora_tx.xml


/******************************************************************************
** LORA_Class
*/
typedef struct 
{

   /* 
   ** App Framework
   */ 
   
   INITBL_Class_t    IniTbl;
   CFE_SB_PipeId_t   CmdPipe;
   CMDMGR_Class_t    CmdMgr;
   CHILDMGR_Class_t  RxChildMgr;
   CHILDMGR_Class_t  TxChildMgr;
   
   /*
   ** Telemetry Packets
   */
   
   LORA_StatusTlm_t  StatusTlm;

   /*
   ** App State & Objects
   */ 
       
   uint32             PerfId;
   CFE_SB_MsgId_t     CmdMid;
   CFE_SB_MsgId_t     OneHzMid;
   
   RADIO_IF_Class_t   RadioIf;
   LORA_RX_Class_t    LoraRx;
   LORA_TX_Class_t    LoraTx;
  
} LORA_APP_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern LORA_APP_Class_t  LoraApp;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: LORA_AppMain
**
*/
void LORA_AppMain(void);


/******************************************************************************
** Function: LORA_APP_NoOpCmd
**
*/
bool LORA_NoOpCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: LORA_APP_ResetAppCmd
**
*/
bool LORA_ResetAppCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _lora_app_ */
