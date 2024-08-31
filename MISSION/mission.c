/*
 * mission.c
 *
 *  Created on: Oct 14, 2022
 *      Author: Sushant
 */
#include "mission.h"

/*
 * @brief		function to execute the command received from GS
 *
 * @param		MCU_ID:		 	0x01 - MAIN OBC MCU
 * 							 	0x02 - RESET MCU
 * 							 	0x03 - CAM MSN(MSN 1)
 * 							 	0x04 - PACDS MSN(MSN2)
 * 							 	0x05 - SPDM MSN(MSN3);
 * 				MAIM_CMD (1-3) - MAIN COMMMAND TO BE EXECUTED
 * 				ADDR(1-4)	   - 4 BYTE ADDRESS OF FM
 * 				PCKT NUM 	   - NO OF PACKETS TO RECEIVE(1 packets  = 40 bytes of data)
 *
 * @retval 		none
 *
 */
void Execute_CMD(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3, uint8_t ADDR1, uint8_t ADDR2, uint8_t ADDR3,
		uint8_t ADDR4, uint8_t PCKT_NUM1, uint8_t PCKT_NUM2) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Command to be excuted right now...\r\n");
	myprintf("Command Relayed to Execute CMD Function:.......\r\n");
	myprintf(
			"MCU ID: %x\r\n MAIN CMD: %x %x %x \r\n ADDRESS: %x %x %x %x \r\n NO OF PACKETS: %x %x\r\n",
			MCU_ID, MAIN_CMD1, MAIN_CMD2, MAIN_CMD3, ADDR1, ADDR2, ADDR3, ADDR4,
			PCKT_NUM1, PCKT_NUM2);
#endif
#endif
	//if the command is directed to OBC
	if (MCU_ID == 0x01) {
		EXECUTE_CMD_OBC(MAIN_CMD1, MAIN_CMD2, MAIN_CMD3, ADDR1, ADDR2, ADDR3,
				ADDR4, PCKT_NUM1, PCKT_NUM2);
	}
	//if the command is directed to RESET, there are no commands to be sent from GS to Reset at the moment
	else if (MCU_ID == 0x02) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Command received to Execute command for Reset.....\r\n");
		myprintf(
				"There are no commands to be sent to Reset at the moment....\r\n");
#endif
#endif
	}
	//if the command is directed to CAM MCU
	else if (MCU_ID == 0x03) {
		Reset_CMD_PWR_ON_OFF(1);
		HAL_Delay(100);
		Reset_CMD_PWR_ON_OFF(2);
		TURN_ON_CAM_MSN();
		HAL_Delay(1000);
		EXECUTE_CMD_CAM(MCU_ID, MAIN_CMD1, MAIN_CMD2, MAIN_CMD3);
		Reset_CMD_PWR_ON_OFF(20);
		TURN_OFF_CAM_MSN();
	}
	//if the command is directed to PACDS MCU
	else if (MCU_ID == 0x04) {
		Reset_CMD_PWR_ON_OFF(1);
		TURN_ON_PACDS_MSN();
		EXECUTE_CMD_PACDS(MCU_ID, MAIN_CMD1, MAIN_CMD2, MAIN_CMD3);
		TURN_OFF_PACDS_MSN();
	}
	//if the command is directed to SPDM MCU
	else if (MCU_ID == 0x05) {
#ifndef S_COM_PLAN_F_MODE
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Command received to execute SPDM MSN\r\n");
#endif
#endif
		Reset_CMD_PWR_ON_OFF(1);
		HAL_Delay(100);
		TURN_ON_SPDM_MSN();
		HAL_Delay(8000);
		EXECUTE_CMD_SPDM(MCU_ID, MAIN_CMD1, MAIN_CMD2, MAIN_CMD3);
		TURN_OFF_SPDM_MSN();
		HAL_Delay(100);
#endif
	}
}

/*
 * @brief	 to execute the command directed to OBC
 *
 * @param	 MAIN_CMD(1-3) The main Command that holds the actual command
 *		     ADDR(1-4)	   The Address related to the FM operation
 *		     PCKT_NUM(1-2) The no of packets to be sent back to the GS
 *
 * @retval   none
 */
void EXECUTE_CMD_OBC(uint8_t MAIN_CMD1, uint8_t MAIN_CMD2, uint8_t MAIN_CMD3,
		uint8_t ADDR1, uint8_t ADDR2, uint8_t ADDR3, uint8_t ADDR4,
		uint8_t PCKT_NUM1, uint8_t PCKT_NUM2) {

	uint32_t ADDRESS = ADDR1 << 24 | ADDR2 << 16 | ADDR3 << 8 | ADDR4; //address of the data
	uint16_t PCKT_NUM = PCKT_NUM1 << 8 | PCKT_NUM2;	//no of packets to be transmitted

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Address: %x\r\n NO OF PACKETS: %x \r\n ", ADDRESS, PCKT_NUM);
#endif
#endif

	/*to erase the Corresponding sector of the FM*/
	if (MAIN_CMD1 == 0xE1 && MAIN_CMD2 == 0XE2) {
		MAIN_FM_MODE();		//switching MUX to OBC side
		for (int i = 0; i < MAIN_CMD3; i++) {//MAIN_CMD3 stores the no of sectors to erase
			Sector_Erase_4B(&MAIN_FM, ADDRESS, 64);	//Erasing MAIN FM corresponding sector
			Sector_Erase_4B(&SMSN_FM, ADDRESS, 64);	//Erasing SMSN FM corresponding sector
			ADDRESS = ADDRESS + SECTOR_SIZE;
		}
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("No of Sectors to erase %x \r\n Starting Address: %x \r\n",
				MAIN_CMD3, ADDRESS);
		myprintf(
				"Sectors Erased ......\r\n Reading first 10 packets for confirmation....\r\n");
		FM_Data_To_PC(&MAIN_FM, &ADDRESS, 10, 0x01);
#endif
#endif
		/* To download current address data*/
	} else if (MAIN_CMD1 == 0xAD && MAIN_CMD2 == 0xDA && MAIN_CMD3 == 0xDD) {

		Get_Address_Current_Address();		//updating the latest address data
		Get_Mission_Data_Address();			//getting the latest mission data

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"Command to download the Current address data of all the Missions...\r\n");
//		Print_Address_Location();
#endif
#endif
#ifdef ADDNICS_COM_MODE
		ADDNICS_TX_MODE();
		HAL_Delay(500);
		COM_Transmit_Data(ADDR_BUFFER, ADDRESS_DATA_SIZE, 0);
		HAL_Delay(500);
		ADDNICS_RX_MODE();
#endif

#ifdef S_COM_PLAN_F_MODE
		S_COM_TX_MODE();
		HAL_Delay(500);
		COM_Transmit(ADDR_BUFFER, ADDRESS_DATA_SIZE, 0);
		S_COM_RX_MODE();
#endif
//		MainToSubSys(&COM_TCV_STREAM, ADDR_BUFFER, ADDRESS_DATA_SIZE, 0);//TODO: test to send command to COM board
	}
	/*to download the data from the MAIN FM or the SMSN FM*/
	else if (MAIN_CMD1 == 0xB1 && MAIN_CMD2 == 0xDA) {
		/*to download the data from the MAIN FM*/
		if (MAIN_CMD3 == 0x11) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Command to download the data FROM MAIN FM\r\n");
#endif
#endif
			TRANSFER_DATA_COM(&MAIN_FM, ADDRESS, PCKT_NUM);

		}
		/*to download the FM data from the SMSN FM*/
		else if (MAIN_CMD3 == 0x22) {
			MAIN_FM_MODE();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf(
					"Command to download the data FROM SMSN FM\r\n Address: %x\r\n",
					ADDRESS);
			myprintf("Switching the FM to OBC access mode\r\n");
//			FM_Data_To_PC(&SMSN_FM, &ADDRESS, PCKT_NUM, 0x01);//Transferring SMSN FM data through USB
//			myprintf("Done......\r\n");
#endif
#endif
//			FM_Data_To_PC(&SMSN_FM, &ADDRESS, PCKT_NUM, 0x02);//Transferring SMSN FM data through UART
			TRANSFER_DATA_COM(&SMSN_FM, ADDRESS, PCKT_NUM);
		}
	}

	else if (MAIN_CMD1 == 0xD1 && MAIN_CMD2 == 0xD2 && MAIN_CMD3 == 0xD3) {	//to kill the satellite
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Command to Kill the Satellite received....\r\n");
		myprintf("Satellite Kill switch to be activated........\r\n");
#endif
#endif

		Save_SAT_Log(0xDE, 0xAD, 0xDE, 0xAD);//to indicate that kill switch is activated

	} else if (MAIN_CMD1 == 0xC1 && MAIN_CMD2 == 0xC2 && MAIN_CMD3 == 0xC3) {//to unkill the satellite i.e. to reverse the satellite kill command
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
		myprintf("Command to unkill the Satellite received....\r\n");
		myprintf("Satellite Kill switch to be deactivated........\r\n");
#endif
#endif
		Save_SAT_Log(0xAB, 0xAB, 0xAB, 0xAB);//sat log saved to indicate that kill switch is deactivated
	} else if (MAIN_CMD1 == 0xAA && MAIN_CMD2 == 0xAA && MAIN_CMD3 == 0xAA) {//to do the global reset of the satellite
		Save_SAT_Log(0xAA, 0xAA, 0xAA, 0xAA);//sat log saved to indicate that RESET_COMMAND_RECEIVED
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
		myprintf("Reset Satellite Command received....\r\n");
		myprintf(
				"Shutting down all TIMERs.\r\nSending Reset Satellite command to reset.\r\n");
		BEACON_FLAG = 0;
		HK_FLAG = 0;
		HAL_TIM_Base_Stop_IT(&HK_TIMER);
		HAL_TIM_Base_Stop_IT(&uS_Delay_TIMER);
		HAL_TIM_Base_Stop_IT(&RESET_TIMER);	// timer used for watchdog counter stopped.
		TURN_OFF_CAM_MSN();
		TURN_OFF_PACDS_MSN();
		MAIN_FM_MODE();

#endif
#endif
		Reset_CMD_PWR_ON_OFF(RST_SAT); //sending the command to reset satellite
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"Wrong Command Directed for OBC........... \r\nMAIN_CMD: %x %x %x\r\n Address: %x %x %x %x \r\n PCKT_NUM: %x\r\n",
				MAIN_CMD1, MAIN_CMD2, MAIN_CMD3, ADDR1, ADDR2, ADDR3, ADDR4,
				PCKT_NUM);
