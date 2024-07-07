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

/*
** Includes
*/

#include "lora_rx.h"


/***********************/
/** Macro Definitions **/
/***********************/


/**********************/
/** Type Definitions **/
/**********************/


/********************************** **/
/** Local File Function Prototypes **/
/************************************/


/*****************/
/** Global Data **/
/*****************/

static LORA_RX_Class_t *LoraRx;


/******************************************************************************
** Function: LORA_RX_Constructor
**
** Initialize the LoRa Transmit object
**
** Notes:
**   1. This must be called prior to any other member functions.
**
*/
void LORA_RX_Constructor(LORA_RX_Class_t *LoraRxPtr)
{

   LoraRx = LoraRxPtr;
   
   memset(LoraRx, 0, sizeof(LORA_RX_Class_t));
   
} /* End LORA_RX_Constructor() */


/******************************************************************************
** Function: LORA_RX_ChildTask
**
** Notes:
**   1. Returning false causes the child task to terminate.
**   2. Information events are sent because this is instructional code and the
**      events provide feedback. The events are filtered so they won't flood
**      the ground. A reset app command resets the event filter.  
*/
bool LORA_RX_ChildTask(CHILDMGR_Class_t *ChildMgr)
{

   // TODO: Add Rx child task logic (see end of file) May not need a child task if callbacks from driver are used
   while (true)
   {  
      LoraRx->PktCnt++;
      CFE_EVS_SendEvent (LORA_RX_CHILD_TASK_EID, CFE_EVS_EventType_INFORMATION,
                         "Rx child task execution. Msgcnt = %d", LoraRx->PktCnt);
      OS_TaskDelay(2000);
   }

   return true;
   
} /* End LORA_RX_ChildTask() */


/******************************************************************************
** Function: LORA_RX_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
*/
void LORA_RX_ResetStatus(void)
{

   LoraRx->PktCnt    = 0;
   LoraRx->PktErrCnt = 0;

} /* End LORA_RX_ResetStatus() */


/** lora_rx.cpp

	// Pins based on hardware configuration
	SX128x_Linux Radio("/dev/spidev0.0", 0, {27, 26, 20, 16, -1, -1, 24, 25});

	// Assume we're running on a high-end Raspberry Pi,
	// so we set the SPI clock speed to the maximum value supported by the chip
	Radio.SetSpiSpeed(8000000);

	Radio.Init();
	puts("Init done");
	Radio.SetStandby(SX128x::STDBY_XOSC);
	puts("SetStandby done");
	Radio.SetRegulatorMode(static_cast<SX128x::RadioRegulatorModes_t>(0));
	puts("SetRegulatorMode done");
	Radio.SetLNAGainSetting(SX128x::LNA_HIGH_SENSITIVITY_MODE);
	puts("SetLNAGainSetting done");

	Radio.SetBufferBaseAddresses(0x00, 0x00);
	puts("SetBufferBaseAddresses done");

	SX128x::ModulationParams_t ModulationParams;
	SX128x::PacketParams_t PacketParams;

	ModulationParams.PacketType = SX128x::PACKET_TYPE_LORA;
	ModulationParams.Params.LoRa.CodingRate = SX128x::LORA_CR_4_8;
	ModulationParams.Params.LoRa.Bandwidth = SX128x::LORA_BW_1600;
	ModulationParams.Params.LoRa.SpreadingFactor = SX128x::LORA_SF7;

	PacketParams.PacketType = SX128x::PACKET_TYPE_LORA;
	auto &l = PacketParams.Params.LoRa;
	l.PayloadLength = PACKET_SIZE;
	l.HeaderType = SX128x::LORA_PACKET_VARIABLE_LENGTH;
	l.PreambleLength = 12;
	l.Crc = SX128x::LORA_CRC_ON;
	l.InvertIQ = SX128x::LORA_IQ_NORMAL;

	Radio.SetPacketType(SX128x::PACKET_TYPE_LORA);

	puts("SetPacketType done");
	Radio.SetModulationParams(ModulationParams);
	puts("SetModulationParams done");
	Radio.SetPacketParams(PacketParams);
	puts("SetPacketParams done");

	auto freq = strtol(argv[1], nullptr, 10);
	Radio.SetRfFrequency(freq * 1000000UL);
	puts("SetRfFrequency done");

	std::cout << "Firmware Version: " << Radio.GetFirmwareVersion() << "\n";

	// Open file
	std::ofstream fileToReceive(argv[2], std::ios::binary);

	size_t packetCount = 0;
	size_t expectedPackets = 0;
	bool receivedFirstPacket = false;
	bool receivedLastPacket = false;

	Radio.callbacks.rxDone = [&]
	{
		printf("Received packet -> ");
		SX128x::PacketStatus_t ps;
		Radio.GetPacketStatus(&ps);

		uint8_t recv_buf[PACKET_SIZE];
		uint8_t recv_size;
		Radio.GetPayload(recv_buf, &recv_size, PACKET_SIZE);

		// If first packet, set size
		if (!receivedFirstPacket)
		{
			expectedPackets = atoi((const char *)recv_buf);
			printf("first packet, expecting %d packets!\n", expectedPackets);

			receivedFirstPacket = true;
			return;
		}

		// Other packets -> write to file
		char *rev_buf_char = (char *)&recv_buf[0];
		fileToReceive.write(rev_buf_char, recv_size);

		packetCount++;
		printf("packet count: %ld\n", packetCount);

		if (packetCount == expectedPackets) {
			puts("Received all packets, exiting...\n");
			receivedLastPacket = true;
			return;
		}

		// Print packet info
		int8_t noise = ps.LoRa.RssiPkt - ps.LoRa.SnrPkt;
		int8_t rscp = ps.LoRa.RssiPkt + ps.LoRa.SnrPkt;
		printf("recvd %u bytes, RSCP: %d, RSSI: %d, Noise: %d, SNR: %d\n", recv_size, rscp, ps.LoRa.RssiPkt, noise, ps.LoRa.SnrPkt);
	};

	auto IrqMask = SX128x::IRQ_RX_DONE | SX128x::IRQ_TX_DONE | SX128x::IRQ_RX_TX_TIMEOUT;
	Radio.SetDioIrqParams(IrqMask, IrqMask, SX128x::IRQ_RADIO_NONE, SX128x::IRQ_RADIO_NONE);
	puts("SetDioIrqParams done");

	Radio.StartIrqHandler();
	puts("StartIrqHandler done");

	Radio.SetRx({SX128x::RADIO_TICK_SIZE_1000_US, 0xFFFF});
	puts("SetRx done");

	while (!receivedLastPacket)
	{
		sleep(1);
	}

	printf("Done!\n");
	fileToReceive.close();
	Radio.StopIrqHandler();
	exit(EXIT_SUCCESS);
}

lora_rx.cpp **/
