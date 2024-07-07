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
**    Implement the Transmit Demo Class methods
**
**  Notes:
**    1. Serves as a bridge between the C++ Radio object and the Lora app.
**       This object is needed because using extern "C" with cfe.h wouldn't
**       compile. If this worked then radio_if and radio could have been
**       one object.
**    2. For each radio command, the Radio object in the SX128X library
**       performs radio level validation checks. The Radio object does not
**       have a cFE interface so these command functions issue events
**       messages.
**    TODO: Determine which command validity checks should be implemented
**    TODO: Detmerine what radio status can be provide in command failure events
**    TODO: Determine how the radio state is maintained and reported in tlm.
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "radio.h"
#include "radio_if.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ   (RadioIf->IniTbl)


/**********************/
/** Global File Data **/
/**********************/

static RADIO_IF_Class_t *RadioIf = NULL;


/*******************************/
/** Local Function Prototypes **/
/*******************************/


/******************************************************************************
** Function: RADIO_IF_Constructor
**
** Initialize the Radio Interface object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void RADIO_IF_Constructor(RADIO_IF_Class_t *RadioIfPtr, INITBL_Class_t *IniTbl)
{
   
   RadioIf = RadioIfPtr;
   
   memset(RadioIf, 0, sizeof(RADIO_IF_Class_t));
   
   RadioIf->IniTbl = IniTbl;
   
   RadioIf->RadioConfig.Frequency = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_FREQUENCY);
   
   RadioIf->RadioConfig.Modulation.SpreadingFactor = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_LORA_SF);
   RadioIf->RadioConfig.Modulation.Bandwidth       = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_LORA_BW);
   RadioIf->RadioConfig.Modulation.CodingRate      = INITBL_GetIntConfig(INITBL_OBJ, CFG_RADIO_LORA_CR);
      
   CFE_MSG_Init(CFE_MSG_PTR(RadioIf->RadioTlm.TelemetryHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_LORA_RADIO_TLM_TOPICID)), sizeof(LORA_RadioTlm_t));

} /* End RADIO_IF_Constructor() */


/******************************************************************************
** Function: RADIO_IF_ChildTask
**
** Notes:
**   1. Returning false causes the child task to terminate.
**   2. Information events are sent because this is instructional code and the
**      events provide feedback. The events are filtered so they won't flood
**      the ground. A reset app command resets the event filter.  
**
*/
bool RADIO_IF_ChildTask(CHILDMGR_Class_t *ChildMgr)
{
   
   bool RetStatus = true;
 
   CFE_EVS_SendEvent (RADIO_IF_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION, 
                      "RADIO_IF_ChildTask()"); 
   OS_TaskDelay(2000);
       
   return RetStatus;

} /* End RADIO_IF_ChildTask() */


/******************************************************************************
** Function: RADIO_IF_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void RADIO_IF_ResetStatus(void)
{

   return;

} /* End RADIO_IF_ResetStatus() */


/******************************************************************************
** Function: RADIO_IF_SendRadioTlmCmd
**
** Notes:
**   1. See radio_if.h file prologue for data source details.
*/
bool RADIO_IF_SendRadioTlmCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   LORA_RadioTlm_Payload_t *RadioTlmPayload = &RadioIf->RadioTlm.Payload;
   
   strcpy(RadioTlmPayload->SpiDevStr, "TODO");   
   RadioTlmPayload->SpiDevNum      = 0;
   RadioTlmPayload->SpiSpeed       = 0; //TODO: Get SPI setting
   RadioTlmPayload->RadioPinBusy   = 1;
   RadioTlmPayload->RadioPinNrst   = 2;
   RadioTlmPayload->RadioPinNss    = 3;
   RadioTlmPayload->RadioPinDio1   = 4;
   RadioTlmPayload->RadioPinDio2   = 5;
   RadioTlmPayload->RadioPinDio3   = 6;
   RadioTlmPayload->RadioPinTxEn   = 7;
   RadioTlmPayload->RadioPinRxEn   = 8;

   RadioTlmPayload->RadioFrequency            = RadioIf->RadioConfig.Frequency;
   RadioTlmPayload->ModulationSpreadingFactor = RadioIf->RadioConfig.Modulation.SpreadingFactor;
   RadioTlmPayload->ModulationBandwidth       = RadioIf->RadioConfig.Modulation.Bandwidth;
   RadioTlmPayload->ModulationCodingRate      = RadioIf->RadioConfig.Modulation.CodingRate;
   
   CFE_SB_TimeStampMsg(CFE_MSG_PTR(RadioIf->RadioTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(RadioIf->RadioTlm.TelemetryHeader), true);

   CFE_EVS_SendEvent(RADIO_IF_SEND_RADIO_TLM_CMD_EID, CFE_EVS_EventType_INFORMATION,
                     "Sent Radio configuration telemetry message");
   return true;
   
} /* RADIO_IF_SendRadioTlmCmd() */