#endif
#endif
	}
}

/*
 * @brief		TO EXECUTE THE COMMAND RECEIVED FOR CAM MISSION
 *
 * @param		MCU_ID -- The ID of the MCU (0x03 for CAM MCU) also works as header for CAM
 * 				MAIN_CMD(1-3) -- The Main Command to be executed by the CAM
 *
 * @retval
 */
void EXECUTE_CMD_CAM(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3) {

	memset(MSN1_Data_Buff, '\0', MSN_TX_BUFFER_LEN);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Command on the CAM function.....\r\n");
#endif
#endif

	uint8_t counter = 1;
	uint8_t no_of_subsec = 0;

	MSN1_DATA_TX_FLAG = 0;
	MSN1_DATA_RX_FLAG = 0;
	Check_CAM_Address();		//checking the latest address of CAM MSN
	/*Making the Data to send to the CAM MCU*/
	MSN1_Data_Buff[0] = MCU_ID;
	MSN1_Data_Buff[1] = MAIN_CMD1;
	MSN1_Data_Buff[2] = MAIN_CMD2;
	MSN1_Data_Buff[3] = MAIN_CMD3;
	MSN1_Data_Buff[4] = CAM_MSN_DATA_CURRENT_ADDRESS >> 24;
	MSN1_Data_Buff[5] = CAM_MSN_DATA_CURRENT_ADDRESS >> 16;
	MSN1_Data_Buff[6] = CAM_MSN_DATA_CURRENT_ADDRESS >> 8;
	MSN1_Data_Buff[7] = CAM_MSN_DATA_CURRENT_ADDRESS;
	MSN1_Data_Buff[8] = TIM_HOUR;		//time : Hours
	MSN1_Data_Buff[9] = TIM_MIN;		//time : Minutes
	MSN1_Data_Buff[10] = TIM_SEC;		//time: SEC
	MSN1_Data_Buff[11] = TIM_DAY;	//time: day
	MSN1_Data_Buff[12] = FOOTER;

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Data to be sent to CAM MSN: \r\n");
	myprintf(
			"MCU ID: %02x\r\n MAIN CMD: %02x %02x %02x \r\nAddress: %02x %02x %02x %02x\r\nTime: %02x %02x %02x %02x \r\n",
			MSN1_Data_Buff[0], MSN1_Data_Buff[1], MSN1_Data_Buff[2],
			MSN1_Data_Buff[3], MSN1_Data_Buff[4], MSN1_Data_Buff[5],
			MSN1_Data_Buff[6], MSN1_Data_Buff[7], MSN1_Data_Buff[8],
			MSN1_Data_Buff[9], MSN1_Data_Buff[10], MSN1_Data_Buff[11]);
	myprintf("\r\n");
#endif
#endif
	for (int i = 0; i < 6; i++) {
		Sector_Erase_4B(&SMSN_FM, CAM_MSN_DATA_CURRENT_ADDRESS + i * 4096, 4);
		Sector_Erase_4B(&MAIN_FM, CAM_MSN_DATA_CURRENT_ADDRESS + i * 4096, 4);
	}
	MSN_FM_MODE();//setting the access of the SMSN FM to Mission Side for data storage;only done is the MCU handshake is successful
	HAL_Delay(1000);

	/*Initial MCU Handshake between the OBC and CAM MSN*/
	if (MSN1_HANDSHAKE(MAIN_CMD1, MAIN_CMD2, MAIN_CMD3) == 1) {
		counter = 1;		//if data is received on the first try
	} else {	//switch the CAM MCU off and on and try again
		for (int i = 0; i < 3; i++) {//trying for 3 times to turn on the CAM Mission
			counter++;
			if (MSN1_HANDSHAKE(MAIN_CMD1, MAIN_CMD2, MAIN_CMD3) == 1) {
				break;
			}
		}
	}

	/* if the data is received */
	if (MSN1_DATA_TX_FLAG == 1 && MSN1_DATA_RX_FLAG == 1) {
		MSN1_DATA_TX_FLAG = 0;	//setting the RX & TX flag to zero
		MSN1_DATA_RX_FLAG = 0;
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"MCU Handshake Done at attempt: %d....\r\n MUX switched to MSN access mode\r\n",
				counter);
#endif
#endif
		HAL_Delay(2000);
		/*sending the data to CAM MCU*/
		MainToSubSys(&MSN1_STREAM, MSN1_Data_Buff, MSN_TX_BUFFER_LEN, 0);//sending the MAIN COMMAND TO CAM BOARD
		if (MSN1_DATA_TX_FLAG == 1) {	//if the flag is set, data is sent
			MSN1_DATA_TX_FLAG = 0;
			MSN_STATUS = 1;		//set the mission status flag
			MSN_TIME = 0;//starting the MSN time from zero to wait for max 15 minutes to response from Corresponding Mission
			MSN1_FLAG = 1;
			SAT_HEALTH.MSN1_STAT = MSN1_FLAG;
			SAT_HEALTH.MSN_FLAG = MSN_STATUS;
			//using the same buffer for receiving the data as well so we need to clear it
			memset(MSN1_Data_Buff, '\0', sizeof(MSN1_Data_Buff));

			//TODO: review the mission time, it probably wont take 15 minutes to complete the MSN
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Main Command sent to CAM........\r\n");
			myprintf(
					"CAM UART interrupt activated...\r\nMSN Status: %02 x\r\nMission Time Counter Set to zero\r\n");
#endif
#endif

			//TODO:TESTING this code
			uint32_t tick_start = HAL_GetTick();
			MSN1_DATA_RX_FLAG = 0;
			HAL_Delay(100);
			HAL_UART_Receive_DMA(&MSN1_STREAM, MSN1_Data_Buff,
			MSN_TX_BUFFER_LEN);	//waiting for the Mission Completion ACK, starting the Interrupt

//			while (HAL_UART_Receive(&MSN1_STREAM, MSN1_Data_Buff,
//			MSN_TX_BUFFER_LEN, 500) != HAL_OK) {
//				if (HAL_GetTick() - tick_start > MSN_RUN_TIME) {
//					break;
//				}
//			}
//			MSN1_DATA_RX_FLAG = 1;
//			if (HAL_GetTick() - tick_start > MSN_RUN_TIME) {
//				MSN1_DATA_RX_FLAG = 0;
//			}
			HAL_Delay(1000);
//			MAIN_FM_MODE();
			for (int i = 0; i < 2; i++) {
				RUN_ADC();
				HAL_Delay(1000);
			}
			MSN_FM_MODE();
			while (MSN1_DATA_RX_FLAG != 1) {
				if (HAL_GetTick() - tick_start > MSN_RUN_TIME) {
					MSN1_DATA_RX_FLAG = 0;
					break;
				}
			}
			MSN_STATUS = 0;
			MAIN_FM_MODE();

			if (MSN1_DATA_RX_FLAG == 1) {
				Save_SAT_Log(0xAA, MSN1_Data_Buff[0], MSN1_Data_Buff[1],
						MSN1_Data_Buff[2]);
				if (MSN1_Data_Buff[0] == HEADER
						&& MSN1_Data_Buff[MSN_TX_BUFFER_LEN - 1] == FOOTER
						&& MSN1_Data_Buff[1] == 0x03) {

					uint32_t last_msn_address = MSN1_Data_Buff[5] << 24
							| MSN1_Data_Buff[6] << 16 | MSN1_Data_Buff[7] << 8
							| MSN1_Data_Buff[8];
					uint32_t MSN_DATA_SIZE = last_msn_address
							- CAM_MSN_DATA_CURRENT_ADDRESS;
					Transfer_Bytes_SFM_MFM(CAM_MSN_DATA_CURRENT_ADDRESS,
							MSN_DATA_SIZE);
					no_of_subsec = (MSN_DATA_SIZE / 4096);
					if (MSN_DATA_SIZE % 4096 != 0) {
						no_of_subsec += 1;
					}

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
					myprintf(
							"MSN completion data from MSN1 received.\r\n Copying data from SMSN FM to MFM \r\n no of occupied sub_sec: %i\r\n",
							no_of_subsec);
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

					memset(MSN1_Data_Buff, '\0', DATA_BUFFER_LEN);
					Bulk_Read_4B(&SMSN_FM, CAM_MSN_IMG_INFO_START_ADDRESS,
							MSN1_Data_Buff, 25);
					MSN1_Data_Buff[25] = '\n';
					MSN1_Data_Buff[26] = CAM_MSN_DATA_CURRENT_ADDRESS >> 24
							& 0xFF;
					MSN1_Data_Buff[27] = CAM_MSN_DATA_CURRENT_ADDRESS >> 16
							& 0xFF;
					MSN1_Data_Buff[28] = CAM_MSN_DATA_CURRENT_ADDRESS >> 8
							& 0xFF;
					MSN1_Data_Buff[29] = CAM_MSN_DATA_CURRENT_ADDRESS & 0xFF;
					MSN1_Data_Buff[30] = '\n';
					MSN1_Data_Buff[31] = (CAM_MSN_DATA_CURRENT_ADDRESS
							+ 3 * 4096) >> 24 & 0xFF;
					MSN1_Data_Buff[31] = (CAM_MSN_DATA_CURRENT_ADDRESS
							+ 3 * 4096) >> 16 & 0xFF;
					MSN1_Data_Buff[32] = (CAM_MSN_DATA_CURRENT_ADDRESS
							+ 3 * 4096) >> 8 & 0xFF;
					MSN1_Data_Buff[33] = (CAM_MSN_DATA_CURRENT_ADDRESS
							+ 3 * 4096) & 0xFF;

					Page_Write_4B(&MAIN_FM, CAM_MSN_IMG_INFO_CURRENT_ADDRESS,
							MSN1_Data_Buff, 34);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
					#else
					myprintf("Text File Read: ");

					for (int i = 0; i < 35; i++) {
						myprintf("%02x ", MSN1_Data_Buff[i]);
					}
					myprintf("\r\n");
#endif
#endif

					CAM_MSN_DATA_CURRENT_ADDRESS += no_of_subsec * 4096;// moving MSN data storage one 4KB subsector forward
					CAM_MSN_IMG_INFO_CURRENT_ADDRESS += 40;

					Write_Address_Data();
				} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
					myprintf("MSN completion ACK MISMATCH.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
				}
			} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
				myprintf("MSN1 execution timeout.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			}

		}
		//if data is not sent to CAM AFTER handshake
		else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data not sent to CAM AFTER handshake....\r\n");
#endif
#endif
			memset(MSN1_Data_Buff, '\0', sizeof(MSN1_Data_Buff));
		}
	}
	/*if data is not received i.e. handshake is not done*/
	else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("MCU Handshake not done no of attempts: %d....\r\n", counter);
