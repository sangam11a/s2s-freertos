/*
 * variables.c
 *
 *  Created on: Aug 23, 2022
 *      Author: jaddu
 */

#include "../../CONFIGS/variables.h"

HAL_StatusTypeDef UART_STATE;
HAL_StatusTypeDef SPI_STATE;
HAL_StatusTypeDef HAL_STATUS;

uint8_t PC_DATA[DATA_BUFFER_LEN];
uint8_t PC_Data_Len;

uint8_t GS_CALL_SIGN[6];
const uint8_t SAT_NAME_CALL_SIGN[5] = { 'M', 'U', 'N', 'A', 'L'};
uint8_t COM_RX_DATA_BUFFER[COM_RX_DATA_BUFFER_LEN];
uint8_t COM_RX_DATA[COM_RX_DATA_BUFFER_LEN];
uint8_t COM_TX_DATA_BUFFER[COM_TX_DATA_BUFFER_LEN];
uint8_t COM_RX_COUNT;
uint8_t COM_TEMP_Rx;
uint8_t COM_MSN3_TEMP_Rx;
uint8_t COM_MSN3_RX_COUNT;
uint8_t GS_CMD[CMD_LEN];
uint8_t COM_MSN3_RX_DATA[COM_RX_DATA_BUFFER_LEN];

DEVICE_ID Dev_ID_Main;
DEVICE_ID Dev_ID_Shared;

float ADC_SUP;
uint16_t ADC1_DMA_TEMP[I_ADC1_DATA_LEN];
uint16_t ADC2_DMA_TEMP[I_ADC2_DATA_LEN];

volatile uint16_t I_ADC1_RAW[I_ADC1_DATA_LEN][ADC_LOOP_MAX];
volatile uint16_t I_ADC2_RAW[I_ADC2_DATA_LEN][ADC_LOOP_MAX];

int averageRaw[I_ADC1_DATA_LEN];
int averageRaw_2[I_ADC2_DATA_LEN];

float I_ADC1_Data[I_ADC1_DATA_LEN];
float I_ADC2_Data[I_ADC2_DATA_LEN];

uint16_t E_ADC0_RAW[E_ADC_0_DATA_LEN];
uint16_t E_ADC1_RAW[E_ADC_1_DATA_LEN];

float EXT_ADC_0[E_ADC_0_DATA_LEN];
float EXT_ADC_1[E_ADC_1_DATA_LEN];
float EXT_ADC_0_TEMP[E_ADC_0_DATA_LEN];

float TEMPS[10];

//union float_to_uint8_t F2I;

// storing the temperature, converted to uin8_t, in 2D format row separates to the next sensor
uint8_t SAT_TEMPS[10][2];
uint8_t SAT_VOLTS[8][2];
uint8_t SAT_CURR[14][2];
uint8_t SAT_IMU[9][2];
uint16_t SAT_RSSI;
uint8_t RSV_TABLE[RSV_TABLE_SIZE + 1];

uint8_t RESET_REFRESH_DATA[RESET_DATA_LEN];
uint8_t RESET_Data_Buff[RESET_DATA_LEN];
RSTRx_DATASET_TypeDef RESET_Rx;
RSTTx_DATASET_TypeDef RESET_Tx;
uint8_t RST_TEMP_Rx;
uint8_t RST_TEMP2_Rx;
uint8_t RST_Count;
uint8_t RST2_Count;

uint8_t HK_Data_Buff[HK_DATA_LEN];
uint8_t MSN1_Data_Buff[DATA_BUFFER_LEN];
uint8_t MSN2_Data_Buff[DATA_BUFFER_LEN];
uint8_t MSN3_Data_Buff[DATA_BUFFER_LEN];
uint8_t FLAG_DATA_BUFF[FLAG_DATA_LEN];
uint8_t SAT_LOG_BUFF[SAT_LOG_LEN];
uint8_t BEACON_DATA[BEACON_DATA_LEN];
SAT_HEALTH_TypeDef SAT_HEALTH;
SOLAR_PWR_GEN	SOLAR_STATUS;
SAT_BEACON_TYPE_A BEACON_TYPE_A;
SAT_BEACON_TYPE_B BEACON_TYPE_B;


