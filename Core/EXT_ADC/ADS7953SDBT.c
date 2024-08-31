/*
 * ADS7953SDBT.c
 *
 *  Created on: Aug 4, 2022
 *      Author: Suvesh
 */

//Include files
#include "ADS7953SDBT.h"
#include "math.h"

uint8_t adc_data[2];
extern UART_HandleTypeDef huart7;
uint8_t adc1_data[2];

/*
 * @brief		function to enable the ADC
 *
 * @retval 		null
 *
 * @param 		null
 */
void ADC_Enable() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_Delay(1);
}

/*
 * @brief		function to enable the ADC1
 *
 * @retval 		null
 *
 * @param 		null
 */
void ADC1_Enable() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_Delay(1);
}

/*
 * @brief 		function to disable the ADC
 *
 * @retval		null
 *
 * @param		null
 */
void ADC_Disable() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_Delay(1);
}

/*
 * @brief 		function to disable the ADC1
 *
 * @retval		null
 *
 * @param		null
 */
void ADC1_Disable() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_Delay(1);
}



/*
 * @brief		function to select MANUAL mode for ADC
 *
 * @retval		null
 *
 * @param		null
 */
void MANUAL_Select() {
	uint8_t command[2];
	uint16_t temp;

	temp = MANUAL_MODE;
	command[0] = temp >> 8;
	command[1] = temp & 0xff;

	ADC1_Enable();
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&E_ADC, command, adc_data, 2, 1000);
	HAL_Delay(1);
	ADC1_Disable();
	HAL_Delay(1);
}

/*
 * @brief		function to select AUTO-2 mode for ADC
 *
 * @retval 		null
 *
 * @param		null
 */
void AUTO_2_Select_ADC(operation_modes mode) {
	uint8_t command[2];
	uint16_t temp;
	HAL_StatusTypeDef ret;
	switch (mode) {
	case ADC_SELECT:
		temp = AUTO_2_MODE;
		command[0] = temp >> 8;
		command[1] = temp & 0xff;
		ADC_Enable();
		HAL_Delay(1);
		ret = HAL_SPI_Transmit(&E_ADC, command, 2, 1000);
		HAL_Delay(1);
		ADC_Disable();
		HAL_Delay(1);
		break;
	case ADC_READ:
		temp = AUTO_2_MODE2;
		command[0] = temp >> 8;
		command[1] = temp & 0xff;
		ADC_Enable();
		HAL_Delay(1);
		ret = HAL_SPI_TransmitReceive(&E_ADC, command, adc_data, 2, 1000);
		HAL_Delay(1);
		ADC_Disable();
		HAL_Delay(1);
		break;
	default:
		break;
	}

}

/*
 * @brief		function to select AUTO-2 mode for ADC1
 *
 * @retval 		null
 *
 * @param		null
 */
void AUTO_2_Select_ADC1(operation_modes mode) {
	uint8_t command[2];
	uint16_t temp;
	switch (mode) {
	case ADC_SELECT:
		temp = AUTO_2_MODE;
		command[0] = temp >> 8;
		command[1] = temp & 0xff;
		ADC1_Enable();
		HAL_Delay(1);
		HAL_SPI_Transmit(&E_ADC, command, 2, 1000);	//(to ask) 8 bit ko data type ma change garyo vane data loss hunxa?
		HAL_Delay(1);
		ADC1_Disable();
		HAL_Delay(1);
		break;
	case ADC_READ:
		temp = AUTO_2_MODE2;
		command[0] = temp >> 8;
		command[1] = temp & 0xff;
		ADC1_Enable();
		HAL_Delay(1);
		HAL_SPI_TransmitReceive(&E_ADC, command, adc1_data, 2, 1000);
		HAL_Delay(1);
		ADC1_Disable();
		HAL_Delay(1);
		break;
	default:
		break;
	}
}

/*
 * @brief 		funtion to program AUTO-2 mode for ADC
 *
 * @retval		null
 *
 * @param		null
 */
void ADC_AUTO_2_Program() {
	uint8_t command[2];
	uint16_t temp;

	temp = ADC_AUTO_2_PROGRAM;
	command[0] = temp >> 8;
	command[1] = temp & 0xff;

	ADC_Enable();
	HAL_Delay(1);
	HAL_SPI_Transmit(&E_ADC, command, 2, 1000);
	HAL_Delay(1);
	ADC_Disable();
	HAL_Delay(1);
}

/*
 * @brief 		funtion to program AUTO-2 mode for ADC1
 *
 * @retval		null
 *
 * @param		null
 */
