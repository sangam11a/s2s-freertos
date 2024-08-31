/*
 * internal_adc.c
 *
 *  Created on: Aug 30, 2024
 *      Author: sanga
 */

#include "internal_adc.h"

//void ADC1_DMA_CONVERT(){
//	HAL_ADC_Init(&I_ADC1);
//	HAL_Delay(1);
//	ADC1_CONV_CPLT_FLAG = 0;
//	memset(ADC1_DMA_TEMP, '\0', sizeof(ADC1_DMA_TEMP));
//	memset(I_ADC1_Data, '\0', sizeof(I_ADC1_Data));
//	if (HAL_ADC_Start_DMA(&I_ADC1, (uint32_t*) ADC1_DMA_TEMP, I_ADC1_DATA_LEN)
//				== HAL_OK) {
//			while (ADC1_CONV_CPLT_FLAG < ADC_LOOP_MAX) {
//			}
//
//			ADC1_CONV_CPLT_FLAG = 0;
//
//	#ifdef DEBUG_MODE
//			if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
//	#ifdef TRACE_MODE
//			#else
//				myprintf(" ADC1 DMA Completed\r\n");
//	#endif
//			}
//	#endif
//
//			ADC_SUP = 1.2 * 4095 / (averageRaw[6]);	// finding the true supply to MCU/ADC
//
//			for (int i = 0; i < I_ADC1_DATA_LEN; i++) {
//				I_ADC1_Data[i] = ADC_SUP * averageRaw[i] / 4095;
//			}
//		} else {
//	#ifdef DEBUG_MODE
//			if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
//
//	#ifdef TRACE_MODE
//			#else
//				myprintf("ADC 1 DMA Start Failed.\r\n");
//	#endif
//			}
//	#endif
//		}
//		memset(averageRaw, '\0', sizeof(averageRaw));
//}
