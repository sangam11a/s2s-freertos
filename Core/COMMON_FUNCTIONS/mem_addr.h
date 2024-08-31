/*
 * mem_addr.h
 *
 *  Created on: Sep 22, 2022
 *      Author: Susant
 */

#pragma once

#ifndef MEM_ADDR_H_
#define MEM_ADDR_H_

#include "../COMMON_FUNCTIONS/USR_FUNCTIONS.h"
//#include "../../MISSION/mission.h"
//#include "USR_FUNCTIONS.h"
//#include "mission.h"
//#include "MT25QL.h"

void Check_Address_Data_Address();
void Write_Address_Data();
void Get_Address_Current_Address();
void Print_Address_Location();
void Get_Mission_Data_Address();

void Check_Flag_Data();
void Store_Flag_Data();
void Print_Flag_Data();

void Check_HK_Data_Address();
void Check_CAM_Address();
void Check_SPDM_Address();
void Check_PACDS_Address();
void Check_SAT_LOG_Address();

uint8_t Delete_Sector(SPI_HandleTypeDef *SPI, uint32_t cmd, uint32_t number);
void FM_Data_To_PC(SPI_HandleTypeDef *hspi, uint32_t *ADDRESS,
		uint32_t PCKT_NUM, uint8_t PORT);

uint8_t Transfer_Sector_SFM_MFM(uint32_t ADDRESS, uint8_t no_of_sectors);

void Transfer_Bytes_SFM_MFM(uint32_t ADDRESS, uint32_t size);

void TRANSFER_DATA_COM(SPI_HandleTypeDef *SPI, uint32_t ADDRESS, uint8_t PKT);

#ifdef ACTIVATE_INT_FLASH

void ERASE_INTERNAL_FLASH(uint32_t SECTOR_NUM);

HAL_StatusTypeDef WRITE_TO_INTERNAL_FLASH(uint32_t sector,
		uint32_t write_address, uint8_t *DATA, uint8_t data_len);

void READ_FRM_INTERNAL_FLASH(uint32_t address, uint8_t *read_buff, uint32_t len);

#endif

#endif /* MEM_ADDR_H_ */
