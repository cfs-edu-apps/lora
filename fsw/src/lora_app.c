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
**    Implement the LoRa application
**
**  Notes:
**    1. Three contained objects perform the following functions: 
**       - radio_if: Configure and control the SX128x radio object
**       - lora_rx: Receive and process data from the SX128x radio object
**       - lora_tx: Transmitt data to the SX128x radio object
**    2. The radio object is defined in the SX128x library
**
*/

/*
** Includes
*/

#include <string.h>
#include "sx128x_lib.h"
#include "lora_app.h"
#include "lora_eds_cc.h"
#include "lora_tx.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ      (&(LoraApp.IniTbl))
#define  CMDMGR_OBJ      (&(LoraApp.CmdMgr))
#define  CHILDMGR_OBJ    (&(LoraApp.ChildMgr))
#define  RX_CHILDMGR_OBJ (&(LoraApp.RxChildMgr))
#define  TX_CHILDMGR_OBJ (&(LoraApp.TxChildMgr))
#define  RADIO_IF_OBJ    (&(LoraApp.RadioIf))
#define  LORA_RX_OBJ     (&(LoraApp.LoraRx))
#define  LORA_TX_OBJ     (&(LoraApp.LoraTx))

/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static int32 ProcessCommands(void);
static void SendStatusTlm(void);


/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  


static CFE_EVS_BinFilter_t  EventFilters[] =
{  
   // Use CFE_EVS_NO_FILTER to see all events
   /* Event ID                Mask */
   {LORA_RX_CHILD_TASK_EID,   CFE_EVS_FIRST_4_STOP},
   {LORA_TX_CHILD_TASK_EID,   CFE_EVS_FIRST_4_STOP}

};


/*****************/
/** Global Data **/
/*****************/

LORA_APP_Class_t  LoraApp;