uint8_t MSN_TIME;

volatile uint16_t R_mSEC;
volatile uint16_t mSEC;
volatile uint16_t TIM_DAY;
volatile uint8_t TIM_SEC;
volatile uint8_t TIM_MIN;
volatile uint8_t TIM_HOUR;
volatile uint16_t TIM_DAY;
volatile uint8_t HK_TIME_COUNTER;
uint8_t LAST_RST_HOUR;
volatile uint8_t BEACON_TIMER;
volatile uint8_t BEACON_TYPE;
volatile uint8_t RSV_MIN;

/* Flags Definition*/
uint8_t ADC1_CONV_CPLT_FLAG;
uint8_t ADC2_CONV_CPLT_FLAG;
uint8_t DEBUG_DATA_RX_FLAG;
uint8_t COM_DATA_RX_FLAG;

uint8_t RESET_DATA_RX_FLAG;

uint8_t MSN1_DATA_RX_FLAG;
uint8_t MSN2_DATA_RX_FLAG;
uint8_t MSN3_DATA_RX_FLAG;
uint8_t DEBUG_DATA_TX_FLAG;
uint8_t COM_DATA_TX_FLAG;
uint8_t COM_MSN3_RX_FLAG;
uint8_t RESET_DATA_TX_FLAG;

uint8_t MSN1_DATA_TX_FLAG;
uint8_t MSN2_DATA_TX_FLAG;
uint8_t MSN3_DATA_TX_FLAG;
uint8_t BEACON_FLAG;

/*Reservation Variables*/
uint8_t RSV_CHECK;
uint8_t RSV_COUNT;
uint8_t RSV_CMD_1;
uint8_t RSV_CMD_2;
uint8_t RSV_CMD_3;
uint8_t RSV_CMD_4;
uint8_t RSV_TIME;

uint8_t MSN_STATUS;

uint8_t GLOBAL_SYS_RST_FLAG;
volatile uint8_t I_ADC1_CONV_CPLT;
uint8_t ANT_DEPLOY_STATUS;
uint8_t DCOMMAND_RX_SUCCESS;
uint8_t MSN1_FLAG;
uint8_t MSN2_FLAG;
uint8_t MSN3_FLAG;
uint8_t KILL_SWITCH;

volatile uint8_t PC_CMD;
volatile uint8_t HK_FLAG;
OPERA_MODES OPER_MODES;
CRITIC_FLAG OPERA_FLAGS;
uint32_t ERR_CODE;

/*Current addresses of the Corresponding Missions of allocated space*/
uint32_t HK_DATA_CURRENT_ADDRESS;
uint32_t SAT_LOG_CURRENT_ADDRESS;
uint32_t RAD_MSN_DATA_CURRENT_ADDRESS;
uint32_t PACDS_MSN_DATA_CURRENT_ADDRESS;
uint32_t SPDM_MSN_DATA_CURRENT_ADDRESS;
uint32_t CAM_MSN_DATA_CURRENT_ADDRESS;
uint32_t FLAG_DATA_CURRENT_ADDRESS;
uint32_t RSV_DATA_CURRENT_ADDRESS;
uint32_t CAM_MSN_IMG_INFO_CURRENT_ADDRESS;
uint32_t ADDRESS_DATA_CURRENT_ADDRESS;
uint8_t ADDR_BUFFER[ADDRESS_DATA_SIZE];

const uint32_t	INT_FLAG_STORE_SECTOR = 21;
const uint32_t 	INT_FLAG_STORE_ADDRESS = 0x081A0000;

const uint32_t ADDRESS_DATA_LOC_ADDRESS_INT=0x081C0000;
const uint32_t ADDRESS_LOC_SECTOR = 22;

const uint32_t ADDRESS_DATA_SECTOR = 23;
const uint32_t ADDRESS_DATA_ADDRESS_INT= 0x081E0000;

/*register status for Flash Memory*/
uint8_t reg_stat;
uint8_t reg_stat_flag;
