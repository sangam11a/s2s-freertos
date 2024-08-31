/*
 * variables.h
 *
 *  Created on: Aug 23, 2022
 *      Author: jaddu
 */
#pragma once

#ifndef INC_VARIABLES_H_
#define INC_VARIABLES_H_

#include "../../CONFIGS/MAIN_CONFIG.h"
#include "../../FLASH_MEMORY/MT25QL.h"

extern HAL_StatusTypeDef UART_STATE;   // stores UART BUS state temporarily
extern HAL_StatusTypeDef SPI_STATE;    // stores SPI BUS state
extern HAL_StatusTypeDef HAL_STATUS;

extern uint8_t PC_DATA[DATA_BUFFER_LEN]; // data buffer to store data sent from PC via USB
extern uint8_t PC_Data_Len;	// variable storing the length of data arriving from PC

extern uint8_t GS_CALL_SIGN[6];
extern const uint8_t SAT_NAME_CALL_SIGN[5];
extern uint8_t COM_RX_DATA_BUFFER[COM_RX_DATA_BUFFER_LEN];
extern uint8_t COM_RX_DATA[COM_RX_DATA_BUFFER_LEN];
extern uint8_t COM_TX_DATA_BUFFER[COM_TX_DATA_BUFFER_LEN];
extern uint8_t COM_RX_COUNT;
extern uint8_t COM_TEMP_Rx;
extern uint8_t COM_MSN3_TEMP_Rx;
extern uint8_t COM_MSN3_RX_COUNT;
extern uint8_t GS_CMD[CMD_LEN];	// buffer to store command received from GS before execution
extern uint8_t COM_MSN3_RX_DATA[COM_RX_DATA_BUFFER_LEN];

extern DEVICE_ID Dev_ID_Main;		//Main FM Read ID data
extern DEVICE_ID Dev_ID_Shared;

/////////////////////////////////////////////////

extern float ADC_SUP;							// acutal ADC supply voltage

extern uint16_t ADC1_DMA_TEMP[I_ADC1_DATA_LEN];
extern uint16_t ADC2_DMA_TEMP[I_ADC2_DATA_LEN];

extern volatile uint16_t I_ADC1_RAW[I_ADC1_DATA_LEN][ADC_LOOP_MAX];
extern volatile uint16_t I_ADC2_RAW[I_ADC2_DATA_LEN][ADC_LOOP_MAX];

extern int averageRaw[I_ADC1_DATA_LEN];
extern int averageRaw_2[I_ADC2_DATA_LEN];

extern float I_ADC1_Data[I_ADC1_DATA_LEN]; // used to store the data converted by Internal ADC1
extern float I_ADC2_Data[I_ADC2_DATA_LEN]; // used to store the data converted by Internal ADC2

extern uint16_t E_ADC0_RAW[E_ADC_0_DATA_LEN];
extern uint16_t E_ADC1_RAW[E_ADC_1_DATA_LEN];

extern float EXT_ADC_0[E_ADC_0_DATA_LEN];			//8 channels
extern float EXT_ADC_1[E_ADC_1_DATA_LEN];			//8 channels
extern float EXT_ADC_0_TEMP[E_ADC_0_DATA_LEN];
extern float TEMPS[10];	// all the temperature datas of the satellite in floating

//extern union float_to_uint8_t F2I;

extern uint8_t SAT_TEMPS[10][2];
extern uint8_t SAT_VOLTS[8][2];
extern uint8_t SAT_CURR[14][2];
extern uint8_t SAT_IMU[9][2];
extern uint8_t SAT_IMU1[17];


////////////////////////////////////////////////

extern uint8_t RESET_REFRESH_DATA[RESET_DATA_LEN];// store data to be sent to RESET_STM
extern uint8_t RESET_Data_Buff[RESET_DATA_LEN]; // data receiving from Reset_STM
extern RSTRx_DATASET_TypeDef RESET_Rx;
extern RSTTx_DATASET_TypeDef RESET_Tx;
extern uint8_t RST_TEMP_Rx;
extern uint8_t RST_TEMP2_Rx;

extern uint8_t RST_Count;
extern uint8_t RST2_Count;

extern uint8_t RSV_TABLE[RSV_TABLE_SIZE + 1];//to store the reservation table data
extern uint8_t HK_Data_Buff[HK_DATA_LEN];	//make and store the HK data in FM
extern uint8_t MSN1_Data_Buff[DATA_BUFFER_LEN]; // data receiving buffer from MSN1
extern uint8_t MSN2_Data_Buff[DATA_BUFFER_LEN]; // data receiving buffer from MSN2
extern uint8_t MSN3_Data_Buff[DATA_BUFFER_LEN]; // data receiving buffer from MSN3
extern uint8_t FLAG_DATA_BUFF[FLAG_DATA_LEN];
extern uint8_t SAT_LOG_BUFF[SAT_LOG_LEN];
extern uint8_t BEACON_DATA[BEACON_DATA_LEN];
extern SAT_HEALTH_TypeDef SAT_HEALTH;
extern SOLAR_PWR_GEN SOLAR_STATUS;
extern SAT_BEACON_TYPE_A BEACON_TYPE_A;
extern SAT_BEACON_TYPE_B BEACON_TYPE_B;

