/*
 * Com_Operations.c
 *
 *  Created on: 21 Feb 2023
 *      Author: janar
 */

#include "Com_Operations.h"
/*******************************COM Function Sets**************************************/

/*
 * @brief intializing of the COM transciever board
 *
 * @param	none
 * @retval	none
 *
 */
void INIT_COM() {
#ifdef S_COM_PLAN_F_MODE
	if (ANT_DEPLOY_STATUS == DEPLOYED || TIM_MIN > 30) {
		//turn on UNREG_1 line
		Reset_CMD_PWR_ON_OFF(3);
	}
	//turn on 3V3_2 Line
	Reset_CMD_PWR_ON_OFF(1);
	//RESET SPDM{which works as com rx}
	TURN_OFF_SPDM_MSN();
	HAL_Delay(5);
	TURN_ON_SPDM_MSN();
	//set COM Transmitter control pin to active

	COM_STATE(0);
	//switch ant access to SPDM, com Rx
	SWITCH_ANT(1);
	HAL_UART_Receive_IT(&MSN3_STREAM, &COM_MSN3_TEMP_Rx, 1);
#endif // S_COM_PLAN_F_MODe

#ifdef S_COM_MODE
	//turn on UNREG_1 line
	Reset_CMD_PWR_ON_OFF(3);
	COM_STATE(0);
	HAL_Delay(10);
	COM_STATE(1);

#endif //S_COM_MODE

#ifdef ADDNICS_COM_MODE
	//turn on UNREG_1 line
	Reset_CMD_PWR_ON_OFF(3);
	ADDNICS_RX_MODE(); //setting the addnics operation pin config to Rx Mode by default
	SWITCH_ANT(0);
	COM_STATE(1);		// COM Active
	HAL_UART_Receive_IT(&MSN3_STREAM, &COM_MSN3_TEMP_Rx, 1);
#endif // ADDNICS_COM_MODE
	HAL_UART_Receive_IT(&COM_TCV_STREAM, &COM_TEMP_Rx, 1);
}

/*
 * @brief used to switch the antenna access between COM and SPDM
 *
 * @param	subsys indicates which system to give antenna access to
 * 			0: [default] Ant access to COM
 * 			1: Ant access to SPDM
 * @retval	none
 *
 */
void SWITCH_ANT(uint8_t subsys) {
	if (subsys) {
		// ANT access to MSN3
		HAL_GPIO_WritePin(RF_SWITCH_V1_GPIO_Port, RF_SWITCH_V1_Pin, RESET);
		HAL_GPIO_WritePin(RF_SWITCH_V2_GPIO_Port, RF_SWITCH_V2_Pin, RESET);
	} else {
		//ANT access to COM
		HAL_GPIO_WritePin(RF_SWITCH_V1_GPIO_Port, RF_SWITCH_V1_Pin, RESET);
		HAL_GPIO_WritePin(RF_SWITCH_V2_GPIO_Port, RF_SWITCH_V2_Pin, SET);
	}
}

/*
 * @brief controls the state of COM system [RESET | ACTIVE]
 *
 * @param	state	1: ACTIVE | 0: RESET
 * @retval	none
 *
 */
void COM_STATE(uint8_t state) {
	if (state) {
		//COM ACTIVE
		HAL_GPIO_WritePin(COM_SEL_GPIO_Port, COM_SEL_Pin, SET);
		HAL_GPIO_WritePin(OBC_COM_PWR_ONOFF_GPIO_Port, OBC_COM_PWR_ONOFF_Pin,
				SET);
	} else {
		HAL_GPIO_WritePin(OBC_COM_PWR_ONOFF_GPIO_Port, OBC_COM_PWR_ONOFF_Pin,
				RESET);
		HAL_GPIO_WritePin(COM_SEL_GPIO_Port, COM_SEL_Pin, RESET);
	}
}

/*
 * @brief Checks the integrity of the command
 *
 * @param	uint8_t * pointer to the buffer containing the data buffer with received GS data packet
 * @retval	HAL_StatusTypedef  status if checksum process
 * 							HAL_OK: if checksum is correct
 * 							HAL_ERROR: if checksum is incorrect
 *
 */
