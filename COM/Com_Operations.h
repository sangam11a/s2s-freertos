/*
 * Com_Operations.h
 *
 *  Created on: 21 Feb 2023
 *      Author: janar
 */

#ifndef COM_OPERATIONS_H_
#define COM_OPERATIONS_H_

#include "../CONFIGS/variables.h"
#include "../COMMON_FUNCTIONS/USR_FUNCTIONS.h"

void INIT_COM();
void SWITCH_ANT(uint8_t subsys);
void COM_STATE(uint8_t state);

HAL_StatusTypeDef Verify_Checksum(uint8_t *GS_PKT);
uint8_t COM_PACK_ACK(uint8_t *BUFFER, uint8_t *RX_CMD, uint8_t state);
void COM_EXTRACT_CMD(uint8_t *FULL_BUFFER, uint8_t *CMD_BUFFER);

HAL_StatusTypeDef Parse_GS_Command(uint8_t *GS_CMD, uint8_t *EXE_CMD);
void COM_Transmit_Data(uint8_t *DATA, uint8_t size, uint8_t PID);
void COM_Transmit(uint8_t *DATA, uint8_t size, uint8_t PID);

int16_t MAKE_CRC(uint8_t *data, uint8_t data_number);
void ADDNICS_TX_MODE();
void ADDNICS_RX_MODE();

void S_COM_RX_MODE();
void S_COM_TX_MODE();
#endif /* COM_OPERATIONS_H_ */
