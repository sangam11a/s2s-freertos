/*
 * USR_FUNCTIONS.h
 *
 *  Created on: Aug 24, 2022
 *      Author: jaddu
 */
#pragma once

#ifndef USR_FUNCTIONS_H_
#define USR_FUNCTIONS_H_

#include "../../CONFIGS/variables.h"
#ifdef CDC_USB_DEBUG
#include "usbd_cdc_if.h"
#endif
#include <stdarg.h>
//#include "../../COMMON_FUNCTIONS/mem_addr.h"
//#include "../../MISSION/res_table.h"
//#include "../../COM/Com_Operations.h"


int buffersize(char *buff);


void myprintf(const char *fmt, ...);

int write(int32_t file, uint8_t *ptr, int32_t len);


/* Main STM operation Functions */

uint32_t USR_GetuTick();
void INIT_VAR();
void Setup();
void Refresh_WDG_STM();
HAL_StatusTypeDef MainToSubSys(UART_HandleTypeDef *uart_port, uint8_t *data, uint16_t len, uint8_t mode);
void ADC1_POLL_MODE_CONVERT();
void ADC1_DMA_CONVERT();
void ADC2_DMA_CONVERT();
void DATA_CONVERSION();
void MAKE_SAT_TEMPS();
void MAKE_SAT_VOLTS();
void Configure_Flash_Pins(void);

void FLASH_INIT_EXECUTION();

uint8_t Check_Reset_Data();
void Send_Reset_Data_Flag();

void Send_ACK_Reset(uint8_t status) ;

void MAIN_FM_MODE();
void MSN_FM_MODE();
void MUX_DISABLE();

void SATELLITE_OPERATION_PC();
void SATELLITE_OPERATION_GS();

void Make_BEACON_Data(void);

void UPDATE_RST_DATA(RSTTx_DATASET_TypeDef *RESET_Tx, RST_CMD TYPE);

uint8_t UPDATE_OPERATION_MODE(OPERA_MODES MODE);
void CHECK_POWER_STATE(uint16_t BATT_VOLT);

void RUN_ADC();

void Make_HK_Data();
#endif /* USR_FUNCTIONS_H_ */