HAL_StatusTypeDef Verify_Checksum(uint8_t *GS_PKT) {

	uint16_t TEMP_CHECKSUM = 0;
	uint8_t *data = GS_PKT;
	uint16_t GS_CHECKSUM = data[COM_RX_DATA_BUFFER_LEN - 1] << 8
			| data[COM_RX_DATA_BUFFER_LEN - 2];
	TEMP_CHECKSUM = MAKE_CRC(data, COM_RX_DATA_BUFFER_LEN - 2);

	if (GS_CHECKSUM == TEMP_CHECKSUM) {
		return HAL_OK;
	} else {
		return HAL_ERROR;
	}
}

/*
 * @brief packs data reception acknowledgement to tx buffer for COM
 *
 * @param	uint8_t * pointer to location to store ACk data
 * 			RX_CMD	command for satellite from GS
 * 			state 	flag to determine the type of ack
 * 					0: Successful data reception
 * 					1: Checksum failure
 * @retval uint8_t 	length of data packed
 *
 */
uint8_t COM_PACK_ACK(uint8_t *BUFFER, uint8_t *RX_CMD, uint8_t state) {
	uint16_t checksum = 0;
	memset(BUFFER, 0xEE, COM_TX_DATA_BUFFER_LEN);

#ifdef S_COM_PLAN_F_MODE
	BUFFER[0] = AX25_HEADER_FOOTER;		// BYTE 0
	for (int i = 0; i < 6; i++) {		// 1-6 BYTE
		BUFFER[i + 1] = SAT_NAME_CALL_SIGN[i];
	}
	BUFFER[7] = 0xAC;		//PID represents ACK packet
	BUFFER[15] = AX25_CONTROL;		//BYTE 15
	switch (state) {
	case 0:			// passed checksum test
		BUFFER[16] = COM_CHECKSUM_PASS;	//BYTE 16 [determies if ack pass or fail
		break;
	default:		//failed checksum test
		BUFFER[16] = COM_CHECKSUM_FAIL;	//BYTE 16 [determies if ack pass or fail
		break;
	}
	for (int i = 0; i < CMD_LEN; i++) { //17-29 BYTE
		BUFFER[i + 17] = RX_CMD[i];
	}

	checksum = MAKE_CRC(BUFFER, 30);
	BUFFER[30] = checksum & 0xFF; //30 Byte
	BUFFER[31] = checksum >> 8 & 0xFF;  //31 Byte
	return 32;
#endif //S_COM_PLAN_F_MODE
#ifdef S_COM_MODE
	BUFFER[0] = AX25_HEADER_FOOTER;		// BYTE 0
	for (int i = 0; i < 14; i++) {		// 1-14 BYTE
		BUFFER[i + 1] = SAT_NAME_CALL_SIGN[i];
	}
	BUFFER[15] = AX25_CONTROL;		//BYTE 15

	switch (state) {
	case 0:			// passed checksum test
		BUFFER[16] = COM_CHECKSUM_PASS;	//BYTE 16 [determies if ack pass or fail
		break;
	default:		//failed checksum test
		BUFFER[16] = COM_CHECKSUM_FAIL;	//BYTE 16 [determies if ack pass or fail
		break;
	}
	for (int i = 0; i < CMD_LEN; i++) { //17-29 BYTE
		BUFFER[i + 17] = RX_CMD[i];
	}

		checksum = MAKE_CRC(BUFFER, 30);
	BUFFER[30] = checksum & 0xFF; //30 Byte
	BUFFER[31] = checksum >> 8 & 0xFF;  //31 Byte
	return 32;
#endif

#ifdef ADDNICS_COM_MODE

//	switch (state) {
//	BUFFER[0] = AX25_HEADER_FOOTER;
//case 0:
//	BUFFER[1] = COM_CHECKSUM_PASS;
//	break;
//case 1:
//	BUFFER[1] = COM_CHECKSUM_FAIL;
//	break;
//	}
//	for (int i = 0; i < CMD_LEN; i++) {
//		BUFFER[i + 2] = RX_CMD[i];
//	}
//	checksum = MAKE_CRC(BUFFER, 15);
//	BUFFER[15] = checksum & 0xFF;
//	BUFFER[16] = checksum >> 8 & 0xFF;
//	HAL_Delay(50);
//	return 17;

	BUFFER[0] = AX25_HEADER_FOOTER;		// BYTE 0
	for (int i = 0; i < 6; i++) {		// 1-6 BYTE
		BUFFER[i + 1] = SAT_NAME_CALL_SIGN[i];
	}
	BUFFER[7] = 0xAC;		//PID represents ACK packet
	BUFFER[15] = AX25_CONTROL;		//BYTE 15
	switch (state) {
	case 0:			// passed checksum test
		BUFFER[16] = COM_CHECKSUM_PASS;	//BYTE 16 [determies if ack pass or fail
		break;
	default:		//failed checksum test
		BUFFER[16] = COM_CHECKSUM_FAIL;	//BYTE 16 [determies if ack pass or fail
		break;
	}
	for (int i = 0; i < CMD_LEN; i++) { //17-29 BYTE
		BUFFER[i + 17] = RX_CMD[i];
	}

	checksum = MAKE_CRC(BUFFER, 30);
	BUFFER[30] = checksum & 0xFF; //30 Byte
	BUFFER[31] = checksum >> 8 & 0xFF;  //31 Byte
	return 32;
#endif

//	return COM_TX_DATA_BUFFER_LEN;
}