extern uint8_t MSN_TIME;

extern volatile uint16_t R_mSEC;   // millisecond counter for REset
extern volatile uint16_t mSEC;	// timing counter
extern volatile uint16_t TIM_DAY; // day counter
extern volatile uint8_t TIM_SEC; // second counter
extern volatile uint8_t TIM_MIN; // Minute coutner
extern volatile uint8_t TIM_HOUR; // hour counter
extern volatile uint16_t TIM_DAY; // Day counter
extern volatile uint8_t HK_TIME_COUNTER; // stores Last HK_Data reception time
extern volatile uint8_t BEACON_TIMER;
extern volatile uint8_t BEACON_TYPE;
extern uint8_t LAST_RST_HOUR;
extern volatile uint8_t RSV_MIN;//count the reservation time for RSV command execution

/* Flags Definition*/
extern uint8_t ADC1_CONV_CPLT_FLAG;
extern uint8_t ADC2_CONV_CPLT_FLAG;
extern uint8_t DEBUG_DATA_RX_FLAG;
extern uint8_t COM_DATA_RX_FLAG;
extern uint8_t COM_MSN3_RX_FLAG;

extern uint8_t RESET_DATA_RX_FLAG;

extern uint8_t MSN1_DATA_RX_FLAG;
extern uint8_t MSN2_DATA_RX_FLAG;
extern uint8_t MSN3_DATA_RX_FLAG;
extern uint8_t DEBUG_DATA_TX_FLAG;
extern uint8_t COM_DATA_TX_FLAG;

extern uint8_t RESET_DATA_TX_FLAG;

extern uint8_t MSN1_DATA_TX_FLAG;
extern uint8_t MSN2_DATA_TX_FLAG;
extern uint8_t MSN3_DATA_TX_FLAG;
extern uint8_t BEACON_FLAG;

extern uint8_t RSV_CHECK;		//to indicate if the reservation table is in use
extern uint8_t RSV_CMD_1;		//Reservation command(s) stored in the table
extern uint8_t RSV_CMD_2;
extern uint8_t RSV_CMD_3;
extern uint8_t RSV_CMD_4;
extern uint8_t RSV_TIME;	//closest time for execution of reservation command

extern uint8_t MSN_STATUS;

extern uint8_t GLOBAL_SYS_RST_FLAG;		// flag used to initiate Global reset
extern volatile uint8_t I_ADC1_CONV_CPLT;// flag to indicate the completion of conversion by ADC
extern uint8_t ANT_DEPLOY_STATUS;// Flag to store the status of Antenna Deployment
extern uint8_t DCOMMAND_RX_SUCCESS;	// Flag used to receive Commands from PC (Debugging and testing phase)
extern uint8_t MSN1_FLAG;	// Flag indicating the activation of MSN1 (CAM_MSN)
extern uint8_t MSN2_FLAG;// Flag indicating the activation of MSN2 (PADCS/RAD_MSN)
extern uint8_t MSN3_FLAG;	// Flag indicating the activation of MSN3 (SPDM_MSN)
extern uint8_t KILL_SWITCH;

extern volatile uint8_t PC_CMD;	// Flag indicating the reception of command from PC
extern volatile uint8_t HK_FLAG;	// Flag indicating HK data collection time
extern OPERA_MODES OPER_MODES;
extern CRITIC_FLAG OPERA_FLAGS;

extern uint32_t ERR_CODE;					// used for storing the error codes

extern uint32_t HK_DATA_CURRENT_ADDRESS;
extern uint32_t SAT_LOG_CURRENT_ADDRESS;
extern uint32_t RAD_MSN_DATA_CURRENT_ADDRESS;
extern uint32_t PACDS_MSN_DATA_CURRENT_ADDRESS;
extern uint32_t SPDM_MSN_DATA_CURRENT_ADDRESS;
extern uint32_t CAM_MSN_DATA_CURRENT_ADDRESS;
extern uint32_t FLAG_DATA_CURRENT_ADDRESS;
extern uint32_t RSV_DATA_CURRENT_ADDRESS;
extern uint32_t CAM_MSN_IMG_INFO_CURRENT_ADDRESS;
extern uint32_t ADDRESS_DATA_CURRENT_ADDRESS;
extern uint8_t ADDR_BUFFER[ADDRESS_DATA_SIZE];

// intenal flash operation variables

extern const uint32_t INT_FLAG_STORE_SECTOR;
extern const uint32_t INT_FLAG_STORE_ADDRESS;

extern const uint32_t ADDRESS_DATA_LOC_ADDRESS_INT;
extern const uint32_t ADDRESS_LOC_SECTOR;

extern const uint32_t ADDRESS_DATA_SECTOR;
extern const uint32_t ADDRESS_DATA_ADDRESS_INT;

extern uint8_t reg_stat;
extern uint8_t reg_stat_flag;
#endif /* INC_VARIABLES_H_ */