void ADC1_AUTO_2_Program() {
	uint8_t command[2];
	uint16_t temp;

	temp = ADC1_AUTO_2_PROGRAM;
	command[0] = temp >> 8;
	command[1] = temp & 0xff;

	ADC1_Enable();
	HAL_Delay(1);
	HAL_SPI_Transmit(&E_ADC, command, 2, 1000);
	HAL_Delay(1);
	ADC1_Disable();
	HAL_Delay(1);
}
/*******************************************************************************/
/*
 * @brief		function to continue the operation of ADC in the selected mode
 * 				and to receive the sampled data from the ADC
 *
 * @retval		16-bit conversion value of the ADC
 *
 * @param		*data pointer to the variable to store the data of the conversion
 */
/*
 * void Continue_Operaion(uint8_t *data) {
 uint8_t command[2];
 uint16_t temp;
 uint8_t Rx_data[2];

 temp = CONTINUE_OPERATION;
 command[0] = temp >> 8;
 command[1] = temp & 0xff;

 ADC_Enable();
 HAL_SPI_Transmit(&E_ADC, command, 2, 1000);
 HAL_SPI_Receive(&E_ADC, Rx_data, 2, 1000);

 data[0] = Rx_data[0];
 data[1] = Rx_data[1];
 ADC_Disable();
 HAL_Delay(1);
 }
 */
/***********************************************************************************/
/*
 * @brief 		function to combine the ADC data to one variable
 *
 * @retval		null
 *
 * @param		float pointer to the buffer where the converted data is to be stored
 */
void ADC_CombineData(uint16_t *adc_channels, uint16_t *adc1_channels) {
	 int i = 0, j = 0;

	if (i < E_ADC_DATA_LEN) {
		adc_channels[i] = ((adc_data[0] << 8) | adc_data[1]);
		i++;
		if (i == E_ADC_DATA_LEN) {
			char uart_buf[20] ;
			for (int x = 0; x < E_ADC_DATA_LEN; x++) {
				adc_channels[x] = adc_channels[x] & 0x0fff;
				sprintf(uart_buf, "%d \0", adc_channels[x]);
//				HAL_UART_Transmit(&huart7, uart_buf, sizeof(uart_buf),100);
			}
//			HAL_UART_Transmit(&huart7, "Data collection completed\n", sizeof("Data collection completed\n"),100);

		}
	}

//	if (i < E_ADC_0_DATA_LEN) {
//		adc_channels[i] = ((adc_data[0] << 8) | adc_data[1]);
//		i++;
//		if (i == E_ADC_0_DATA_LEN) {
//			for (int x = 0; x < E_ADC_0_DATA_LEN; x++) {
//				adc_channels[x] = adc_channels[x] & 0x0fff;
//			}
//		}
//	}
//
//	if (j < E_ADC_1_DATA_LEN) {
//		adc1_channels[j] = ((adc1_data[0] << 8) | adc1_data[1]);
//		j++;
//		if (j == E_ADC_1_DATA_LEN) {
//			for (int x = 0; x < E_ADC_1_DATA_LEN; x++) {
//				adc1_channels[x] = adc1_channels[x] & 0x0fff;
//			}
//			i = 0;
//			j = 0;
//		}
//	}
}

/*
 * @brief		function to convert the raw data into actual data for ADC
 *
 * @retval		null
 *
 * @param		floating pointer to the buffer where the converted data is to be stored,
 * 				uint16_t pointer to the buffer which contains the raw data
 */
void ADC_Conv_Data(float *adc_conv_buf, uint16_t *adc_buf) {

//	for (int x = 0; x < E_ADC_0_DATA_LEN; x++) {
//		adc_conv_buf[x] = (2.5 * adc_buf[x]) / 4095.0;
//	}
	float dummy;
		float root;

		for (int x = 0; x < E_ADC_DATA_LEN; x++) {

		  if (x < 8) adc_conv_buf[x] = (2.5 * adc_buf[x]) / 4095.0;

	//	for (int x = number_of_current_channel; x < E_ADC_DATA_LEN; x++) {
			  else{
					switch (x) {
							case 8:		// Solar Panel Total Current
								dummy = (2.5 * adc_buf[x]) / 4095.0;
								adc_conv_buf[x] = ((dummy - 1.65) / 264) * 1000;//converting voltage to current. In actual code need to keep separate variable for current
								break;
							case 9:		// Battery current
								dummy = (2.5 * adc_buf[x]) / 4095.0;
								adc_conv_buf[x] = ((dummy - 1.65) / 264) * 1000;//converting voltage to current. In actual code need to keep separate variable for current

								break;
							case 10:  // COM Board Temp data
								dummy = (2.5 * adc_buf[x]) / 4095.0;
								root = sqrtf(
										(5.506 * 5.506) + (4 * 0.00176 * (870.6 - (dummy * 1000))));
								adc_conv_buf[x] = ((5.506 - root) / (2 * (-0.00176))) + 30;
								break;
							case 11:	// COM RSSI analog data
								adc_conv_buf[x] = adc_buf[x];
								break;
							default:		// baki sabai
								dummy = (2.5 * adc_buf[x]) / 4095.0;
								adc_conv_buf[x] = (dummy * (1100.0 + 931.0)) / 931.0;
								break;
							}
			  }
			}
}