#endif
#endif
	}
	MSN1_FLAG = 0;
	if (MSN2_FLAG == 0) {
		MSN_STATUS = 0;
	}
	SAT_HEALTH.MSN1_STAT = MSN1_FLAG;
	SAT_HEALTH.MSN_FLAG = MSN_STATUS;
}

/*
 * @brief		TO EXECUTE THE COMMAND RECEIVED FOR SPDM MISSION
 * 				SPDM is the MSN 3
 */
void EXECUTE_CMD_SPDM(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3) {

	memset(MSN3_Data_Buff, '\0', sizeof(MSN3_Data_Buff));
	uint8_t counter = 1;

	MSN3_DATA_RX_FLAG = 0;
	MSN3_DATA_TX_FLAG = 0;

	Check_SPDM_Address();		//updating the current SPDM mission data

	/* making the data to send to the SPDM MSN */
	MSN3_Data_Buff[0] = MCU_ID;
	MSN3_Data_Buff[1] = MAIN_CMD1;
	MSN3_Data_Buff[2] = MAIN_CMD2;
	MSN3_Data_Buff[3] = MAIN_CMD3;

	MSN3_Data_Buff[4] = SPDM_MSN_DATA_CURRENT_ADDRESS >> 24;
	MSN3_Data_Buff[5] = SPDM_MSN_DATA_CURRENT_ADDRESS >> 16;
	MSN3_Data_Buff[6] = SPDM_MSN_DATA_CURRENT_ADDRESS >> 8;
	MSN3_Data_Buff[7] = SPDM_MSN_DATA_CURRENT_ADDRESS;

	MSN3_Data_Buff[8] = 12;		//time: Hours
	MSN3_Data_Buff[9] = 19;		//time: Minutes

	MSN3_Data_Buff[10] = 9;		//time: Month
	MSN3_Data_Buff[11] = 10;	//time: day

	MSN3_Data_Buff[12] = FOOTER;

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Data to be sent to SPDM MSN: \r\n");
	myprintf(
			"MCU ID: %x\r\n MAIN CMD: %x %x %x \r\nAddress: %x %x %x %x\r\nTime: %x %x %x %x \r\n",
			MSN3_Data_Buff[0], MSN3_Data_Buff[1], MSN3_Data_Buff[2],
			MSN3_Data_Buff[3], MSN3_Data_Buff[4], MSN3_Data_Buff[5],
			MSN3_Data_Buff[6], MSN3_Data_Buff[7], MSN3_Data_Buff[8],
			MSN3_Data_Buff[9], MSN3_Data_Buff[10], MSN3_Data_Buff[11]);
	myprintf("\r\n");
#endif
#endif

	HAL_Delay(8000);	// delay 8 secs to let the hardware initalize

	/*Initial MCU Handshake between the OBC and SPDM MSN*/
	if (MSN3_HANDSHAKE() == 1) {
		counter = 1;
	} else {
//		TURN_OFF_SPDM_MSN();
//		TURN_ON_SPDM_MSN();	//switch the SPDM MCU off and on and try again
		HAL_Delay(100);
		for (int i = 0; i < 3; i++) {//trying for 3 times to turn on and handshake the SPDM Mission
			counter++;
			if (MSN3_HANDSHAKE() == 1) {
				break;
			}
//			TURN_OFF_SPDM_MSN();
//			TURN_ON_SPDM_MSN();		//toggling the SPDM MCU on and off
			HAL_Delay(500);
		}
	}

	/* if the handshake is done */
	if (MSN3_DATA_TX_FLAG == 1 && MSN3_DATA_RX_FLAG == 1) {

		MSN3_DATA_TX_FLAG = 0;	//setting the RX & TX flag to zero
		MSN3_DATA_RX_FLAG = 0;

		MSN_FM_MODE();		//switching the MUX to MSN side

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"SPDM MCU Handshake Done at attempt: %d....\r\n MUX switched to MSN access mode\r\n",
				counter);
#endif
#endif
		/*sending the data to SPDM MCU*/
		MainToSubSys(&MSN3_STREAM, MSN3_Data_Buff, MSN_TX_BUFFER_LEN, 0);
		if (MSN3_DATA_TX_FLAG == 1) {	//if the flag is set, data is sent

			Sector_Erase_4B(&SMSN_FM, SPDM_MSN_DATA_CURRENT_ADDRESS, 4);
			Sector_Erase_4B(&MAIN_FM, SPDM_MSN_DATA_CURRENT_ADDRESS, 4);

			//using the same buffer for receiving the data as well so we need to clear it
			memset(MSN3_Data_Buff, '\0', sizeof(MSN3_Data_Buff));

			memset(MSN3_Data_Buff, '\0', sizeof(MSN3_Data_Buff));
			HAL_UART_Receive_DMA(&MSN3_STREAM, MSN3_Data_Buff, 32);	// for EM only

			MSN3_DATA_RX_FLAG = 0;
			MSN_STATUS = 1;
			MSN3_FLAG = 1;
			MSN_TIME = 0;

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Main Command sent to SPDM........\r\n");
			myprintf("SPDM UART interrupt activated...\r\nMSN Status: %x\r\n",
					MSN_STATUS);
#endif
#endif
			uint32_t current_tick = HAL_GetTick();
			while (MSN3_DATA_RX_FLAG != 1) {
				if (HAL_GetTick() - current_tick > MSN_RUN_TIME) {
					break;
				}
			}
			MSN_STATUS = 0;
			MSN3_FLAG = 1;

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("Data received from SPDM MSN.\r\n");
			for (uint8_t i = 0; i < 32; i++) {
				myprintf("%02x ", MSN3_Data_Buff[i]);
//				HAL_Delay(1);
			}
			myprintf("\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

			MSN_STATUS = 0;
			MAIN_FM_MODE();

			Save_SAT_Log(0xAA, MSN3_Data_Buff[0], MSN3_Data_Buff[1],
					MSN3_Data_Buff[2]);

			if (MSN3_Data_Buff[0] == HEADER && MSN3_Data_Buff[31] == FOOTER
					&& MSN3_Data_Buff[1] == 0x05) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
				myprintf(
						"MSN completion data from MSN3 received.\r\n Writing data to SMSN FM and MFM");
#else
						#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

				Page_Write_4B(&SMSN_FM, SPDM_MSN_DATA_CURRENT_ADDRESS,
						MSN3_Data_Buff, 32);
				Transfer_Bytes_SFM_MFM(SPDM_MSN_DATA_CURRENT_ADDRESS, 32);

				SPDM_MSN_DATA_CURRENT_ADDRESS += 4096;// moving MSN data storage one 4KB subsector forward
				Write_Address_Data();
			} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
				myprintf("Correct data order not received.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			}

		} else {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data not sent to SPDM after handshake....\r\n");

#endif
#endif
			memset(MSN3_Data_Buff, '\0', sizeof(MSN3_Data_Buff));
		}
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("MCU Handshake not done no of attempts: %d....\r\n", counter);
#endif
#endif
		memset(MSN3_Data_Buff, '\0', sizeof(MSN3_Data_Buff));
	}

}