/******************************************************************************
** Function: RADIO_IF_SetLowNoiseAmpModeCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
bool RADIO_IF_SetLowNoiseAmpModeCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetLowNoiseAmpMode_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetLowNoiseAmpMode_t);
   bool RetStatus = false;

   if (Cmd->LowNoiseAmpMode >= SX128X_LowNoiseAmpMode_Enum_t_MIN && Cmd->LowNoiseAmpMode <= SX128X_LowNoiseAmpMode_Enum_t_MAX)
   {
      RadioIf->RadioConfig.LowNoiseAmpMode = Cmd->LowNoiseAmpMode;
      
      RetStatus = RADIO_SetLowNoiseAmpMode(Cmd->LowNoiseAmpMode);
      if (RetStatus)
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_LOW_NOISE_AMP_MODE_CMD_EID, CFE_EVS_EventType_INFORMATION,
                           "Set radio power amp sensitivity mode succeeded: Mode = %d", Cmd->LowNoiseAmpMode);
      }
      else
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_LOW_NOISE_AMP_MODE_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio power amp sensitivity mode command failed");
      }
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_LOW_NOISE_AMP_MODE_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio power amp sensitivity mode command failed, invalid mode %d.",
                           Cmd->LowNoiseAmpMode);
   }

   return RetStatus;
   
}/* RADIO_IF_SetLowNoiseAmpModeCmd() */


/******************************************************************************
** Function: RADIO_IF_SetModulationParamsCmd
**
** Notes:
**   1. See file prologue notes for command function design notes. 
*/
bool RADIO_IF_SetModulationParamsCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetModulationParams_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetModulationParams_t);
   bool RetStatus = false;

   RadioIf->RadioConfig.Modulation.SpreadingFactor = Cmd->SpreadingFactor;
   RadioIf->RadioConfig.Modulation.Bandwidth       = Cmd->Bandwidth;
   RadioIf->RadioConfig.Modulation.CodingRate      = Cmd->CodingRate;

   RetStatus = RADIO_SetModulationParams(RadioIf->RadioConfig.Modulation.SpreadingFactor,
                                         RadioIf->RadioConfig.Modulation.Bandwidth,
                                         RadioIf->RadioConfig.Modulation.CodingRate);
   if (RetStatus)
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_MODULATION_PARAMS_CMD_EID, CFE_EVS_EventType_INFORMATION,
                        "Set modulation parameters command succeeded: SF=%d, BW=%d, RC=%d", Cmd->SpreadingFactor,
                        Cmd->Bandwidth, Cmd->CodingRate);
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_MODULATION_PARAMS_CMD_EID, CFE_EVS_EventType_ERROR,
                        "Set modulation parameters command failed");
   }

   return RetStatus;
   
} /* RADIO_IF_SetModulationParamsCmd() */


/******************************************************************************
** Function: RADIO_IF_PowerAmpRampTimeCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
bool RADIO_IF_SetPowerAmpRampTimeCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetPowerAmpRampTime_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetPowerAmpRampTime_t);
   bool RetStatus = false;

   if (Cmd->PowerAmpRampTime >= SX128X_PowerAmpRampTime_Enum_t_MIN && Cmd->PowerAmpRampTime <= SX128X_PowerAmpRampTime_Enum_t_MAX)
   {
      RadioIf->RadioConfig.PowerAmpRampTime = Cmd->PowerAmpRampTime;
      
      RetStatus = RADIO_SetLowNoiseAmpMode(Cmd->PowerAmpRampTime);
      if (RetStatus)
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_POWER_AMP_RAMP_TIME_CMD_EID, CFE_EVS_EventType_INFORMATION,
                           "Set radio power amp ramp time succeeded: Mode = %d", Cmd->PowerAmpRampTime);
      }
      else
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_POWER_AMP_RAMP_TIME_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio power amp ramp time command failed");
      }
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_POWER_AMP_RAMP_TIME_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio power amp ramp time command failed, invalid mode %d.",
                           Cmd->PowerAmpRampTime);
   }

   return RetStatus;
   
}/* RADIO_IF_PowerAmpRampTimeCmd() */


