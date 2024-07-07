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
**    Define application configurations for the LoRa application
**
**  Notes:
**    1. These macros can only be built with the application and can't
**       have a platform scope because the same app_cfg.h filename is used for
**       all applications following the object-based application design.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "lora_eds_typedefs.h"
#include "lora_platform_cfg.h"
#include "app_c_fw.h"


/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release
*/

#define  LORA_APP_MAJOR_VER   1
#define  LORA_APP_MINOR_VER   0


/******************************************************************************
** Init File declarations create:
**
**  typedef enum {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigEnum;
**    
**  typedef struct {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigStruct;
**
**   const char *GetConfigStr(value);
**   ConfigEnum GetConfigVal(const char *str);
**
** XX(name,type)
*/

#define CFG_APP_CFE_NAME     APP_CFE_NAME
#define CFG_APP_PERF_ID      APP_PERF_ID

#define CFG_CMD_PIPE_NAME    APP_CMD_PIPE_NAME
#define CFG_CMD_PIPE_DEPTH   APP_CMD_PIPE_DEPTH

#define CFG_LORA_CMD_TOPICID         LORA_CMD_TOPICID
#define CFG_BC_SCH_1_HZ_TOPICID      BC_SCH_1_HZ_TOPICID
#define CFG_LORA_STATUS_TLM_TOPICID  LORA_STATUS_TLM_TOPICID
#define CFG_LORA_RADIO_TLM_TOPICID   LORA_RADIO_TLM_TOPICID

#define CFG_RX_CHILD_NAME       RX_CHILD_NAME
#define CFG_RX_CHILD_PERF_ID    RX_CHILD_PERF_ID
#define CFG_RX_CHILD_STACK_SIZE RX_CHILD_STACK_SIZE
#define CFG_RX_CHILD_PRIORITY   RX_CHILD_PRIORITY

#define CFG_TX_CHILD_SEM_NAME   TX_CHILD_SEM_NAME
#define CFG_TX_CHILD_NAME       TX_CHILD_NAME
#define CFG_TX_CHILD_PERF_ID    TX_CHILD_PERF_ID
#define CFG_TX_CHILD_STACK_SIZE TX_CHILD_STACK_SIZE
#define CFG_TX_CHILD_PRIORITY   TX_CHILD_PRIORITY

#define CFG_RADIO_FREQUENCY    RADIO_FREQUENCY
#define CFG_RADIO_LORA_SF      RADIO_LORA_SF
#define CFG_RADIO_LORA_BW      RADIO_LORA_BW
#define CFG_RADIO_LORA_CR      RADIO_LORA_CR

#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_PERF_ID,uint32) \
   XX(APP_CMD_PIPE_NAME,char*) \
   XX(APP_CMD_PIPE_DEPTH,uint32) \
   XX(LORA_CMD_TOPICID,uint32) \
   XX(BC_SCH_1_HZ_TOPICID,uint32) \
   XX(LORA_STATUS_TLM_TOPICID,uint32) \
   XX(LORA_RADIO_TLM_TOPICID,uint32) \
   XX(RX_CHILD_NAME,char*) \
   XX(RX_CHILD_PERF_ID,uint32) \
   XX(RX_CHILD_STACK_SIZE,uint32) \
   XX(RX_CHILD_PRIORITY,uint32) \
   XX(TX_CHILD_SEM_NAME,char*) \
   XX(TX_CHILD_NAME,char*) \
   XX(TX_CHILD_PERF_ID,uint32) \
   XX(TX_CHILD_STACK_SIZE,uint32) \
   XX(TX_CHILD_PRIORITY,uint32) \
   XX(RADIO_FREQUENCY,uint32) \
   XX(RADIO_LORA_SF,uint32) \
   XX(RADIO_LORA_BW,uint32) \
   XX(RADIO_LORA_CR,uint32)
   
DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define LORA_APP_BASE_EID  (APP_C_FW_APP_BASE_EID +  0)
#define LORA_RX_BASE_EID   (APP_C_FW_APP_BASE_EID + 20)
#define LORA_TX_BASE_EID   (APP_C_FW_APP_BASE_EID + 40)
#define RADIO_IF_BASE_EID  (APP_C_FW_APP_BASE_EID + 60)

#endif /* _app_cfg_ */