void EXECUTE_CMD_PACDS(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3) {
	memset(MSN2_Data_Buff, '\0', sizeof(MSN2_Data_Buff));
	uint8_t counter = 1;

	MSN2_DATA_RX_FLAG = 0;
	MSN2_DATA_TX_FLAG = 0;

	Check_PACDS_Address();		//updating the current SPDM mission data

	/* making the data to send to the SPDM MSN */
	MSN2_Data_Buff[0] = MCU_ID;
	MSN2_Data_Buff[1] = MAIN_CMD1;
	MSN2_Data_Buff[2] = MAIN_CMD2;
	MSN2_Data_Buff[3] = MAIN_CMD3;

	MSN2_Data_Buff[4] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 24;
	MSN2_Data_Buff[5] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 16;
	MSN2_Data_Buff[6] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 8;
	MSN2_Data_Buff[7] = PACDS_MSN_DATA_CURRENT_ADDRESS;

	MSN2_Data_Buff[8] = TIM_HOUR;		//time: Hours
	MSN2_Data_Buff[9] = TIM_MIN;		//time: Minutes

	MSN2_Data_Buff[10] = TIM_SEC;		//time: Month
	MSN2_Data_Buff[11] = TIM_DAY;	//time: day

	MSN2_Data_Buff[12] = FOOTER;

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Data to be sent to PACDS MSN: \r\n");

	myprintf(
			"MCU ID: %x\r\n MAIN CMD: %x %x %x \r\nAddress: %x %x %x %x\r\nTime: %x %x %x %x \r\n",
			MSN2_Data_Buff[0], MSN2_Data_Buff[1], MSN2_Data_Buff[2],
			MSN2_Data_Buff[3], MSN2_Data_Buff[4], MSN2_Data_Buff[5],
			MSN2_Data_Buff[6], MSN2_Data_Buff[7], MSN2_Data_Buff[8],
			MSN2_Data_Buff[9], MSN2_Data_Buff[10], MSN2_Data_Buff[11]);

	myprintf("\r\n");
#endif
#endif
	HAL_Delay(10000);			//waiting for MSN2 to get intialized

	/*Initial MCU Handshake between the OBC and PACDS MSN*/
	if (MSN2_HANDSHAKE() == 1) {
		counter = 1;
	} else {
		HAL_Delay(500);
		for (int i = 0; i < 3; i++) {//trying for 3 times to turn on the SPDM Mission
			counter++;
			if (MSN2_HANDSHAKE() == 1) {
				break;
			}
			HAL_Delay(500);	// delay 500ms before trying again
		}
	}

	/* if the handshake is done */
	if (MSN2_DATA_TX_FLAG == 1 && MSN2_DATA_RX_FLAG == 1) {

		MSN2_DATA_TX_FLAG = 0;	//setting the RX & TX flag to zero
		MSN2_DATA_RX_FLAG = 0;

		Sector_Erase_4B(&SMSN_FM, PACDS_MSN_DATA_CURRENT_ADDRESS, 4);
		HAL_Delay(10);
		MSN_FM_MODE();		//switching the MUX to MSN side
		HAL_Delay(10);

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"PACDS MCU Handshake Done at attempt: %d....\r\n MUX switched to MSN access mode\r\n",
				counter);
#endif
#endif
		/*sending the data to SPDM MCU*/
		MainToSubSys(&MSN2_STREAM, MSN2_Data_Buff, MSN_TX_BUFFER_LEN, 0);
		if (MSN2_DATA_TX_FLAG == 1) {	//if the flag is set, data is sent

			//using the same buffer for receiving the data as well so we need to clear it
			memset(MSN2_Data_Buff, '\0', DATA_BUFFER_LEN);
			HAL_Delay(1000);

			MSN2_DATA_RX_FLAG = 0;
			HAL_UART_Receive_DMA(&MSN2_STREAM, MSN2_Data_Buff,
			MSN_TX_BUFFER_LEN);

			MSN_STATUS = 1;		//to indicate that a mission is ongoing
			MSN2_FLAG = 1;
			MSN_TIME = 0;
			SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
			SAT_HEALTH.MSN_FLAG = MSN_STATUS;
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Main Command sent to PACDS........\r\n");
			HAL_Delay(1);
			myprintf(
					"PACDS UART interrupt activated(MSN2_STREAM)...\r\nMSN Status: %d\r\n",
					MSN_STATUS);
#endif
#endif

			//TODO: add the MSN2 completion interrupt handler
			//for EM model, blocking mode operation of MSNs
			uint32_t tick_start = HAL_GetTick();
			MAIN_FM_MODE();
			RUN_ADC();
			Write_Address_Data();
			MSN_FM_MODE();
			while (MSN2_DATA_RX_FLAG != 1) {
				if (HAL_GetTick() - tick_start > MSN_RUN_TIME) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
					myprintf("MSN2_TIMEOUT.\r\n");
#else
					#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
					MSN2_DATA_RX_FLAG = 0;
					break;
				}

			}
			MSN_STATUS = 0;
			MAIN_FM_MODE();

			if (MSN2_DATA_RX_FLAG) {
				Save_SAT_Log(0xAA, MSN2_Data_Buff[0], MSN2_Data_Buff[1],
						MSN2_Data_Buff[2]);
				if (MSN2_Data_Buff[0] == HEADER
						&& MSN2_Data_Buff[MSN_TX_BUFFER_LEN - 1] == FOOTER
						&& MSN2_Data_Buff[1] == 0x04) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
					myprintf(
							"MSN completion data from MSN2 received.\r\n Copying data from SMSN FM to MFM");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
					uint32_t last_msn_address = MSN2_Data_Buff[5] << 24
							| MSN2_Data_Buff[6] << 16 | MSN2_Data_Buff[7] << 8
							| MSN2_Data_Buff[8];
					uint32_t MSN_DATA_SIZE = last_msn_address
							- PACDS_MSN_DATA_CURRENT_ADDRESS;
					Transfer_Bytes_SFM_MFM(PACDS_MSN_DATA_CURRENT_ADDRESS,
							MSN_DATA_SIZE);
					PACDS_MSN_DATA_CURRENT_ADDRESS += 4096;	// moving MSN data storage one 4KB subsector forward
					Write_Address_Data();

				} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
					myprintf("MSN completion ACK MISMATCH.\r\n");

#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
				}
			}

		} else {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data not sent to PACDS after handshake....\r\n");

#endif
#endif
			memset(MSN2_Data_Buff, '\0', sizeof(MSN2_Data_Buff));
		}
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("MCU Handshake with PACDS not done no of attempts: %d....\r\n",
				counter);
#endif
#endif
		memset(MSN2_Data_Buff, '\0', sizeof(MSN2_Data_Buff));
	}
	MSN2_DATA_TX_FLAG = 0;
	MSN2_DATA_RX_FLAG = 0;
	MSN2_FLAG = 0;
	if (MSN1_FLAG == 0) {
		MSN_STATUS = 0;
	}
	SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
	SAT_HEALTH.MSN_FLAG = MSN_STATUS;
}

/*
 * @brief 	to send command to reset to turn the power lines on and off
 *
 * @param	1 -- for turning on the 3v3 2 line
 * 			2 -- to turn on the 5v line
 * 			3 -- to turn on the UNREG_1 [COM line]
 * 			4 -- to turn on the UNREG_2 [ANT Line]
 *
 * 			10 -- for turning off the 3v3 2 line
 * 			20 -- to turn off the 5V line
 * 			30 -- to turn off the UNREG_1 [COM line]
 * 			40 -- to turn off the UNREG_2 [ANT LINE]
 *
 * @retval 	none
 */
void Reset_CMD_PWR_ON_OFF(uint8_t MSN) {

	uint8_t Rx_Buff[3] = { '\0' };
	RESET_DATA_RX_FLAG = 0;
	RESET_DATA_TX_FLAG = 0;
	switch (MSN) {
	case 1:
		UPDATE_RST_DATA(&RESET_Tx, TURN_ON_3V3_2);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning on 3V3_2.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 2:
		UPDATE_RST_DATA(&RESET_Tx, TURN_ON_5V);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning on 5V.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 3:
		UPDATE_RST_DATA(&RESET_Tx, TURN_ON_UNREG_1);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning on UNREG_1.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 4:
		UPDATE_RST_DATA(&RESET_Tx, TURN_ON_UNREG_2);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning on UNREG_2.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 10:
		UPDATE_RST_DATA(&RESET_Tx, TURN_OFF_3V3_2);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning off 3V3_2.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 20:
		UPDATE_RST_DATA(&RESET_Tx, TURN_OFF_5V);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning off 5V.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 30:
		UPDATE_RST_DATA(&RESET_Tx, TURN_OFF_UNREG_1);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning off UNREG_1.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case 40:
		UPDATE_RST_DATA(&RESET_Tx, TURN_OFF_UNREG_2);
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Turning off UNREG_2.\r\n");
#endif //TRACE_MODE
		}
#endif	//DEBUG_MODE
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;

	case RST_SAT:
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("OBC [SAT] reset command sending to reset.\r\n");
#endif //TRACE_MODE

#endif	//DEBUG_MODE
		//add the command transmission to reset OBC here
		UPDATE_RST_DATA(&RESET_Tx, RST_SAT);
//		MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
		break;
	case RST_FLAG_RST:
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Flag Reset command sending to reset.\r\n");
#endif //TRACE_MODE

#endif	//DEBUG_MODE
		UPDATE_RST_DATA(&RESET_Tx, RST_FLAG_RST);
		break;

	default:
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Wrong command for Reset....\r\n");
#endif
#endif
		memset(&RESET_Rx, '\0', RESET_DATA_RX_LEN);
		return;
		break;
	}
	MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);
	MainToSubSys(&RESET_ALT_STREAM, (uint8_t*) &RESET_Tx, RESET_DATA_TX_LEN, 1);

	uint32_t current_tick = HAL_GetTick();
	while (!RESET_DATA_TX_FLAG) {
		if (HAL_GetTick() - current_tick > 2000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("RESET_CMD_TX_TIMEOUT.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			break;
		}

	}
	if (RESET_DATA_TX_FLAG) {
		RESET_DATA_TX_FLAG = 0;
		current_tick = HAL_GetTick();
		while (!RESET_DATA_RX_FLAG) {
			if (HAL_GetTick() - current_tick > 2000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
				myprintf("RESET_ACK_RX_TIMEOUT.\r\n");
#else
					#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
//				HAL_UART_Receive_IT(&RESET_STREAM, RESET_Data_Buff, RESET_DATA_RX_LEN);
				break;
			}

		}

		if (RESET_DATA_RX_FLAG) {
			memcpy(&RESET_Rx, RESET_Data_Buff, RESET_DATA_RX_LEN);
			uint8_t status = Check_Reset_Data();

			if (status) {
#ifdef DEBUG_MODE
				if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
					myprintf(
							"Command sent successfully to Reset\r\nACK success received\r\n");

#endif
				}
#endif
			} else if (Rx_Buff[1] == 0xEE) {
#ifdef DEBUG_MODE
				if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
					myprintf(
							"Command sent successfully to Reset\r\nACK Failure received\r\n");

#endif
				}
#endif
			}
		}
	} else {
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
		#else
			myprintf("Reset Command not transmitted.\r\n");
#endif
		}