/******************************************************************************
** Function: LORA_AppMain
**
*/
void LORA_AppMain(void)
{

   uint32 RunStatus = CFE_ES_RunStatus_APP_ERROR;


   CFE_EVS_Register(EventFilters, sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                    CFE_EVS_EventFilter_BINARY);

   if (InitApp() == CFE_SUCCESS) /* Performs initial CFE_ES_PerfLogEntry() call */
   {  
   
      RunStatus = CFE_ES_RunStatus_APP_RUN;
      
   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      RunStatus = ProcessCommands(); /* Pends indefinitely & manages CFE_ES_PerfLogEntry() calls */

   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("LORA App terminating, err = 0x%08X\n", RunStatus);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(LORA_APP_EXIT_EID, CFE_EVS_EventType_CRITICAL, "LORA App terminating, err = 0x%08X", RunStatus);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of LORA_AppMain() */


/******************************************************************************
** Function: LORA_APP_NoOpCmd
**
*/

bool LORA_APP_NoOpCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_SendEvent (LORA_APP_NOOP_EID, CFE_EVS_EventType_INFORMATION,
                      "No operation command received for LORA App version %d.%d.%d",
                      LORA_APP_MAJOR_VER, LORA_APP_MINOR_VER, LORA_APP_PLATFORM_REV);

   return true;


} /* End LORA_APP_NoOpCmd() */


/******************************************************************************
** Function: LORA_APP_ResetAppCmd
**
** Notes:
**   1. No need to pass an object reference to contained objects because they
**      already have a reference from when they were constructed
**
*/

bool LORA_APP_ResetAppCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_ResetAllFilters();
   
   CMDMGR_ResetStatus(CMDMGR_OBJ);
   CHILDMGR_ResetStatus(RX_CHILDMGR_OBJ);
   CHILDMGR_ResetStatus(TX_CHILDMGR_OBJ);
   
   RADIO_IF_ResetStatus();
   LORA_RX_ResetStatus();
   LORA_TX_ResetStatus();
   
   return true;

} /* End LORA_APP_ResetAppCmd() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = APP_C_FW_CFS_ERROR;
   
   CHILDMGR_TaskInit_t ChildTaskInit;
   
   CFE_ES_PerfLogEntry(LoraApp.PerfId);

   /*
   ** Initialize objects 
   */

   if (INITBL_Constructor(&LoraApp.IniTbl, LORA_INI_FILENAME, &IniCfgEnum))
   {

      LoraApp.PerfId   = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_PERF_ID);
      LoraApp.CmdMid   = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_LORA_CMD_TOPICID));
      LoraApp.OneHzMid = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_BC_SCH_1_HZ_TOPICID));
      
      /*
      ** Initialize contained objects
      */
      
      RADIO_IF_Constructor(RADIO_IF_OBJ, &LoraApp.IniTbl);

      /* Child Manager constructor sends error events */

      LORA_RX_Constructor(LORA_RX_OBJ);
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_RX_CHILD_NAME);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_RX_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_RX_CHILD_PRIORITY);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CFG_RX_CHILD_PERF_ID);
      Status = CHILDMGR_Constructor(RX_CHILDMGR_OBJ, ChildMgr_TaskMainCallback,
                                    LORA_RX_ChildTask, &ChildTaskInit); 

      LORA_TX_Constructor(LORA_TX_OBJ, INITBL_GetStrConfig(INITBL_OBJ, CFG_TX_CHILD_SEM_NAME));
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_TX_CHILD_NAME);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_TX_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_TX_CHILD_PRIORITY);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CFG_TX_CHILD_PERF_ID);
      Status = CHILDMGR_Constructor(TX_CHILDMGR_OBJ, ChildMgr_TaskMainCallback,
                                    LORA_TX_ChildTask, &ChildTaskInit); 
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS)
   {

      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&LoraApp.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe(LoraApp.CmdMid,   LoraApp.CmdPipe);
      CFE_SB_Subscribe(LoraApp.OneHzMid, LoraApp.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, LORA_APP_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, LORA_APP_ResetAppCmd, 0);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SEND_RADIO_TLM_CC,           RADIO_IF_OBJ, RADIO_IF_SendRadioTlmCmd,          0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SET_LOW_NOISE_AMP_MODE_CC,   RADIO_IF_OBJ, RADIO_IF_SetLowNoiseAmpModeCmd,    sizeof(LORA_SetLowNoiseAmpMode_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SET_MODULATION_PARAMS_CC,    RADIO_IF_OBJ, RADIO_IF_SetModulationParamsCmd,   sizeof(LORA_SetModulationParams_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SET_POWER_AMP_RAMP_TIME_CC,  RADIO_IF_OBJ, RADIO_IF_SetPowerAmpRampTimeCmd,   sizeof(LORA_SetPowerAmpRampTime_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SET_POWER_REGULATOR_MODE_CC, RADIO_IF_OBJ, RADIO_IF_SetPowerRegulatorModeCmd, sizeof(LORA_SetPowerRegulatorMode_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SET_RADIO_FREQUENCY_CC,      RADIO_IF_OBJ, RADIO_IF_SetRadioFrequencyCmd,     sizeof(LORA_SetRadioFrequency_CmdPayload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_SET_STANDBY_MODE_CC,         RADIO_IF_OBJ, RADIO_IF_SetStandbyModeCmd,        sizeof(LORA_SetStandbyMode_CmdPayload_t));

      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_START_TX_DEMO_CC, LORA_TX_OBJ, LORA_TX_StartDemoCmd, 0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, LORA_STOP_TX_DEMO_CC,  LORA_TX_OBJ, LORA_TX_StopDemoCmd,  0);

      CFE_MSG_Init(CFE_MSG_PTR(LoraApp.StatusTlm.TelemetryHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_LORA_STATUS_TLM_TOPICID)), sizeof(LORA_StatusTlm_t));
   
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(LORA_APP_INIT_APP_EID, CFE_EVS_EventType_INFORMATION,
                        "LORA  App Initialized. Version %d.%d.%d",
                        LORA_APP_MAJOR_VER, LORA_APP_MINOR_VER, LORA_APP_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static int32 ProcessCommands(void)
{

   int32  RetStatus = CFE_ES_RunStatus_APP_RUN;
   int32  SysStatus;

   CFE_SB_Buffer_t* SbBufPtr;
   CFE_SB_MsgId_t   MsgId = CFE_SB_INVALID_MSG_ID;
   

   CFE_ES_PerfLogExit(LoraApp.PerfId);
   SysStatus = CFE_SB_ReceiveBuffer(&SbBufPtr, LoraApp.CmdPipe, CFE_SB_PEND_FOREVER);
   CFE_ES_PerfLogEntry(LoraApp.PerfId);

   if (SysStatus == CFE_SUCCESS)
   {
      
      SysStatus = CFE_MSG_GetMsgId(&SbBufPtr->Msg, &MsgId);
   
      if (SysStatus == CFE_SUCCESS)
      {
  
         if (CFE_SB_MsgId_Equal(MsgId, LoraApp.CmdMid)) 
         {
            
            CMDMGR_DispatchFunc(CMDMGR_OBJ, &SbBufPtr->Msg);
         
         } 
         else if (CFE_SB_MsgId_Equal(MsgId, LoraApp.OneHzMid))
         {

            SendStatusTlm();
            
         }
         else
         {
            
            CFE_EVS_SendEvent(LORA_APP_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                              "Received invalid command packet, MID = 0x%04X",
                              CFE_SB_MsgIdToValue(MsgId));
         } 

      }
      else
      {
         
         CFE_EVS_SendEvent(LORA_APP_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                           "CFE couldn't retrieve message ID from the message, Status = %d", SysStatus);
      }
      
   } /* Valid SB receive */ 
   else 
   {
   
         CFE_ES_WriteToSysLog("LORA App software bus error. Status = 0x%08X\n", SysStatus);   /* Use SysLog, events may not be working */
         RetStatus = CFE_ES_RunStatus_APP_ERROR;
   }  
      
   return RetStatus;

} /* End ProcessCommands() */


/******************************************************************************
** Function: SendStatusTlm
**
*/
static void SendStatusTlm(void)
{
   
   LORA_StatusTlm_Payload_t *StatusTlmPayload = &LoraApp.StatusTlm.Payload;
   
   StatusTlmPayload->ValidCmdCnt   = LoraApp.CmdMgr.ValidCmdCnt;
   StatusTlmPayload->InvalidCmdCnt = LoraApp.CmdMgr.InvalidCmdCnt;

   /*
   ** Radio Interface Object
   */ 

   StatusTlmPayload->RadioInit = SX128X_Initialized();

   /*
   ** Rx Object
   */ 

   StatusTlmPayload->RxPktCnt    = LoraApp.LoraRx.PktCnt;
   StatusTlmPayload->RxPktErrCnt = LoraApp.LoraRx.PktErrCnt;

   /*
   ** Tx Object
   */ 
   
   StatusTlmPayload->TxDemoActive = LoraApp.LoraTx.DemoActive;
   StatusTlmPayload->TxPktCnt     = LoraApp.LoraTx.PktCnt;
   StatusTlmPayload->TxPktErrCnt  = LoraApp.LoraTx.PktErrCnt;
      
   CFE_SB_TimeStampMsg(CFE_MSG_PTR(LoraApp.StatusTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(LoraApp.StatusTlm.TelemetryHeader), true);
   
} /* End SendStatusTlm() */