/*
 * @brief extract the command for satellte from GS
 *
 * @param	FULL_BUFFER		pointer to the location containing the full packet
 * 			CMD_BUFFER		pointer to the location to store extracted command
 * @retval	none
 *
 */
void COM_EXTRACT_CMD(uint8_t *FULL_BUFFER, uint8_t *CMD_BUFFER) {
	for (int i = 1; i < COM_RX_DATA_BUFFER_LEN - 2; i++) {
		if (i > CMD_LEN) { // for addnics mode this is necessary to avoid affecting the memory space behind the limit of *CMD_BUFFER pointers variable
			break;
		}
		CMD_BUFFER[i - 1] = FULL_BUFFER[i];
	}
}
/*
 * @brief Parse GS Command received from COM to extract command for
 *
 * @param	uint8_t * pointer to the varaible containing the received data
 * 			uint8_t * popinter to the variable to store the parsed command
 * @retval	HAL_StatusTypeDef		status of data reception
 *
 */

HAL_StatusTypeDef Parse_GS_Command(uint8_t *GS_CMD, uint8_t *EXE_CMD) {
	HAL_StatusTypeDef status;
	uint32_t tick_start;
	uint8_t ACK_DATA_LEN;
//	uint8_t len;
	status = Verify_Checksum(GS_CMD);
	COM_EXTRACT_CMD(GS_CMD, EXE_CMD);
	memset(GS_CMD, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(COM_TX_DATA_BUFFER, '\0', COM_TX_DATA_BUFFER_LEN);
	if (status == HAL_OK) {		// checksum verified successfully
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf(
				"Data received from COM.\r\n Checksum Success..\r\n ACK Transmitting.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf(
				"Data received from COM.\r\n Checksum test fail..\r\n ACK  Not Transmitting.\r\n");
#else
					#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	}
	ACK_DATA_LEN = COM_PACK_ACK(COM_TX_DATA_BUFFER, EXE_CMD, status);

#ifdef ADDNICS_COM_MODE
	ADDNICS_TX_MODE();
	HAL_Delay(1000);
	HAL_UART_Transmit(&COM_TCV_STREAM, COM_TX_DATA_BUFFER, ACK_DATA_LEN, 1000);	// sending successful ack
	HAL_Delay(1000);
	ADDNICS_RX_MODE();
#endif //ADDNICS_COM_MODE

#ifdef S_COM_PLAN_F_MODE
	COM_DATA_TX_FLAG = 0;
	COM_DATA_RX_FLAG = 0;
	S_COM_TX_MODE();
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&COM_TCV_STREAM, COM_TX_DATA_BUFFER,
	COM_TX_DATA_BUFFER_LEN);

	uint32_t current_tick = HAL_GetTick();
	while (COM_DATA_TX_FLAG == 0) {
		if (HAL_GetTick() - current_tick > 3000) {
			break;
		}
	}
	COM_DATA_TX_FLAG = 0;

	tick_start = HAL_GetTick();
	while (COM_DATA_RX_FLAG != 1 && ((HAL_GetTick() - tick_start) < 5000)) {
	}
	COM_DATA_TX_FLAG = 0;
	if ((COM_RX_DATA[0] << 8 | COM_RX_DATA[1]) == COM_ACK) {

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("COM ACK received.\r\n");
#else
						#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	} else {

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("COM ACK not received.\r\n");
#else
						#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	}
	HAL_Delay(1000);
	S_COM_RX_MODE();
	COM_DATA_RX_FLAG = 0;
	COM_TEMP_Rx = 0;
	COM_RX_COUNT = 0;
	memset(COM_RX_DATA, '\0', COM_RX_DATA_BUFFER_LEN);
#endif //S_COM_PLAN_F_MODE

#ifdef S_COM_MODE
		tick_start = HAL_GetTick();
		while (COM_DATA_RX_FLAG != 1 && ((HAL_GetTick() - tick_start) < 5000)) {
		}
		COM_DATA_TX_FLAG = 0;
		if ((COM_RX_DATA[0] << 8 | COM_RX_DATA[1]) == COM_ACK) {

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("COM ACK received.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

		} else {

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("COM ACK not received.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

		}
#endif    // S_COM_MODE

	return status;
}

/*
 * @brief PACKS the data to be sent to GS into 100 byte packet for Transmission
 *
 * @param	DATA	pointer to data location
 * @retval	none
 *
 */
void COM_Transmit_Data(uint8_t *DATA, uint8_t size, uint8_t PID) {
	uint8_t tx_buffer[COM_TX_DATA_BUFFER_LEN];

	/*
	 #ifdef S_COM_PLAN_F_MODE

	 S_COM_TX_MODE();
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf(
	 "\r\n\r\nAntenna Switched to Transmitter. COM Shifted Transmitting Mode.\r\n");

	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 memset(tx_buffer, 0x00, COM_TX_DATA_BUFFER_LEN);
	 uint16_t checksum = 0;
	 tx_buffer[0] = AX25_HEADER_FOOTER;		//BYTE 0
	 tx_buffer[1] = SAT_NAME_CALL_SIGN[0];
	 tx_buffer[2] = SAT_NAME_CALL_SIGN[1];
	 tx_buffer[3] = SAT_NAME_CALL_SIGN[2];
	 tx_buffer[4] = SAT_NAME_CALL_SIGN[3];
	 tx_buffer[5] = SAT_NAME_CALL_SIGN[4];
	 tx_buffer[6] = SAT_NAME_CALL_SIGN[5];
	 tx_buffer[7] = PID;					//Byte 1	Packet ID
	 if (size < PACKET_DATA_LEN) {
	 for (int i = 0; i < size; i++) {
	 tx_buffer[i + 8] = DATA[i];
	 }
	 //		for (int i = (size + 8); i < 98; i++) {
	 //			tx_buffer[i] = 0xff;
	 //		}
	 checksum = MAKE_CRC(tx_buffer, size + 8);
	 tx_buffer[size + 8] = checksum & 0xFF;
	 tx_buffer[size + 9] = checksum >> 8 & 0xFF;

	 } else {
	 for (int i = 0; i < PACKET_DATA_LEN; i++) {		//Byte 17-96
	 tx_buffer[i + 8] = DATA[i];
	 }
	 checksum = MAKE_CRC(tx_buffer, 98);
	 tx_buffer[98] = checksum & 0xFF;
	 tx_buffer[99] = checksum >> 8 & 0xFF;
	 }

	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE

	 #else
	 myprintf("Transmitting Data to GS.\r\nData to Transmit: ");
	 for (int i = 0; i < COM_TX_DATA_BUFFER_LEN; i++) {
	 myprintf("%02x ", tx_buffer[i]);
	 }
	 myprintf("\r\n");
	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 COM_DATA_TX_FLAG = 0;
	 COM_DATA_RX_FLAG = 0;

	 uint32_t start_tick = HAL_GetTick();
	 HAL_UART_Transmit_IT(&COM_TCV_STREAM, tx_buffer,
	 COM_TX_DATA_BUFFER_LEN);

	 //	while (HAL_UART_Transmit(&COM_TCV_STREAM, tx_buffer,
	 //	COM_TX_DATA_BUFFER_LEN, 1000) != HAL_OK) {
	 //		if (HAL_GetTick() - start_tick > 5000) {
	 //#ifdef DEBUG_MODE
	 //#ifdef TRACE_MODE
	 //		#else
	 //			myprintf("COM TX Timeout");
	 //#endif //ifndef TRACE_MODE
	 //#endif //ifdef DEBUG_MODE
	 //			//			INIT_COM();
	 //			COM_DATA_TX_FLAG = 0;
	 //			break;
	 //		}
	 //	}
	 //	if (HAL_GetTick() - start_tick < 5000) {
	 //		COM_DATA_TX_FLAG = 1;
	 //	}

	 while (COM_DATA_TX_FLAG == 0) {
	 if (HAL_GetTick() - start_tick > 5000) {
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf("COM TX Timeout");
	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 //			INIT_COM();
	 COM_DATA_TX_FLAG = 0;
	 break;
	 }
	 }
	 if (COM_DATA_TX_FLAG) {
	 HAL_Delay(100);
	 start_tick = HAL_GetTick();
	 while (COM_DATA_RX_FLAG != 1) {
	 if (HAL_GetTick() - start_tick > 5000) {
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf("COM TX ACK Timeout");
	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 //			INIT_COM();
	 break;
	 }
	 }
	 if ((COM_RX_DATA[0] << 8 | COM_RX_DATA[1]) == COM_ACK) {
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf("COM ACK received.\r\n");
	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 } else {
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf("COM ACK Failed.\r\n");
	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 }
	 } else {
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf("COM TX Failed.\r\n");
	 #endif //ifndef TRACE_MODE
	 #endif //ifdef DEBUG_MODE
	 }

	 memset(COM_RX_DATA_BUFFER, '\0', COM_RX_DATA_BUFFER_LEN);
	 memset(COM_RX_DATA, '\0', COM_RX_DATA_BUFFER_LEN);
	 HAL_Delay(500);
	 #ifdef DEBUG_MODE
	 #ifdef TRACE_MODE
	 #else
	 myprintf(
	 "\r\n\r\nANTENNA Switched to COM Receiver [SPDM].\r\nCOM Switched to Receiver Mode.\r\n");
	 #endif		//TRACE_MODE
	 #endif		//DEBUG_MODE
	 S_COM_RX_MODE();
	 HAL_Delay(100);
	 COM_DATA_TX_FLAG = 0;
	 COM_DATA_RX_FLAG = 0;
	 #endif // S_COM_PLAN_F_MODE
	 */

#ifdef S_COM_MODE
	memset(tx_buffer, '\0', COM_TX_DATA_BUFFER_LEN);
	uint16_t checksum = 0;
	tx_buffer[0] = AX25_HEADER_FOOTER;		//BYTE 0
	for (int i = 0; i <6; i++) {		//BYTE 1-14
		tx_buffer[i + 1] = SAT_NAME_CALL_SIGN[i];
	}
	tx_buffer[15] = AX25_CONTROL;		//Byte 15
	tx_buffer[16] = PID;			//Byte 16	Packet ID
	if (size < PACKET_DATA_LEN) {
		for (int i = 0; i < size; i++) {
			tx_buffer[i + 17] = DATA[i];
		}
		for (int i = 0; i < (17 + size); i++) {
			checksum += tx_buffer[i];
		}
		tx_buffer[17 + size] = checksum >> 8 & 0xFF;
		tx_buffer[18 + size] = checksum & 0xFF;
		tx_buffer[19 + size] = AX25_HEADER_FOOTER;
	} else {

		for (int i = 0; i < PACKET_DATA_LEN; i++) {		//Byte 17-96
			tx_buffer[i + 17] = DATA[i];
		}
		checksum = MAKE_CRC(tx_buffer, 98)
		tx_buffer[98] = checksum & 0xFF;
		tx_buffer[99] = checksum>>8 & 0xFF;
	}

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("Transmitting Data to GS.\r\n");
//	HAL_Delay(1);
#else
	#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	COM_DATA_TX_FLAG = 0;
	COM_DATA_RX_FLAG = 0;
	HAL_UART_Transmit_IT(&COM_TCV_STREAM, tx_buffer,
	tx_buffer_LEN);
	RUN_ADC();
	uint32_t start_tick = HAL_GetTick();

	while (COM_DATA_RX_FLAG != 1) {
		if (HAL_GetTick() - start_tick > 5000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("COM ACK Timeout");
			HAL_Delay(1);
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			INIT_COM();
			break;
		}
	}
	if ((COM_RX_DATA[0] << 8 | COM_RX_DATA[1]) == COM_ACK) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("COM ACK received.\r\n");
#else
				#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("COM ACK Failed.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	}
	Write_Address_Data();
	memset(COM_RX_DATA_BUFFER, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(COM_RX_DATA, '\0', COM_RX_DATA_BUFFER_LEN);
	HAL_Delay(500);

#endif //S_COM_MODE

#ifdef ADDNICS_COM_MODE

	memset(tx_buffer, '\0', COM_TX_DATA_BUFFER_LEN);
	uint16_t checksum = 0;
	tx_buffer[0] = AX25_HEADER_FOOTER;		//BYTE 0
	for (int i = 0; i < 5; i++) {		//BYTE 1-14
		tx_buffer[i + 1] = SAT_NAME_CALL_SIGN[i];
	}
	tx_buffer[7] = PID; 		// char 0

//	for (int i = 0; i < 6; i++) {
//		tx_buffer[i + 8] = SAT_NAME_CALL_SIGN[i];
//	}
//	tx_buffer[14] = 0x30; // char 0
//
//	tx_buffer[15] = AX25_CONTROL;		//Byte 15
//	tx_buffer[16] = PID >> 8 & 0xFF;			//Byte 16	Packet ID
//	tx_buffer[17] = PID & 0xFF;

	if (size < PACKET_DATA_LEN) {
		for (int i = 0; i < size; i++) {
			tx_buffer[i + 8] = DATA[i];
		}

		checksum = MAKE_CRC(tx_buffer, 98);
		tx_buffer[98] = checksum & 0xFF;
		tx_buffer[99] = checksum >> 8 & 0xFF;

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Shifting to Transmission Mode.\r\n");
		myprintf("Transmitting Data to GS.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

		COM_DATA_TX_FLAG = 0;
		COM_DATA_RX_FLAG = 0;
//		ADDNICS_TX_MODE();
//		HAL_Delay(1000);
		UART_STATE = HAL_UART_Transmit(&COM_TCV_STREAM, tx_buffer,
		COM_TX_DATA_BUFFER_LEN, 1000);
		if (BEACON_FLAG || PID % 50 == 0) {
			RUN_ADC();
		}

	} else if (size == PACKET_DATA_LEN) {
		for (int i = 0; i < PACKET_DATA_LEN; i++) {		//Byte 18-97
			tx_buffer[i + 8] = DATA[i];
		}
		checksum = MAKE_CRC(tx_buffer, 98);
		tx_buffer[98] = checksum & 0xFF;
		tx_buffer[99] = checksum >> 8 & 0xFF;
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Transmitting Data to GS.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		COM_DATA_TX_FLAG = 0;
		COM_DATA_RX_FLAG = 0;
//		ADDNICS_TX_MODE();
		UART_STATE = HAL_UART_Transmit(&COM_TCV_STREAM, tx_buffer,
		COM_TX_DATA_BUFFER_LEN, 1000);

		if (BEACON_FLAG || PID % 50 == 0) {
			RUN_ADC();
		}

	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Error.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		return;
	}

//	HAL_Delay(1000);
//	ADDNICS_RX_MODE();
	Write_Address_Data();
	if (UART_STATE == HAL_OK) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Data Transmission complete.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Data Transmission Error.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	}
	memset(COM_RX_DATA_BUFFER, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(COM_RX_DATA, '\0', COM_RX_DATA_BUFFER_LEN);
#endif //ADDNICS_COM_MODE
}

/*
 * @brief PACKS the data to be sent to GS into 100 byte packet for Transmission
 *
 * @param	DATA	pointer to data location
 * 			size	size of data to be transmitted
 * 			PID		packet id
 * @retval	none
 *
 */

void COM_Transmit(uint8_t *DATA, uint8_t size, uint8_t PID) {
#ifdef S_COM_PLAN_F_MODE
	uint8_t DATA_BUFFER[COM_TX_DATA_BUFFER_LEN];
	memset(DATA_BUFFER, '\0', COM_TX_DATA_BUFFER_LEN);
	uint16_t checksum = 0;
	DATA_BUFFER[0] = AX25_HEADER_FOOTER;
	for (int i = 0; i < 5; i++) {
		DATA_BUFFER[i + 1] = SAT_NAME_CALL_SIGN[i];
	}
	DATA_BUFFER[7] = PID;
	if (size < PACKET_DATA_LEN) {
		for (int i = 0; i < size; i++) {
			DATA_BUFFER[i + 8] = DATA[i];
		}
		checksum = MAKE_CRC(DATA_BUFFER, (size + 8));
		DATA_BUFFER[size + 8] = checksum & 0xFF;
		DATA_BUFFER[size + 9] = checksum >> 8 & 0xFF;

	} else if (size == PACKET_DATA_LEN) {
		for (int i = 0; i < PACKET_DATA_LEN; i++) {
			DATA_BUFFER[i + 8] = DATA[i];
		}
		checksum = MAKE_CRC(DATA_BUFFER, COM_TX_DATA_BUFFER_LEN - 2);
		DATA_BUFFER[COM_TX_DATA_BUFFER_LEN - 2] = checksum & 0xFF;
		DATA_BUFFER[COM_TX_DATA_BUFFER_LEN - 1] = checksum >> 8 & 0xFF;
	}
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
//	myprintf("Data packet Ready. Transmitting to GS\r\n Transmitting Data: ");

	myprintf("Data Ready. Tx2gs\r\n");

//	for (int i = 0; i < COM_TX_DATA_BUFFER_LEN; i++) {
//		myprintf("%02x ", DATA_BUFFER[i]);
//	}
//	myprintf("\r\n");
#endif
#endif

	COM_DATA_TX_FLAG = 0;
	COM_DATA_RX_FLAG = 0;
	uint32_t start_tick = HAL_GetTick();
	HAL_UART_Transmit_IT(&COM_TCV_STREAM, DATA_BUFFER,
	COM_TX_DATA_BUFFER_LEN);

	while (!COM_DATA_TX_FLAG) {
		if (HAL_GetTick() - start_tick > 5000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
			myprintf("COM_TX_TIMEOUT\r\n");
#endif //TRACE_MODE
#endif //DEBUG_MODE
			COM_DATA_TX_FLAG = 0;
			break;
		}
	}
	if (COM_DATA_TX_FLAG) {
		if (BEACON_FLAG || (PID % 50 == 0)) {
			RUN_ADC(); // runs adcs, collects, sorts and stores data to flash memory
		}
		HAL_Delay(100);
		start_tick = HAL_GetTick();
		while (COM_DATA_RX_FLAG != 1) {
			if (HAL_GetTick() - start_tick > 2000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
				myprintf("COM TX ACK Timeout");
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
				break;
			}
		}

		if ((COM_RX_DATA[0] << 8 | COM_RX_DATA[1]) == COM_ACK) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
			myprintf("COM ACK.\r\n");
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			COM_RX_COUNT = 0;
			COM_TEMP_Rx = 0;
		} else {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
			myprintf("COM ACK Failed.\r\n");
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

		}

	} else {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
	#else
		myprintf("COM TX Failed.\r\n");
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	}
	memset(COM_RX_DATA, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(DATA_BUFFER, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(COM_TX_DATA_BUFFER, '\0', COM_TX_DATA_BUFFER_LEN);
	HAL_Delay(100);
	COM_DATA_TX_FLAG = 0;
	COM_DATA_RX_FLAG = 0;
#endif //S_COM_PLAN_F_MODE
}

/*
 * @brief	function which creates the CRC checksum for data being transmitted from GS
 *
 * @param	data			pointer to address storing data to be transmitted
 * 			data_number		length of data for which CRC is to be generated
 *
 * @retval	int16_t			calculated CRC value
 */

//TODO: figure out how this works!!!
int16_t MAKE_CRC(uint8_t *data, uint8_t data_number) {
	uint32_t crcReg = 0xFFFF;
	uint32_t calc = 0x8408;		// CRC calculator value
	uint8_t w;
	uint32_t i;
	uint32_t k;
	uint8_t cal_data[DATA_BUFFER_LEN];

	for (k = 0; k < data_number; k++) {
		cal_data[k] = data[k];
		for (i = 0; i < 8; i++) {
			w = (crcReg ^ cal_data[k]) & 0x0001;
			crcReg = crcReg >> 1;

			if (w == 1) {
				crcReg = crcReg ^ calc;
			}

			cal_data[k] = cal_data[k] >> 1;
		}
	}

	crcReg = crcReg ^ 0xFFFF;
	return crcReg;
}

/*
 * @brief	changes the mode of ADDNICS to Transmission Mode
 *
 * @param	none
 *
 * @retval 	none
 */

void ADDNICS_TX_MODE() {
//	Reset_CMD_PWR_ON_OFF(3);
	SWITCH_ANT(0);
	HAL_GPIO_WritePin(OBC_COM_PWR_ONOFF_GPIO_Port, OBC_COM_PWR_ONOFF_Pin,
			GPIO_PIN_SET);
	HAL_GPIO_WritePin(OBC_COMCWPIC_CONFIG_GPIO_Port, OBC_COMCWPIC_CONFIG_Pin,
			GPIO_PIN_RESET);
	HAL_GPIO_WritePin(OBC_COM_CWPIC_CWKEY_GPIO_Port, OBC_COM_CWPIC_CWKEY_Pin,
			GPIO_PIN_RESET);
	HAL_Delay(1000);
}

/*
 * @brief	changes the mode of ADDNICS to Receiving mode
 *
 * @param	none
 * @retval	none
 *
 */

void ADDNICS_RX_MODE() {
//	Reset_CMD_PWR_ON_OFF(1);
	SWITCH_ANT(1);
	HAL_GPIO_WritePin(OBC_COM_PWR_ONOFF_GPIO_Port, OBC_COM_PWR_ONOFF_Pin,
			GPIO_PIN_RESET);
	HAL_GPIO_WritePin(OBC_COMCWPIC_CONFIG_GPIO_Port, OBC_COMCWPIC_CONFIG_Pin,
			GPIO_PIN_RESET);
	HAL_GPIO_WritePin(OBC_COM_CWPIC_CWKEY_GPIO_Port, OBC_COM_CWPIC_CWKEY_Pin,
			GPIO_PIN_RESET);
	HAL_Delay(1000);
}

/*
 * @brief	sets the communication channel in RX mode for S_COM_PLAN_F_MODE
 *
 * @param	none
 * @retval	none
 */

void S_COM_RX_MODE() {
	SWITCH_ANT(1);
	TURN_ON_SPDM_MSN();		// turns on SPDM OCP
	COM_STATE(0);
//	Reset_CMD_PWR_ON_OFF(3);	// turn on UNREG_1 Line
}

/*
 * @brief	Sets the communication channel in TX Mode for S_COM_PLAN_F_MODE
 *
 * @param	none
 * @retval	none
 */
void S_COM_TX_MODE() {
	COM_STATE(1);
	SWITCH_ANT(0);
	TURN_OFF_SPDM_MSN();
	Reset_CMD_PWR_ON_OFF(3);	//  turn on UNREG_1 Line
}