#endif
	}

}

uint8_t MSN3_HANDSHAKE() {
	uint8_t temp[5];
	uint8_t temp_rx[5] = { '\0' };

	temp[0] = 0x04;
	temp[1] = 0xab;
	temp[2] = 0x90;
	temp[3] = 0x11;
	temp[4] = FOOTER;
	MainToSubSys(&MSN3_STREAM, temp, 5, 0);
	if (MSN3_DATA_TX_FLAG == 1) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Data sent to SPDM\r\n");
		HAL_Delay(1);
#endif
#endif

		//waiting for 2 seconds to receive data from SPDM
		if (HAL_UART_Receive(&MSN3_STREAM, temp_rx, 5, 2000) == HAL_OK) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data received from SPDM\r\n");
			HAL_Delay(1);
//			for (int i = 0; i < 5; i++) {
//				myprintf("%x", temp_rx[i]);
//				HAL_Delay(1);
//			}
#endif
#endif
			MSN3_DATA_RX_FLAG = 1;		//to indicate that data is received
			return 1;
		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data not received from SPDM\r\n");
			HAL_Delay(1);
#endif
#endif
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data communication timeout\r\n");
			HAL_Delay(1);
#endif
#endif
		}

	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Data not sent to SPDM\r\n");
		HAL_Delay(1);
#endif
#endif
		return 0;
	}
	return 0;

}

/*
 * @brief	to communicate with Mission 2(PACDS MSN)
 *
 * @param	none
 *
 * @retval	none
 */
uint8_t MSN2_HANDSHAKE() {
	uint8_t temp[5];
	uint8_t temp_rx[5] = { '\0' };

	temp[0] = 0x04;
	temp[1] = 0xab;
	temp[2] = 0x90;
	temp[3] = 0x11;
	temp[4] = FOOTER;
	HAL_UART_Transmit_IT(&MSN2_STREAM, temp, 5);
	uint32_t tick_now = HAL_GetTick();
	while (!MSN2_DATA_TX_FLAG) {
		if (HAL_GetTick() - tick_now > 3000) {
			break;
		}
	}
	if (MSN2_DATA_TX_FLAG == 1) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Handshake Data sent to PACDS\r\n");
		HAL_Delay(1);
#endif
#endif
		if (HAL_UART_Receive(&MSN2_STREAM, temp_rx, 5, 2000) == HAL_OK) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("ACK Data received from PACDS\r\n");
			HAL_Delay(1);
//			for (int i = 0; i < 5; i++) {
//				myprintf("%x", temp_rx[i]);
//				HAL_Delay(1);
//			}
//			myprintf("\r\n");
//			HAL_Delay(1);
#endif
#endif
			MSN2_DATA_RX_FLAG = 1;
			return 1;
		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("ACK Data not received from PACDS\r\n");
			HAL_Delay(1);
#endif
#endif
			return 0;
		}
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Handshake Data not sent to PACDS\r\n");
		HAL_Delay(1);
#endif
#endif
		return 0;
	}
	return 0;
}

/*
 * @brief	to communicate with Mission 1(CAM MSN)
 *
 * @param	none
 *
 * @retval	none
 */
uint8_t MSN1_HANDSHAKE(uint8_t MAIN_CMD1, uint8_t MAIN_CMD2, uint8_t MAIN_CMD3) {
	uint8_t temp[5];
	uint8_t temp_rx[5] = { '\0' };
	MSN1_DATA_RX_FLAG = 0;
	temp[0] = 0x03;
	temp[1] = 0x99;
	temp[2] = 0xab;
	temp[3] = 0xae;
	temp[4] = FOOTER;
	MainToSubSys(&MSN1_STREAM, temp, 5, 0);
	HAL_Delay(1);
	HAL_UART_Receive_DMA(&MSN1_STREAM, temp_rx, 5);
	if (MSN1_DATA_TX_FLAG == 1) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Handshake Data sent to CAM\r\n");
		HAL_Delay(1);
#endif
#endif
		uint32_t tick = HAL_GetTick();
		while (MSN1_DATA_RX_FLAG != 1) {
			if (HAL_GetTick() - tick > 2000) {
				break;
			}
		}

		if (MSN1_DATA_RX_FLAG == 1) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("ACK Data received from CAM\r\n");
			HAL_Delay(1);
			for (int i = 0; i < 5; i++) {
				myprintf("%x", temp_rx[i]);
				HAL_Delay(1);
			}
			myprintf("\r\n");
			HAL_Delay(1);
#endif
#endif
			MSN1_DATA_RX_FLAG = 1;
			return 1;
		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("ACK Data not received from CAM\r\n");
			HAL_Delay(1);
#endif
#endif
			return 0;
		}
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Handshake Data not sent to CAM\r\n");
		HAL_Delay(1);
#endif
#endif
		return 0;
	}
}

/*
 * @brief	to receive the MSN data after sending the command
 * 			when the data is received and the flag is set, we need to turn off the mission
 *
 * 	@param	none
 *
 * 	@retval	none
 */
void Check_Rx_Data_MSN() {

	/*for the data received from the CAM subsystem*/
	if (MSN1_DATA_RX_FLAG == 1) {
		Save_SAT_Log(0xAA, MSN1_Data_Buff[0], MSN1_Data_Buff[1],
				MSN1_Data_Buff[2]);	//to indicate that the mission completion ack is received
		uint32_t initial_address = 0;
		if (MSN1_Data_Buff[0] == 0x03 && MSN1_Data_Buff[12] == 0xfe) {

			/* it means data is in correct order, no byte is missed */
			TURN_OFF_CAM_MSN();		//turning of CAM MSN

			MAIN_FM_MODE();			//switching the SMSN FM to Main FM mode

			uint8_t PHOTO_SIZE = MSN1_Data_Buff[8];

			/* To be done after the Communication and data storage is complete, to check if photo is stored in the FM
			 * If photo is not stored then get the data from CAM and store in the MAIN FM by OBC
			 */
//			uint8_t DATA_STORE_STATUS = MSN1_Data_Buff[9];	//to check if photo is stored or not
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf(
					"DATA Received from CAM MCU\r\nCAM Mission Turned off.......\r\n Photo Size(in KB): %d\r\n",
					PHOTO_SIZE);
			HAL_Delay(1);
			myprintf(
					"Data received from CAM:\r\n MAIN_CMD: %x %x %x\r\n ADDRESS: %x %x %x %x\r\n Other data: %x %x %x %x\r\n",
					MSN1_Data_Buff[1], MSN1_Data_Buff[2], MSN1_Data_Buff[3],
					MSN1_Data_Buff[4], MSN1_Data_Buff[5], MSN1_Data_Buff[6],
					MSN1_Data_Buff[7], MSN1_Data_Buff[8], MSN1_Data_Buff[9],
					MSN1_Data_Buff[10], MSN1_Data_Buff[11]);
			HAL_Delay(1);
#endif
#endif
		} else {
			TURN_OFF_CAM_MSN();
			MAIN_FM_MODE();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf(
					"MSN completion ack received but not in correct order....\r\n");
			HAL_Delay(1);
			for (int i = 0; i < 14; i++) {
				myprintf("%x", MSN1_Data_Buff[i]);
				HAL_Delay(1);
			}
#endif
#endif
			//TODO: figure out the else part
		}
		initial_address = CAM_MSN_DATA_CURRENT_ADDRESS;
		CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_DATA_CURRENT_ADDRESS
				+ ((1024 * 13) + 1024);	// for now, 13 sectors is allocated assuming maximum photo size is around 800KB, extra 1 sector buffer is given

		Write_Address_Data();//storing the latest address data to the Flash Memory after updating

		Transfer_Sector_SFM_MFM(initial_address, 14);	//14 sectors to copy

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Camera MSN turned off...\r\n");
		HAL_Delay(1);
#endif
#endif

	}
	/*If data is received from PACDS MSN*/
	else if (MSN2_DATA_RX_FLAG == 1) {
		Save_SAT_Log(0xAA, MSN2_Data_Buff[0], MSN2_Data_Buff[1],
				MSN2_Data_Buff[2]);	//to indicate that the command is from PACDS
		if (MSN2_Data_Buff[0] == HEADER
				&& MSN2_Data_Buff[1] == 0x04&& MSN2_Data_Buff[12] == FOOTER) {

//			MainToSubSys(&MSN2_STREAM, data, len, mode)
			TURN_OFF_PACDS_MSN();
			MAIN_FM_MODE();
			HAL_Delay(100);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf(
					"Mission of PACDS Complete\r\n ACK Received in Correct orderPACDS MSN Turned off\r\n");
			HAL_Delay(1);
#endif
#endif
			PACDS_MSN_DATA_CURRENT_ADDRESS = PACDS_MSN_DATA_CURRENT_ADDRESS
					+ 14;

		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data received from PACDS BUT not in correct order\r\n");
			HAL_Delay(1);
#endif
#endif
			PACDS_MSN_DATA_CURRENT_ADDRESS = PACDS_MSN_DATA_CURRENT_ADDRESS
					+ 14;
			TURN_OFF_PACDS_MSN();
			MAIN_FM_MODE();
			HAL_Delay(100);
		}

	}

	/*If data is received from SSOC MSN*/
	else if (MSN3_DATA_RX_FLAG == 1) {
		Save_SAT_Log(0xAA, MSN3_Data_Buff[0], MSN3_Data_Buff[1],
				MSN3_Data_Buff[2]);	//to indicate that the command is
		if (MSN3_Data_Buff[0] == 0x05 && MSN3_Data_Buff[12] == 0xfe) {
			TURN_OFF_SPDM_MSN();
			MAIN_FM_MODE();
			HAL_Delay(100);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf(
					"Mission of SPDM Complete\r\n Turning off the SPDM MSN\r\n");
			HAL_Delay(1);
#endif
#endif
			SPDM_MSN_DATA_CURRENT_ADDRESS = SPDM_MSN_DATA_CURRENT_ADDRESS + 50;	//assuming that SSOC has maximum 20 bytes of data
			Write_Address_Data();
		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Data received but not in correct order...\r\n");
			HAL_Delay(1);
#endif
#endif
		}
	} else {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"Data not received from any of the systems but MSN Status is set....\r\n");
		HAL_Delay(1);
#endif
#endif

		return;
	}
}

