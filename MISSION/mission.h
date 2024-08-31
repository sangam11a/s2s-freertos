/*
 * mission.h
 *
 *  Created on: Oct 14, 2022
 *      Author: Sushant
 */
#pragma once

#ifndef MISSION_H_
#define MISSION_H_

#include "../../CONFIGS/variables.h"
#include "../../EXT_ADC/ADS7953SDBT.h"
#include "../COMMON_FUNCTIONS/mem_addr.h"
#include "../COMMON_FUNCTIONS/USR_FUNCTIONS.h"
//#include "variables.h"
//#include "ADS7953SDBT.h"
//#include "mem_addr.h"
//#include "USR_FUNCTIONS.h"


void Execute_CMD(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3, uint8_t ADDR1, uint8_t ADDR2, uint8_t ADDR3,
		uint8_t ADDR4, uint8_t PCKT_NUM1, uint8_t PCKT_NUM2);
void EXECUTE_CMD_OBC(uint8_t MAIN_CMD1, uint8_t MAIN_CMD2, uint8_t MAIN_CMD3,
		uint8_t ADDR1, uint8_t ADDR2, uint8_t ADDR3, uint8_t ADDR4,
		uint8_t PCKT_NUM1, uint8_t PCKT_NUM2);
void EXECUTE_CMD_CAM(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3);
void EXECUTE_CMD_SPDM(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3);
void EXECUTE_CMD_PACDS(uint8_t MCU_ID, uint8_t MAIN_CMD1, uint8_t MAIN_CMD2,
		uint8_t MAIN_CMD3);

void Reset_CMD_PWR_ON_OFF(uint8_t MSN);

uint8_t MSN1_HANDSHAKE(uint8_t MAIN_CMD1, uint8_t MAIN_CMD2, uint8_t MAIN_CMD3);
uint8_t MSN2_HANDSHAKE();
uint8_t MSN3_HANDSHAKE();

void Check_Rx_Data_MSN();

uint8_t Antenna_Deployment();
void OTHER_OPERATIONS(uint8_t *CMD);
void GET_EXT_ADC_DATA();
void GET_PACDS_GYRO_DATA();

void Save_SAT_Log(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);

void CMD_FROM_PC_FM_OPERATION(uint8_t CMD1, uint8_t CMD2, uint8_t CMD3,
		uint8_t CMD4, uint8_t checksum);

void TURN_ON_CAM_MSN();
void TURN_OFF_CAM_MSN();
void TURN_ON_PACDS_MSN();
void TURN_OFF_PACDS_MSN();
void TURN_ON_SPDM_MSN();
void TURN_OFF_SPDM_MSN();


uint8_t TURN_OFF_KILL_SWITCH();
uint8_t TURN_ON_KILL_SWITCH();

#endif /* MISSION_H_ */