/******************************************************************************
** Function: RADIO_IF_SetPowerRegulatorModeCmd
**
** Notes:
**   1. See file prologue notes for command function design notes. 
*/
bool RADIO_IF_SetPowerRegulatorModeCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetPowerRegulatorMode_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetPowerRegulatorMode_t);
   bool RetStatus = false;

   if (Cmd->PowerRegulatorMode >= SX128X_PowerRegulatorMode_Enum_t_MIN && Cmd->PowerRegulatorMode <= SX128X_PowerRegulatorMode_Enum_t_MAX)
   {
      RadioIf->RadioConfig.PowerRegulatorMode = Cmd->PowerRegulatorMode;
      
      RetStatus = RADIO_SetPowerRegulatorMode(Cmd->PowerRegulatorMode);
      if (RetStatus)
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_POWER_REGULATOR_MODE_CMD_EID, CFE_EVS_EventType_INFORMATION,
                           "Set radio power regulator mode succeeded: Mode = %d", Cmd->PowerRegulatorMode);
      }
      else
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_POWER_REGULATOR_MODE_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio power regulator mode command failed");
      }
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_POWER_REGULATOR_MODE_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio power regulator mode command failed, invalid mode %d.",
                           Cmd->PowerRegulatorMode);
   }

   return RetStatus;
      
} /* RADIO_IF_SetPowerRegulatorModeCmd() */


/******************************************************************************
** Function: RADIO_IF_SetRadioFrequencyCmd
**
** Notes:
**   1. See file prologue notes for command function design notes. 
*/
bool RADIO_IF_SetRadioFrequencyCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetRadioFrequency_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetRadioFrequency_t);
   bool RetStatus = false;

   if (Cmd->Frequency >= 0 && Cmd->Frequency <= 48000)
   {
      RadioIf->RadioConfig.Frequency = Cmd->Frequency;
      
      RetStatus = RADIO_SetRadioFrequency(Cmd->Frequency*1000000UL);
      if (RetStatus)
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_RADIO_FREQUENCY_CMD_EID, CFE_EVS_EventType_INFORMATION,
                           "Set radio frequency command succeeded: Frequency = %d Mhz", Cmd->Frequency);
      }
      else
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_RADIO_FREQUENCY_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio frequency command failed");
      }
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_RADIO_FREQUENCY_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio frequency command failed, invalid frequency %d.",
                           Cmd->Frequency);
   }

   return RetStatus;
   
} /* RADIO_IF_SetRadioFrequencyCmd() */


/******************************************************************************
** Function: RADIO_IF_SetStandbyModeCmd
**
** Notes:
**   1. See file prologue notes for command function design notes. 
*/
bool RADIO_IF_SetStandbyModeCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{
   
   const LORA_SetStandbyMode_CmdPayload_t *Cmd = CMDMGR_PAYLOAD_PTR(MsgPtr, LORA_SetStandbyMode_t);
   bool RetStatus = false;

   if (Cmd->StandbyMode >= SX128X_StandbyMode_Enum_t_MIN && Cmd->StandbyMode <= SX128X_StandbyMode_Enum_t_MAX)
   {
      RadioIf->RadioConfig.StandbyMode = Cmd->StandbyMode;
      
      RetStatus = RADIO_SetStandbyMode(Cmd->StandbyMode);
      if (RetStatus)
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_STANDBY_MODE_CMD_EID, CFE_EVS_EventType_INFORMATION,
                           "Set radio standby mode succeeded: Mode = %d", Cmd->StandbyMode);
      }
      else
      {
         CFE_EVS_SendEvent(RADIO_IF_SET_STANDBY_MODE_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio standby mode command failed");
      }
   }
   else
   {
      CFE_EVS_SendEvent(RADIO_IF_SET_STANDBY_MODE_CMD_EID, CFE_EVS_EventType_ERROR,
                           "Set radio standby mode command failed, invalid mode %d.",
                           Cmd->StandbyMode);
   }

   return RetStatus;
   
} /* RADIO_IF_SetStandbyModeCmd() */