/*
 * @brief	to run the antenna deployment sequence
 * 			to be done only one time after the satellite is deployed into orbit
 */
uint8_t Antenna_Deployment() {

	memset(RESET_Data_Buff, '\0', sizeof(RESET_Data_Buff));

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("Antenna deployment sequence Starting. \r\n");
#else
	#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	while (TIM_MIN < 30) {
	}

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("time: %02d:%02d:%02d:%02d", TIM_DAY, TIM_HOUR, TIM_MIN, TIM_SEC);
	myprintf("Antenna deployment sequence\r\n");
	myprintf("Waiting to receive data from reset...\r\n", TIM_SEC);
#endif
#endif

	uint32_t tick = HAL_GetTick();

	while (RESET_DATA_RX_FLAG == 0) {
		if (HAL_GetTick() - tick > 30000) {
			myprintf(
					"Data not received from Reset even after 30 seconds of waiting\r\n");
			break;
		}
	}

	memcpy(&RESET_Rx.HEAD, RESET_Data_Buff, RESET_DATA_RX_LEN);
	Check_Reset_Data();
	if (ANT_DEPLOY_STATUS == DEPLOY_NOW) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Antenna deployment sequence Data received from Reset\r\n "
				"Power Line Turned on.\r\n");
#endif
#endif
		for (int Burner_attempt = 1; Burner_attempt <= 3; Burner_attempt++) {//to turn on and off the photomos sensor 3 times
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("burner attempt: %i\r\n", Burner_attempt);
#else
							#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			HAL_GPIO_WritePin(BUNER_SIGNAL_GPIO_Port,
			BUNER_SIGNAL_Pin, SET); //turning on the photomos
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("burner on\r\n");
#else
							#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			RUN_ADC();
			HAL_Delay(8000); //turn on the burner circuit for 8 seconds
			HAL_GPIO_WritePin(BUNER_SIGNAL_GPIO_Port,
			BUNER_SIGNAL_Pin, RESET); //turning off the photomos
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("burner off.\r\n");
#else
							#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			Write_Address_Data();
//			HAL_Delay(500); //turn off the burner circuit and turn it on again after a second
			if (Burner_attempt == 3) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf("burner circuit procedure completed...\r\n%d\r\n",
						Burner_attempt);
#endif
#endif
			}
		}
		ANT_DEPLOY_STATUS = 0xDE; //setting the antenna deployment flag
		SAT_HEALTH.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
		Store_Flag_Data(); //storing the flag data into the FM
		Reset_CMD_PWR_ON_OFF(40); //sending command to reset to Turn off the Unreg line
		return 1;

	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"data not received From Reset....\r\nAntenna not Deployed...\r\n");
#endif
#endif
		return 0;
	}

	return 0;
}

/*
 * EXT ADC1 gets the data of Solar Panel voltages and the voltage of battery
 *
 * EXT ADC2 gets the temperature of the Solar panels and battery and BPB temperature
 */
void GET_EXT_ADC_DATA() {
	/*TODO: use actual function to get external adc data*/
	memset(E_ADC0_RAW, '\0', sizeof(E_ADC0_RAW));
	memset(E_ADC1_RAW, '\0', sizeof(E_ADC1_RAW));
	memset(EXT_ADC_0, '\0', sizeof(EXT_ADC_0));
	memset(EXT_ADC_1, '\0', sizeof(EXT_ADC_1));
	memset(EXT_ADC_0_TEMP, '\0', sizeof(EXT_ADC_0_TEMP));

	ADC_Operate(E_ADC0_RAW, E_ADC1_RAW);
	ADC_Conv_Data(EXT_ADC_0, E_ADC0_RAW);
	ADC1_Conv_Data(EXT_ADC_1, E_ADC1_RAW);
	ADC_Temp_Conv(EXT_ADC_0, EXT_ADC_0_TEMP);

}

void GET_PACDS_GYRO_DATA() {
	memset(MSN2_Data_Buff, '\0', sizeof(MSN2_Data_Buff));

	//hk data request command
	MSN2_Data_Buff[0] = 0x04;
	MSN2_Data_Buff[1] = 0x0e;
	MSN2_Data_Buff[2] = 0x54;
	MSN2_Data_Buff[3] = 0xCF;

	MSN2_Data_Buff[4] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 24;
	MSN2_Data_Buff[5] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 16;
	MSN2_Data_Buff[6] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 8;
	MSN2_Data_Buff[7] = PACDS_MSN_DATA_CURRENT_ADDRESS;

	MSN2_Data_Buff[8] = TIM_HOUR;		//time: Hours
	MSN2_Data_Buff[9] = TIM_MIN;		//time: Minutes

	MSN2_Data_Buff[10] = TIM_SEC;		//time: Month
	MSN2_Data_Buff[11] = TIM_DAY;	//time: day

	MSN2_Data_Buff[12] = FOOTER;

	uint8_t temp_rx[3];
	uint32_t start_tick = HAL_GetTick();

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("HK data Req command [PACDS] ready.\r\n");
#else
	#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	HAL_Delay(8000);
	while (MSN2_HANDSHAKE() != 1) {
		if (HAL_GetTick() - start_tick > 5000) {
			MSN2_DATA_TX_FLAG = 0;
			MSN2_DATA_RX_FLAG = 0;
			break;
		}
		HAL_Delay(500);
	}

	if (MSN2_DATA_TX_FLAG && MSN2_DATA_RX_FLAG) {

		MSN2_DATA_TX_FLAG = 0;
		MSN2_DATA_RX_FLAG = 0;

		uint32_t start_tick = HAL_GetTick();
		do {
			HAL_Delay(200);
			HAL_UART_Transmit_IT(&MSN2_STREAM, MSN2_Data_Buff,
			MSN_TX_BUFFER_LEN);
			if (HAL_GetTick() - start_tick > HK_DATA_WAIT_DELAY) {
				break;
			}
		} while (HAL_UART_Receive(&MSN2_STREAM, temp_rx, 3, 100) != HAL_OK);

		MSN2_DATA_RX_FLAG = 0;
		start_tick = HAL_GetTick();
		if (MSN2_DATA_TX_FLAG) {
			MSN2_DATA_TX_FLAG = 0;
			MSN2_DATA_RX_FLAG = 0;
			memset(SAT_IMU, '\0', sizeof(SAT_IMU));
			HAL_UART_Receive_IT(&MSN2_STREAM, &SAT_IMU[0][0], sizeof(SAT_IMU));
			while (!MSN2_DATA_RX_FLAG) {
				if ((HAL_GetTick() - start_tick) > HK_DATA_WAIT_DELAY) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
					myprintf("ADS HK_Data collection TIMEOUT.\r\n");
#else
					#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
					break;
				}
			}

			if (MSN2_DATA_RX_FLAG) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
				myprintf("ADS HK data collection completed.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
				SAT_HEALTH.ACCL_X = SAT_IMU[0][0] << 8 | SAT_IMU[0][1];
				SAT_HEALTH.ACCL_Y = SAT_IMU[1][0] << 8 | SAT_IMU[1][1];
				SAT_HEALTH.ACCL_Z = SAT_IMU[2][0] << 8 | SAT_IMU[2][1];
				SAT_HEALTH.Gyro_X = SAT_IMU[3][0] << 8 | SAT_IMU[3][1];
				SAT_HEALTH.Gyro_Y = SAT_IMU[4][0] << 8 | SAT_IMU[4][1];
				SAT_HEALTH.Gyro_Z = SAT_IMU[5][0] << 8 | SAT_IMU[5][1];
				SAT_HEALTH.MAG_X = SAT_IMU[6][0] << 8 | SAT_IMU[6][1];
				SAT_HEALTH.MAG_Y = SAT_IMU[7][0] << 8 | SAT_IMU[7][1];
				SAT_HEALTH.MAG_Z = SAT_IMU[8][0] << 8 | SAT_IMU[8][1];

				BEACON_TYPE_B.ACCL_X = SAT_HEALTH.ACCL_X;
				BEACON_TYPE_B.ACCL_Y = SAT_HEALTH.ACCL_Y;
				BEACON_TYPE_B.ACCL_Z = SAT_HEALTH.ACCL_Z;
				BEACON_TYPE_B.GYRO_X = SAT_HEALTH.Gyro_X;
				BEACON_TYPE_B.GYRO_Y = SAT_HEALTH.Gyro_Y;
				BEACON_TYPE_B.GYRO_Z = SAT_HEALTH.Gyro_Z;
				BEACON_TYPE_B.MAG_X = SAT_HEALTH.MAG_X;
				BEACON_TYPE_B.MAG_Y = SAT_HEALTH.MAG_Y;
				BEACON_TYPE_B.MAG_Z = SAT_HEALTH.MAG_Z;

			} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
				myprintf("HK_DATA collection [PACDS] failed.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			}
		} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("HK data command [PACDS] req failed.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		}
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Handshake not Completed with MSN2 [PACDS]\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	}
	MSN2_DATA_RX_FLAG = 0;
	MSN2_DATA_TX_FLAG = 0;
}

void Save_SAT_Log(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4) {

	SAT_LOG_BUFF[0] = HEADER;
	SAT_LOG_BUFF[1] = HEADER;

	SAT_LOG_BUFF[2] = TIM_MIN;		//Satellite uptime(minutes)
	SAT_LOG_BUFF[3] = TIM_HOUR;

	SAT_LOG_BUFF[4] = TIM_DAY;		//rtc time hour
	SAT_LOG_BUFF[5] = TIM_HOUR;		//rtc time minutes
	SAT_LOG_BUFF[6] = TIM_MIN;		//rtc month
	SAT_LOG_BUFF[7] = TIM_SEC;		//rtc day

	SAT_LOG_BUFF[8] = data1;
	SAT_LOG_BUFF[9] = data2;
	SAT_LOG_BUFF[10] = data3;
	SAT_LOG_BUFF[11] = data4;

	SAT_LOG_BUFF[12] = FOOTER;

	Check_SAT_LOG_Address();		//checking the Satellite Log Address

	if (MSN_STATUS != 1) {
		MAIN_FM_MODE();
		Page_Write_4B(&MAIN_FM, SAT_LOG_CURRENT_ADDRESS, SAT_LOG_BUFF,
		SAT_LOG_LEN);
		Page_Write_4B(&SMSN_FM, SAT_LOG_CURRENT_ADDRESS, SAT_LOG_BUFF,
		SAT_LOG_LEN);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Sat Log saved in both the FMs\r\n");
//		HAL_Delay(1);
#endif
#endif
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"Mission ongoing......\r\n Saving the Sat Log in Main FM only\r\n");
//		HAL_Delay(1);
#endif
#endif
		Page_Write_4B(&MAIN_FM, SAT_LOG_CURRENT_ADDRESS, SAT_LOG_BUFF,
		SAT_LOG_LEN);
	}

	SAT_LOG_CURRENT_ADDRESS = SAT_LOG_CURRENT_ADDRESS + SAT_LOG_LEN;//increasing the satellite log address

	Write_Address_Data();	//writing the latest Address data into the FM

}