/*
 * @brief		function to convert the raw data into actual data for ADC1
 *
 * @retval		null
 *
 * @param		floating pointer to the buffer where the converted data is to be stored,
 * 				uint16_t pointer to the buffer which contains the raw data
 */
void ADC1_Conv_Data(float *adc1_conv_buf, uint16_t *adc1_buf) {
	float dummy;
	float root;

	for (int x = 6; x < E_ADC_1_DATA_LEN; x++) {
		switch (x) {
		case 8:		// Solar Panel Total Current
			dummy = (2.5 * adc1_buf[x]) / 4095.0;
			adc1_conv_buf[x] = ((dummy - 1.65) / 264) * 1000;//converting voltage to current. In actual code need to keep separate variable for current
			break;
		case 9:		// Battery current
			dummy = (2.5 * adc1_buf[x]) / 4095.0;
			adc1_conv_buf[x] = ((dummy - 1.65) / 264) * 1000;//converting voltage to current. In actual code need to keep separate variable for current

			break;
		case 10:  // COM Board Temp data
			dummy = (2.5 * adc1_buf[x]) / 4095.0;
			root = sqrtf(
					(5.506 * 5.506) + (4 * 0.00176 * (870.6 - (dummy * 1000))));
			adc1_conv_buf[x] = ((5.506 - root) / (2 * (-0.00176))) + 30;
			break;
		case 11:	// COM RSSI analog data
			adc1_conv_buf[x] = adc1_buf[x];
			break;
		default:		// baki sabai
			dummy = (2.5 * adc1_buf[x]) / 4095.0;
			adc1_conv_buf[x] = (dummy * (1100.0 + 931.0)) / 931.0;
			break;
		}
	}
}

/*
 * @brief		function to run the whole ADC operation
 *
 * @param		floating pointers to the buffer which stores the converted data.
 *
 * @retval		null
 */
void ADC_Operate(uint16_t *adc_channels, uint16_t *adc1_channels) {
	MANUAL_Select();
	AUTO_2_Select_ADC(ADC_SELECT); // programming cycle of ADC 0
	ADC_AUTO_2_Program();
	AUTO_2_Select_ADC1(ADC_SELECT); // programming cycle of ADC 1
	ADC1_AUTO_2_Program();

	for (int x = 0; x < E_ADC_DATA_LEN; x++) {
		if (x < E_ADC_DATA_LEN) {
			AUTO_2_Select_ADC(ADC_READ);			// Data read cycle of ADC 0

		}
		 			// Data  Read cycle from ADC 1
		ADC_CombineData(adc_channels, adc1_channels);
	}
//		MANUAL_Select();
//		AUTO_2_Select_ADC(ADC_SELECT); // programming cycle of ADC 0
//		ADC_AUTO_2_Program();
//
//		for (int x = 0; x < E_ADC_DATA_LEN; x++) {
//			if (x < E_ADC_DATA_LEN) {
//				AUTO_2_Select_ADC(ADC_READ);			// Data read cycle of ADC 0
//
//			}				// Data  Read cycle from ADC 1
//			ADC_CombineData(adc_channels, adc1_channels);
//	//		ADC1_Conv_Data((float*)adc_channels, adc1_channels);
//		}

}

/*
 * @brief		function to convert the voltage data of ADC1 to temperature data
 *
 * @param		float adc1_conv_buf		pointer to the buffer which is storing the converted raw data(voltage) of ADC1
 * 				float temp_buf			pointer to the buffer which stores the voltage converted to the temperature data.
 * 										The temperature converted is multiplied by 10 to remove floating point of 1 decimal place
 *
 * @retval		null
 */
void ADC_Temp_Conv(float *adc_conv_buf, float *temp_buf) {
	float root = 0;

	for (int i = 0; i < E_ADC_0_DATA_LEN; i++) {

		if (i == 6) {		// Battery temperature channel
			float res = (adc_conv_buf[i] * 10000) / (2.5 - adc_conv_buf[i]);
			float tempK = (3976 * 298) / (3976 - (298 * log(10000 / res)));
			temp_buf[i] = (tempK - 273) * 100;
		} else {
			root =
					sqrtf(
							(5.506 * 5.506)
									+ (4 * 0.00176
											* (870.6 - (adc_conv_buf[i] * 1000))));
			temp_buf[i] = ((((5.506 - root) / (2 * (-0.00176))) + 30))*100;
		}
	}
}