/*
 * @brief		to Turn the missions on and off
 */
void TURN_ON_PACDS_MSN() {
	HAL_GPIO_WritePin(MSN_2_EN_GPIO_Port, MSN_2_EN_Pin, SET);
	HAL_Delay(10);
}

void TURN_OFF_PACDS_MSN() {
	HAL_GPIO_WritePin(MSN_2_EN_GPIO_Port, MSN_2_EN_Pin, RESET);
}

void TURN_ON_SPDM_MSN() {
	HAL_GPIO_WritePin(MSN_3_EN_GPIO_Port, MSN_3_EN_Pin, SET);//turning on the OCP OF SPDM
//	HAL_GPIO_WritePin(RF_SWITCH_V1_GPIO_Port, RF_SWITCH_V1_Pin, RESET);	//setting both the RF switch low to enable SPDM
//	HAL_GPIO_WritePin(RF_SWITCH_V2_GPIO_Port, RF_SWITCH_V2_Pin, RESET);
}

void TURN_OFF_SPDM_MSN() {
	HAL_GPIO_WritePin(MSN_3_EN_GPIO_Port, MSN_3_EN_Pin, RESET);	//turning off the OCP
//	HAL_GPIO_WritePin(RF_SWITCH_V1_GPIO_Port, RF_SWITCH_V1_Pin, RESET);	//setting RF SWITCH 1 -- Reset
//	HAL_GPIO_WritePin(RF_SWITCH_V2_GPIO_Port, RF_SWITCH_V2_Pin, SET);//setting RF SWITCH 2 -- Set
}

void TURN_ON_CAM_MSN() {
	HAL_GPIO_WritePin(MSN_1_EN_GPIO_Port, MSN_1_EN_Pin, SET);
	HAL_Delay(10);
}

void TURN_OFF_CAM_MSN() {
	HAL_GPIO_WritePin(MSN_1_EN_GPIO_Port, MSN_1_EN_Pin, RESET);
	HAL_Delay(10);
}

/*
 * @brief 	other operation commands
 *
 * @param	command buffer
 */

void OTHER_OPERATIONS(uint8_t *CMD) {

	uint8_t cmd[4];
	for (int i = 0; i < 4; i++) {
		cmd[i] = *CMD++;
	}

	if (cmd[1] == 0x01) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE

#else
		myprintf("Debug operation command received.");

		for (int i = 0; i < 5; i++) {
			myprintf("%02X ", cmd[i]);
		}
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

		uint8_t checksum = cmd[1] + cmd[2];
		if (checksum == cmd[3]) {
			if (cmd[2] == 0xAD) {
#ifdef DEBUG_MODE
				myprintf("MUX MSN side.........\r\n");
#endif
				MSN_FM_MODE();
			} else if (cmd[2] == 0xAF) {
#ifdef DEBUG_MODE
				myprintf("MUX MAIN side.........\r\n");
#endif
				MAIN_FM_MODE();
			} else if (cmd[2] == 0xAA) {
#ifdef DEBUG_MODE
				myprintf(
						"Command to delete all of the Starting Sector.....\r\n");
#endif
				MAIN_FM_MODE();
				Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);
				Sector_Erase_4B(&MAIN_FM,
				ADDRESS_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM, HK_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM, FLAG_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM, SAT_LOG_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM, RSV_TABLE_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM,
				CAM_MSN_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM, CAM_MSN_IMG_INFO_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM,
				SPDM_MSN_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&MAIN_FM,
				PACDS_MSN_DATA_START_ADDRESS, 64);

				Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);
				Sector_Erase_4B(&SMSN_FM,
				ADDRESS_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM, HK_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM, FLAG_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM, SAT_LOG_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM, RSV_TABLE_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM,
				CAM_MSN_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM, CAM_MSN_IMG_INFO_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM,
				SPDM_MSN_DATA_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM,
				PACDS_MSN_DATA_START_ADDRESS, 64);

#ifdef DEBUG_MODE
				myprintf("Starting Sectors of Erased...\r\n");
#endif

			} else if (cmd[2] == 0xAB) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf(
						"External & Internal Flash Erase Command Received.\r\n");
#endif
#endif
				MAIN_FM_MODE();

//				Chip_Erase(&MAIN_FM);
//#ifdef DEBUG_MODE
//#ifdef TRACE_MODE
//				#else
//				myprintf("MAIN FLASH Chip Erase sequence complete.\r\n");
//
//				myprintf(
//						"Reading address locations for verification.\r\n HK Data: ");
//				for (int i = 0; i < 10; i++) {
//					myprintf("%02x ",
//							Read_Byte_4B(&MAIN_FM, HK_DATA_START_ADDRESS + i));
//				}
//				myprintf("\r\n CAM DATA: ");
//				for (int i = 0; i < 10; i++) {
//					myprintf("%02x ", Read_Byte_4B(&MAIN_FM,
//					CAM_MSN_DATA_START_ADDRESS + i));
//				}
//				myprintf("\r\n Address Data Sector");
//				for (int i = 0; i < 10; i++) {
//					myprintf("%02x ", Read_Byte_4B(&MAIN_FM,
//					ADDRESS_DATA_START_ADDRESS + i));
//				}
//				myprintf("\r\n");
//#endif
//#endif
//
//				Chip_Erase(&SMSN_FM);
//
//#ifdef DEBUG_MODE
//#ifdef TRACE_MODE
//				#else
//				myprintf("Shared FLASH Chip Erase sequence complete.\r\n");
//
//				myprintf(
//						"Reading address locations for verification.\r\n HK Data: ");
//				for (int i = 0; i < 10; i++) {
//					myprintf("%02x ",
//							Read_Byte_4B(&SMSN_FM, HK_DATA_START_ADDRESS + i));
//				}
//				myprintf("\r\n CAM DATA: ");
//				for (int i = 0; i < 10; i++) {
//					myprintf("%02x ", Read_Byte_4B(&SMSN_FM,
//					CAM_MSN_DATA_START_ADDRESS + i));
//				}
//				myprintf("\r\n Address Data Sector");
//				for (int i = 0; i < 10; i++) {
//					myprintf("%02x ", Read_Byte_4B(&SMSN_FM,
//					ADDRESS_DATA_START_ADDRESS + i));
//				}
//				myprintf("\r\n");
//#endif
//#endif

				for (int i = 0; i < 2048; i++) {
					Sector_Erase_4B(&MAIN_FM, i * SECTOR_SIZE, 64);
					Sector_Erase_4B(&SMSN_FM, i * SECTOR_SIZE, 64);
				}

			} else if (cmd[2] == 0xBB) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf("MSN 1 Power Up command received.\r\n");
#endif
#endif
				Reset_CMD_PWR_ON_OFF(1);
				Reset_CMD_PWR_ON_OFF(2);
				TURN_ON_CAM_MSN();
				MSN1_FLAG = 1;

			} else if (cmd[2] == 0xCC) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf("MSN 2 Power Up command received.\r\n");
#endif
#endif
				Reset_CMD_PWR_ON_OFF(1);
				TURN_ON_PACDS_MSN();
				MSN2_FLAG = 1;

			} else if (cmd[2] == 0xDD) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf("MSN 3 Power Up Command Received.\r\n");
#endif
#endif
				Reset_CMD_PWR_ON_OFF(1);
				TURN_ON_SPDM_MSN();
				MSN3_FLAG = 1;

			} else if (cmd[2] == 0xBC) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf("MSN 1 Power off command Received.\r\n");
#endif
#endif
				Reset_CMD_PWR_ON_OFF(10);
				Reset_CMD_PWR_ON_OFF(20);
				TURN_OFF_CAM_MSN();
				MSN1_FLAG = 0;

			} else if (cmd[2] == 0xCD) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf("MSN 2 Power off command received.\r\n");
#endif
#endif
				Reset_CMD_PWR_ON_OFF(10);
				TURN_OFF_PACDS_MSN();
				MSN2_FLAG = 0;

			} else if (cmd[2] == 0xDE) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf("MSN 3 power off command received.\r\n");
#endif
#endif
				Reset_CMD_PWR_ON_OFF(10);
				TURN_OFF_SPDM_MSN();
				MSN3_FLAG = 0;

			} else if (cmd[2] == 0xEE) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf(
						"Reset Flags Command Received.\r\n Resetting All Flags.\r\n");
#endif
#endif
				GLOBAL_SYS_RST_FLAG = 0;
				ANT_DEPLOY_STATUS = UNDEPLOYED;
				DCOMMAND_RX_SUCCESS = UL_NOT_RX;
				MSN1_FLAG = OFF;
				MSN2_FLAG = OFF;
				MSN3_FLAG = ON;
				KILL_SWITCH = OFF;
				OPER_MODES = NRML_MODE;
				MSN_STATUS = 0;

				I_ADC1_CONV_CPLT = 0;
				HK_FLAG = 0;
				RESET_Tx.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
				RESET_Tx.UL_STAT = DCOMMAND_RX_SUCCESS;
				RESET_Tx.MSN_FLAG = MSN_STATUS;
				RESET_Tx.RSV_FLAG = 0;
				RESET_Tx.MSN1_STAT = MSN1_FLAG;
				RESET_Tx.MSN2_STAT = MSN2_FLAG;
				RESET_Tx.MSN3_STAT = MSN3_FLAG;

				SAT_HEALTH.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
				SAT_HEALTH.UL_STAT = DCOMMAND_RX_SUCCESS;
				SAT_HEALTH.OPER_MODE = NRML_MODE;
				SAT_HEALTH.MSN_FLAG = MSN_STATUS;
				SAT_HEALTH.RSV_FLAG = 0;
				SAT_HEALTH.MSN3_STAT = MSN3_FLAG;
				SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
				SAT_HEALTH.MSN1_STAT = MSN1_FLAG;
				Store_Flag_Data();
				Reset_CMD_PWR_ON_OFF(RST_FLAG_RST);

			} else if (cmd[2] == 0xFE) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
				#else
				myprintf(
						"Address Data Reset Command Received.\r\n "
								"Resetting all address to staring address, but counter will not be reset.");
#endif
#endif
				Get_Mission_Data_Address();
				HK_DATA_CURRENT_ADDRESS = HK_DATA_START_ADDRESS;
				SAT_LOG_CURRENT_ADDRESS = SAT_LOG_START_ADDRESS;
//					RAD_MSN_DATA_CURRENT_ADDRESS = RAD_MSN_DATA_START_ADDRESS;
				PACDS_MSN_DATA_CURRENT_ADDRESS =
				PACDS_MSN_DATA_START_ADDRESS;
				CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_DATA_START_ADDRESS;
				CAM_MSN_IMG_INFO_CURRENT_ADDRESS =
				CAM_MSN_IMG_INFO_START_ADDRESS;
				SPDM_MSN_DATA_CURRENT_ADDRESS = SPDM_MSN_DATA_START_ADDRESS;
				FLAG_DATA_CURRENT_ADDRESS = FLAG_DATA_START_ADDRESS;
				RSV_DATA_CURRENT_ADDRESS = RSV_TABLE_START_ADDRESS;
				//erasing all the sectors of HK Address
				MAIN_FM_MODE();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf("Erasing all sectors of HK data and SAT_LOG.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
				for (int i = 0; i < 600; i++) {
					Sector_Erase_4B(&MAIN_FM,
							(HK_DATA_START_ADDRESS + i * SECTOR_SIZE), 64);
					Sector_Erase_4B(&SMSN_FM,
							(HK_DATA_START_ADDRESS + i * SECTOR_SIZE), 64);
				}
				Sector_Erase_4B(&MAIN_FM, CAM_MSN_IMG_INFO_START_ADDRESS, 64);
				Sector_Erase_4B(&SMSN_FM, CAM_MSN_IMG_INFO_START_ADDRESS, 64);
				Write_Address_Data();

			} else if (cmd[2] == 0xF0) {
				MAIN_FM_MODE();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
				myprintf("Kill Switch Deactivate Command Received.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
				if (TURN_OFF_KILL_SWITCH()) {
					SAT_HEALTH.KILL_SWITCH = 0x00;
					KILL_SWITCH = 0x00;
					Store_Flag_Data();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
					myprintf("Kill Switch Deactivated.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
				}

			} else if (cmd[2] == 0xFF) {
				MAIN_FM_MODE();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
				myprintf("Kill Switch Activate Command Received.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
				if (TURN_ON_KILL_SWITCH()) {

					Store_Flag_Data();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
					myprintf("Kill Switch Activated.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
				}
			}
		}
	}

}

/*
 * @brief		to turn the kill switch off so that solar panels are connected to the Battery
 *
 * @param		none
 *
 * @retval		1 --  either of the kill switches are turned on
 * 				0 --  both of the kill switches are turned off
 */
uint8_t TURN_OFF_KILL_SWITCH() {

	uint8_t status1 = 0;
	uint8_t status2 = 0;
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Turning off kill switch");
#endif
#endif
	HAL_GPIO_WritePin(KILL_SWITCH1___GPIO_Port, KILL_SWITCH1___Pin, RESET);
	HAL_GPIO_WritePin(KILL_SWITCH1__D5_GPIO_Port, KILL_SWITCH1__D5_Pin, SET);

	HAL_GPIO_WritePin(KILL_SWITCH1_EN_GPIO_Port, KILL_SWITCH1_EN_Pin, SET);	//enabling Kill switch driver
	HAL_Delay(10);

	HAL_GPIO_WritePin(KILL_SWITCH2___GPIO_Port, KILL_SWITCH2___Pin, RESET);
	HAL_GPIO_WritePin(KILL_SWITCH2__B5_GPIO_Port, KILL_SWITCH2__B5_Pin, SET);

	HAL_GPIO_WritePin(KILL_SWITCH2_EN_GPIO_Port, KILL_SWITCH2_EN_Pin, SET);	//enabling kill switch2 driver
	HAL_Delay(10);
//	HAL_GPIO_WritePin(KILL_SWITCH2_EN_GPIO_Port, KILL_SWITCH2_EN_Pin,
//			GPIO_PIN_RESET);  // disabling kill switch2 driver

	status1 = HAL_GPIO_ReadPin(KILL_SWITCH1_STAT_GPIO_Port,
	KILL_SWITCH1_STAT_Pin);
	status2 = HAL_GPIO_ReadPin(KILL_SWITCH2_STAT_GPIO_Port,
	KILL_SWITCH2_STAT_Pin);

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf(
			"kill switch turned off...\r\n Kill switch 1 status: %x \r\n Kill switch 2 status: %x",
			status1, status2);
#endif
#endif
	return (status1 || status2);
}

/*
 *
 *
 * @brief		to turn on the kill switch i.e. disconnect the solar panels from battery
 * @param		none
 *
 * @retval		1 --  either of the kill switches are turned on
 * 				0 --  both of the kill switches are turned off
 */
uint8_t TURN_ON_KILL_SWITCH() {

	static int kill_count;
	static int kill_time;

	uint8_t status1 = 0;
	uint8_t status2 = 0;
	if (kill_count == 0) {
		kill_time = HAL_GetTick();
		kill_count++;
	} else {
		if (HAL_GetTick() - kill_time > 120000 && kill_count < 5) {
			kill_count = 0;
			kill_time = 0;
		}
		kill_count++;
	}

	if (kill_count == 5) {
		kill_count = 0;
		kill_time = 0;

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Turning on kill switch");
#endif
#endif
//		HAL_Delay(10);
		HAL_GPIO_WritePin(KILL_SWITCH1___GPIO_Port, KILL_SWITCH1___Pin, SET);
		HAL_GPIO_WritePin(KILL_SWITCH1__D5_GPIO_Port, KILL_SWITCH1__D5_Pin,
				RESET);
		HAL_GPIO_WritePin(KILL_SWITCH1_EN_GPIO_Port, KILL_SWITCH1_EN_Pin, SET);	//enabling kill switch1 driver

		HAL_GPIO_WritePin(KILL_SWITCH2_EN_GPIO_Port, KILL_SWITCH2_EN_Pin, SET);	//enabling kill switch2 driver
		HAL_Delay(10);
		HAL_GPIO_WritePin(KILL_SWITCH2___GPIO_Port, KILL_SWITCH2___Pin, SET);
		HAL_GPIO_WritePin(KILL_SWITCH2__B5_GPIO_Port, KILL_SWITCH2__B5_Pin,
				RESET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(KILL_SWITCH2_EN_GPIO_Port, KILL_SWITCH2_EN_Pin,
				RESET);  // disabling kill switch2 driver
		HAL_Delay(10);

		status1 = HAL_GPIO_ReadPin(KILL_SWITCH1_STAT_GPIO_Port,
		KILL_SWITCH1_STAT_Pin);
		status2 = HAL_GPIO_ReadPin(KILL_SWITCH2_STAT_GPIO_Port,
		KILL_SWITCH2_STAT_Pin);

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"kill switch turned on...\r\n Kill switch 1 status: %x \r\n Kill switch 2 status: %x",
				status1, status2);
#endif
#endif

		SAT_HEALTH.KILL_SWITCH = 0x01;
		KILL_SWITCH = 0x01;
		return (status1 || status2);
	}
	return 0;
}

