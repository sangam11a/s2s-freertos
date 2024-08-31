/*
 * USR_FUNCTIONS.c
 *
 *  Created on: Aug 24, 2022
 *      Author: jaddu
 */
#include "USR_FUNCTIONS.h"

/*
 * @brief	counts the number of non-null data in given array
 *
 * @param	buff	pointer to the array of data to be counted
 * @retval	int		number of non-null values in the array
 */
int buffersize(char *buff) {
	int i = 0;
	while (*buff++ != '\0')
		i++;
	return i;
}

#ifdef DEBUG_MODE
#ifndef TRACE_MODE

/*
 * @brief	Outputs the string data to the PC, via USB CDC
 *
 * @param	fmt	pointer the array of characters data to be transmitted
 *
 * @retval	none
 */
void myprintf(const char *fmt, ...) {
	static char temp[100];
	va_list args;
	va_start(args, fmt);
	vsnprintf(temp, sizeof(temp), fmt, args);
	va_end(args);
	int len = buffersize(temp);
#ifdef CDC_USB_DEBUG
	CDC_Transmit_FS((uint8_t*) temp, len);
//	HAL_Delay(1);
#endif
#ifdef UART_DEBUG
	// Add the code here for printing data to serial monitor via software simulated UART
//	while (DEBUG_DATA_TX_FLAG != 0) {
//	}
	uint32_t current_tick = HAL_GetTick();
	while (HAL_UART_GetState(&DEBUG_STREAM) == HAL_UART_STATE_BUSY_TX) {
		if (HAL_GetTick() - current_tick > 2000) {
			break;
		}
	}
	DEBUG_DATA_TX_FLAG = 0;
	uint32_t tickstart_ = HAL_GetTick();
	HAL_UART_Transmit_IT(&DEBUG_STREAM, (uint8_t*) temp, len);

	while (DEBUG_DATA_TX_FLAG != 1 && (HAL_GetTick() - tickstart_ < 2000)) {
	}
	DEBUG_DATA_TX_FLAG = 0;
#endif
}
#endif

/*
 * @brief	transfer debug information to SWTrace analyzer in Trace Debug mode of STLink
 *
 */
#ifdef TRACE_MODE
int _write(int32_t file, uint8_t *ptr, int32_t len)
{
    for (int i = 0; i < len; i++)
    {
        ITM_SendChar(*ptr++);
    }
    return len;
}
#endif
#endif

/*
 * @brief provides the microsecond tick
 *
 * @param	none
 * @retval	uint32_t value of current tick
 */
uint32_t USR_GetuTick() {
//	__HAL_TIM_SET_COUNTER(&uS_Delay_TIMER, 0);
	return __HAL_TIM_GET_COUNTER(&uS_Delay_TIMER);

}

/*
 * @brief	Initialises all the declared flag for system operation
 * @param	none
 * @retval 	none
 */
void INIT_VAR() {

	UART_STATE = 0;
	SPI_STATE = 0;
	HAL_STATUS = 0;

	PC_Data_Len = 0;

//	F2I.f = 0.0;

	memset(PC_DATA, '\0', DATA_BUFFER_LEN);
	memset(COM_RX_DATA_BUFFER, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(COM_RX_DATA, '\0', COM_RX_DATA_BUFFER_LEN);
	memset(COM_TX_DATA_BUFFER, '\0', COM_TX_DATA_BUFFER_LEN);
	memset(GS_CMD, '\0', CMD_LEN);

	ADC_SUP = 0;

	memset(ADC1_DMA_TEMP, '\0', sizeof(ADC1_DMA_TEMP));
	memset(ADC2_DMA_TEMP, '\0', sizeof(ADC2_DMA_TEMP));
	memset((char*) I_ADC1_RAW, '\0', sizeof(I_ADC1_RAW));
	memset((char*) I_ADC2_RAW, '\0', sizeof(I_ADC2_RAW));
	memset(averageRaw, '\0', sizeof(averageRaw));
	memset(averageRaw_2, '\0', sizeof(averageRaw_2));
	memset(I_ADC1_Data, '\0', sizeof(I_ADC1_Data));
	memset(I_ADC2_Data, '\0', sizeof(I_ADC2_Data));

	memset(E_ADC0_RAW, '\0', sizeof(E_ADC0_RAW)); // E_ADC_RAW is a 2 byte(uint16_t) variable, so multiply the length by no of bytes per index
	memset(E_ADC1_RAW, '\0', sizeof(E_ADC1_RAW));
	memset(EXT_ADC_0, '\0', sizeof(EXT_ADC_0));
	memset(EXT_ADC_1, '\0', sizeof(EXT_ADC_1));
	memset(EXT_ADC_0_TEMP, '\0', sizeof(EXT_ADC_0_TEMP));
	memset(TEMPS, '\0', sizeof(TEMPS));

	memset(SAT_TEMPS, '\0', sizeof(SAT_TEMPS));
	memset(SAT_VOLTS, '\0', sizeof(SAT_VOLTS));
	memset(SAT_CURR, '\0', sizeof(SAT_CURR));
	memset(SAT_IMU, '\0', sizeof(SAT_IMU));

	memset(RSV_TABLE, '\0', sizeof(RSV_TABLE));
	memset(RESET_REFRESH_DATA, '\0', RESET_DATA_LEN);
	memset(RESET_Data_Buff, '\0', RESET_DATA_LEN);

	memset(&RESET_Rx, '\0', sizeof(RESET_Rx));
	memset(&RESET_Tx, '\0', sizeof(RESET_Tx));

	memset(HK_Data_Buff, '\0', HK_DATA_LEN);

	memset(MSN1_Data_Buff, '\0', DATA_BUFFER_LEN);
	memset(MSN2_Data_Buff, '\0', DATA_BUFFER_LEN);
	memset(MSN3_Data_Buff, '\0', DATA_BUFFER_LEN);
	memset(FLAG_DATA_BUFF, '\0', FLAG_DATA_LEN);
	memset(SAT_LOG_BUFF, '\0', SAT_LOG_LEN);
	memset(BEACON_DATA, '\0', BEACON_DATA_LEN);
	memset(&SAT_HEALTH, '\0', sizeof(SAT_HEALTH));
	memset(&SOLAR_STATUS, '\0', SOLAR_STATUS_DATA_LEN);
	memset(&BEACON_TYPE_A, '\0', 32);
	memset(&BEACON_TYPE_B, '\0', 32);

	R_mSEC = 0;
	mSEC = 0;
	TIM_DAY = 0;
	TIM_SEC = 0;
	TIM_MIN = 0;
	TIM_HOUR = 0;
	TIM_DAY = 0;
	HK_TIME_COUNTER = 0;
	BEACON_TIMER = 0;
	BEACON_TYPE = 0;
	LAST_RST_HOUR = 0;

	RSV_MIN = 0;
	MSN_TIME = 0;

	ADC1_CONV_CPLT_FLAG = 0;
	ADC2_CONV_CPLT_FLAG = 0;
	DEBUG_DATA_RX_FLAG = 0;
	COM_DATA_RX_FLAG = 0;
	RESET_DATA_RX_FLAG = 0;
	MSN1_DATA_RX_FLAG = 0;
	MSN2_DATA_RX_FLAG = 0;
	MSN3_DATA_RX_FLAG = 0;
	DEBUG_DATA_TX_FLAG = 0;
	COM_DATA_TX_FLAG = 0;
	RESET_DATA_TX_FLAG = 0;
	MSN1_DATA_TX_FLAG = 0;
	MSN2_DATA_TX_FLAG = 0;
	MSN3_DATA_TX_FLAG = 0;
	BEACON_FLAG = 0;
	COM_TEMP_Rx = 0;
	COM_MSN3_TEMP_Rx = 0;
	COM_MSN3_RX_COUNT = 0;

	SAT_HEALTH.HEAD = HEADER;

	OPER_MODES = NRML_MODE;

	OPERA_FLAGS.MSN1_EN_FLAG = ON;
	OPERA_FLAGS.MSN2_EN_FLAG = ON;
	OPERA_FLAGS.MSN3_EN_FLAG = ON;
	OPERA_FLAGS.COM_EN_FLAG = ON;
	OPERA_FLAGS.LOG_EN_FLAG = ON;
	OPERA_FLAGS.FM_EN_FLAG = ON;
	OPERA_FLAGS.RSV_MSN_EXE_FLAG = ON;
	OPERA_FLAGS.TEL_TX_FLAG = ON;

	RSV_CHECK = 0;
	RSV_CMD_1 = 0;
	RSV_CMD_2 = 0;
	RSV_CMD_3 = 0;
	RSV_CMD_4 = 0;
	RSV_TIME = 1;

	MSN_STATUS = 0;

	GLOBAL_SYS_RST_FLAG = 0;
	I_ADC1_CONV_CPLT = 0;
	ANT_DEPLOY_STATUS = 0;
	DCOMMAND_RX_SUCCESS = 0;
	MSN1_FLAG = 0;
	MSN2_FLAG = 0;
	MSN3_FLAG = 0;
	KILL_SWITCH = 0;

	PC_CMD = 0;
	HK_FLAG = 0;

	ERR_CODE = 0;

	HK_DATA_CURRENT_ADDRESS = HK_DATA_START_ADDRESS;
	SAT_LOG_CURRENT_ADDRESS = SAT_LOG_START_ADDRESS;
//	RAD_MSN_DATA_CURRENT_ADDRESS = 0;
	PACDS_MSN_DATA_CURRENT_ADDRESS = PACDS_MSN_DATA_START_ADDRESS;
	SPDM_MSN_DATA_CURRENT_ADDRESS = SPDM_MSN_DATA_START_ADDRESS;
	CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_DATA_START_ADDRESS;
	CAM_MSN_IMG_INFO_CURRENT_ADDRESS = CAM_MSN_IMG_INFO_START_ADDRESS;
	FLAG_DATA_CURRENT_ADDRESS = FLAG_DATA_START_ADDRESS;
	RSV_DATA_CURRENT_ADDRESS = RSV_TABLE_START_ADDRESS;
	ADDRESS_DATA_CURRENT_ADDRESS = ADDRESS_DATA_START_ADDRESS;

	memset(ADDR_BUFFER, '\0', ADDRESS_DATA_SIZE);

	reg_stat = 0;
	reg_stat_flag = 0;

}

/*
 * @brief	initial function to run after every reset
 * 			initializes the FLASH, Loads Flags, Address
 * 			and Buffers necessary for the operation
 *
 * @param	none
 * @retval	none
 */
void Setup() {

	INIT_VAR();			//initializing all the variables

	HAL_TIM_Base_Start_IT(&RESET_TIMER); // start the reset refresh timer in Interrupt mode (1 ms interrupt)
	HAL_TIM_Base_Start(&uS_Delay_TIMER); // Start timer for introduce microsecond delay, normal mode
	HAL_TIM_Base_Start_IT(&HK_TIMER); // Start timer to time the HK data collection, Interrupt mode, (1s interrupt timer)

//	HAL_ADC_DeInit(&I_ADC1);		//deinitializing the ADC1
//	HAL_ADC_DeInit(&I_ADC2);		//Deinitializing the ADC2

#ifdef CDC_USB_DEBUG
	HAL_Delay(1000); //Waiting for System to get initialized and USB to be connected
#else
	HAL_Delay(300);
#endif

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("\r\n\r\nMUNAL MAIN MCU, Initializing.....\r\n");
#endif
#else
	HAL_Delay(100);
#endif
	HAL_UART_Receive_IT(&RESET_STREAM, &RST_TEMP_Rx, 1);
	HAL_UART_Receive_IT(&RESET_ALT_STREAM, &RST_TEMP2_Rx, 1);
	HAL_UART_Receive_IT(&DEBUG_STREAM, PC_DATA, CMD_LEN);

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("Initialised Receiving Stream, form Reset and Debug Port.\r\n");
#else
	#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	//initial execution of the Flash Memory(reading Device ID, Getting latest address, updating Flags, Checking RSV table)
	Reset_CMD_PWR_ON_OFF(1);
	Reset_CMD_PWR_ON_OFF(20);
	Reset_CMD_PWR_ON_OFF(40);
	Configure_Flash_Pins();		//setting the hold, WP pins of FM high
	FLASH_INIT_EXECUTION();
	Send_Reset_Data_Flag();
	uint8_t i = 0;
	while (!Check_Reset_Data() && i < 3) {
		Send_Reset_Data_Flag();
		i++;
	}
	HAL_Delay(100);

	ADC1_DMA_CONVERT();
	ADC2_DMA_CONVERT();
	GET_EXT_ADC_DATA();
	DATA_CONVERSION();
	MAKE_SAT_TEMPS();
	MAKE_SAT_VOLTS();
	CHECK_POWER_STATE(SAT_HEALTH.BATT_VOLT);

	INIT_COM();

	Reset_CMD_PWR_ON_OFF(1);		// turn on 3v3_2 line for com power on

	if (TIM_HOUR < 1 && TIM_MIN < 30 && DCOMMAND_RX_SUCCESS != UL_RX
			&& ANT_DEPLOY_STATUS == DEPLOYED) {
		ANT_DEPLOY_STATUS = UNDEPLOYED; // resetting ant deployment flag at the start of every day, if UL flag is not set to Deployed state

	}

	if (KILL_SWITCH) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Turning on kill switch");
#endif
#endif
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);	//enabling kill switch1 driver

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET); // KILL_SWITCH2_-
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); // KILL_SWITCH_+
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);	//enabling kill switch2 driver
//		HAL_Delay(10);

//		HAL_Delay(10);
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // disabling kill switch2 driver
//		HAL_Delay(10);

		uint8_t status1 = HAL_GPIO_ReadPin(KILL_SWITCH1_STAT_GPIO_Port,
		KILL_SWITCH1_STAT_Pin);
		uint8_t status2 = HAL_GPIO_ReadPin(KILL_SWITCH2_STAT_GPIO_Port,
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

	} else {
		TURN_OFF_KILL_SWITCH();
	}
}

/*
 * @brief	to execute the initial setup needed for FM
 *
 * 	updating and loading the latest address from Flash
 * 	Loading reservation table data
 *	Loading Flag data
 *	Loading Latest Mission Address data
 *
 */
void FLASH_INIT_EXECUTION() {

#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("Resetting Flash Memory.\r\n");
	HAL_Delay(1);
#else
	#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
//	Reset_Memory(&SMSN_FM);
//	Reset_Memory(&MAIN_FM);

	Read_ID(&MAIN_FM, &Dev_ID_Main);
	MAIN_FM_MODE();
	Read_ID(&SMSN_FM, &Dev_ID_Shared);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Main FM Device Info ............\r\n");
	myprintf("Device ID: %x\r\n", Dev_ID_Main.MAN_ID);
	myprintf("Memory Capacity: %x\r\n", Dev_ID_Main.M_CAP);
	myprintf("Memory type:%x\r\n", Dev_ID_Main.M_TYPE);
	myprintf("Device Info: %x\r\n", Dev_ID_Main.DEV_INFO);
	myprintf("Extended ID: %x\r\n", Dev_ID_Main.EXT_ID);
	myprintf("Device Memory Type: %x\r\n", Dev_ID_Main.M_CAP);
	myprintf("Remaining Bytes: %x\r\n", Dev_ID_Main.REM_BYTES);
	myprintf("UID: %x\r\n");
	for (int i = 0; i < 14; i++) {
		myprintf("%x", Dev_ID_Main.UID[i]);
	}

	myprintf("......................................\r\n");
	myprintf("Shared FM Device Info ................\r\n");
	myprintf("Device ID shared: %x\r\n", Dev_ID_Shared.MAN_ID);
	myprintf("Memory Capacity shared: %x\r\n", Dev_ID_Shared.M_CAP);
	myprintf("Memory type shared:%x\r\n", Dev_ID_Shared.M_TYPE);
	myprintf("Device Info shared: %x\r\n", Dev_ID_Shared.DEV_INFO);
	myprintf("Extended ID shared: %x\r\n", Dev_ID_Shared.EXT_ID);
	myprintf("Device Memory Type shared: %x\r\n", Dev_ID_Shared.M_CAP);
	myprintf("Remaining Bytes shared: %x\r\n", Dev_ID_Shared.REM_BYTES);
	myprintf("UID: %x\r\n");
	for (int i = 0; i < 14; i++) {
		myprintf("%x", Dev_ID_Shared.UID[i]);
	}
#endif
#endif
	uint16_t NON_V_MAIN, NON_V_SHARED;
	Status_Reg_NonVol(&MAIN_FM, &NON_V_MAIN);
	Status_Reg_NonVol(&SMSN_FM, &NON_V_SHARED);
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("nonvolatiile register read.\r\n main: %4x \r\nshared %4x\r\n",
			NON_V_MAIN, NON_V_SHARED);
	HAL_Delay(1);
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	Get_Address_Current_Address(); //checking for the latest writing address for the flash

	Get_Mission_Data_Address();	//Loading the Mission data address from the Flash
//	Check_CAM_Address();
//	Check_HK_Data_Address();
//	Check_SAT_LOG_Address();
//	Check_PACDS_Address();
//	Check_SPDM_Address();
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("retrieved mission data address from FM......\r\n");
	Print_Address_Location();//to print the address data into the serial monitor
#endif
#endif
	Check_Flag_Data(); ///checking for the flag data from flash memory and updating

	Get_RSV_From_Flash(); //checking if there is any reservation command in the FM

//	HAL_Delay(4000);

}

/*
 * @brief	sends flag data  to watchdog (RESET STM)
 *
 * 			Interrupt is used for sending and receiving data because this function
 * 			is called inside the timer interrupt so delays cannot be given
 *
 * 			Refer to the RESET_OBC command list......
 *
 * @param none
 *
 * @retval none
 */
void Refresh_WDG_STM() {

	memset(RESET_REFRESH_DATA, '\0', RESET_DATA_LEN);

//	RESET_REFRESH_DATA[0] = HEADER;
//	RESET_REFRESH_DATA[1] = 0x21;	//to indicate data is flag data type
//	RESET_REFRESH_DATA[2] = 0x00;
//	RESET_REFRESH_DATA[3] = ANT_DEPLOY_STATUS << 4 | MSN1_FLAG << 3
//			| MSN2_FLAG << 2 | MSN3_FLAG << 1 | HK_FLAG;
//	RESET_REFRESH_DATA[4] = RESET_REFRESH_DATA[1] + RESET_REFRESH_DATA[2]
//			+ RESET_REFRESH_DATA[3];
//	RESET_REFRESH_DATA[5] = FOOTER;
	UPDATE_RST_DATA(&RESET_Tx, RESET_FLAG_TYPE);
	UART_STATE = MainToSubSys(&RESET_STREAM, (uint8_t*) &RESET_Tx,
	RESET_DATA_TX_LEN, 1);
	uint32_t time_count = HAL_GetTick();
	while (RESET_DATA_TX_FLAG != 1) {
		if (HAL_GetTick() - time_count > 50) {
			break;
		}
	}
	if (UART_STATE == HAL_OK && RESET_DATA_TX_FLAG) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("%03i:%02i:%02i:%02i:%03i:: Reset Refresh Data Sent!\r\n",
				TIM_DAY, TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf(
				"%03i:%02i:%02i:%02i:%03i:: Reset Refresh Data Send Error!\r\n",
				TIM_DAY, TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		ERR_CODE = 1;
	}


	RESET_DATA_TX_FLAG = 0;
	UART_STATE = MainToSubSys(&RESET_ALT_STREAM, (uint8_t*) &RESET_Tx,
	RESET_DATA_TX_LEN, 1);
	time_count = HAL_GetTick();
	while (RESET_DATA_TX_FLAG != 1) {
		if (HAL_GetTick() - time_count > 50) {
			break;
		}
	}
	if (UART_STATE == HAL_OK && RESET_DATA_TX_FLAG) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("%03i:%02i:%02i:%02i:%03i:: Reset Alt stream Refresh Data Sent!\r\n",
				TIM_DAY, TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf(
				"%03i:%02i:%02i:%02i:%03i:: Reset alt stream Refresh Data Send Error!\r\n",
				TIM_DAY, TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		ERR_CODE = 1;
	}
}

/*
 * @brief	TO Send the Flag data periodically to reset for updating in the Internal Flash
 */
void Send_Reset_Data_Flag() {

	memset(RESET_Data_Buff, '\0', RESET_DATA_LEN);
//	memset(RESET_REFRESH_DATA, '\0', RESET_DATA_LEN);

	RESET_DATA_TX_FLAG = 0;
	RESET_DATA_RX_FLAG = 0;
	ERR_CODE = 0;

	UPDATE_RST_DATA(&RESET_Tx, RESET_FLAG_TYPE);
	HAL_UART_Transmit_IT(&RESET_STREAM, (uint8_t*) &RESET_Tx,
	RESET_DATA_TX_LEN);
	HAL_UART_Transmit_IT(&RESET_ALT_STREAM, (uint8_t*) &RESET_Tx,
	RESET_DATA_TX_LEN);
	uint32_t current_tick = HAL_GetTick();

	RESET_DATA_RX_FLAG = 0;
	while (!RESET_DATA_TX_FLAG) {
		if (HAL_GetTick() - current_tick > 2000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("RESET_DATA_TX_TIMEOUT.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			break;
		}
	}
	current_tick = HAL_GetTick();
	while (!RESET_DATA_RX_FLAG) {
		if (HAL_GetTick() - current_tick > 3000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("RESET_DATA_RX_TIMEOUT.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			break;
		}
	}
	if (RESET_DATA_RX_FLAG) {
		memcpy(&RESET_Rx, &RESET_Data_Buff, RESET_DATA_RX_LEN);

	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Data not received from Reset...\r\n");

#endif
#endif
		RESET_DATA_RX_FLAG = 0;
		RESET_DATA_TX_FLAG = 0;
	}
}

/*
 * @brief Sends acknowledgement to reset after receiving the data
 *
 * @param	status
 * 			1: ACK Successful
 * 			2: ACK Failure
 * @retval
 *
 */
void Send_ACK_Reset(uint8_t status) {
	uint8_t ACK[6] = { '\0' };
	switch (status) {
	case 1:
		//it means success ACK
		ACK[0] = HEADER;
		ACK[1] = 0x12;
		ACK[2] = 0xAC;
		ACK[3] = FOOTER;
		ACK[4] = 0xff;
		ACK[5] = 0xff;
		HAL_UART_Transmit(&RESET_STREAM, ACK, 6, 1000);
		HAL_UART_Transmit(&RESET_ALT_STREAM, ACK, 6, 1000);
		break;
	case 2:

		ACK[0] = HEADER;
		ACK[1] = 0x12;
		ACK[2] = 0XEE;
		ACK[3] = FOOTER;
		ACK[4] = 0xff;
		ACK[5] = 0xff;
		HAL_UART_Transmit(&RESET_STREAM, ACK, 6, 1000);
		HAL_UART_Transmit(&RESET_ALT_STREAM, ACK, 6, 1000);
		//it means Failure ACK
		break;
	default:
		break;
	}
}

/*
 * @brief		TO check for the Ack data received from Reset MCU
 * 				RESET_Data_Buff[1] == 0xAC -- Success ACK
 * 				RESET_Data_Buff[1] == 0xEE -- Failure ACK
 *
 * @param		none
 *
 * @retval		1 : if Data received is correct
 * 				0 : if the data received is incorrect/not in order
 */
uint8_t Check_Reset_Data() {
	uint16_t CHK_SUM = 0;
	for (int i = 0; i < RESET_DATA_RX_LEN - 3; i++) {
		CHK_SUM += RESET_Data_Buff[i];
	}

	if (RESET_Data_Buff[RESET_DATA_LEN - 1] == FOOTER
			&& CHK_SUM == RESET_Rx.CHK_SUM) {

		RESET_DATA_RX_FLAG = 0;
		RESET_DATA_TX_FLAG = 0;
		ERR_CODE = 0;

		TIM_DAY = RESET_Rx.TIM_DAYS;
		TIM_HOUR = RESET_Rx.TIM_HOUR;
		TIM_MIN = RESET_Rx.TIM_MIN;
		TIM_SEC = RESET_Rx.TIM_SEC;
		GLOBAL_SYS_RST_FLAG = RESET_Rx.GBL_RST_STAT;
		if (ANT_DEPLOY_STATUS == 0x00) {
			ANT_DEPLOY_STATUS = RESET_Rx.ANT_STAT_FLAG;
		}
		SAT_HEALTH.TIM_DAYS = TIM_DAY;
		SAT_HEALTH.TIM_HOUR = TIM_HOUR;
		SAT_HEALTH.TIM_MIN = TIM_MIN;
		SAT_HEALTH.TIM_SEC = TIM_SEC;

		SAT_HEALTH.OBC_ABNL_RST = RESET_Rx.OBC_ABN_RST;
		SAT_HEALTH.RST_ABNL_RST = RESET_Rx.RST_ABN_RST;

		if (RESET_Rx.ACK_BYTE == 0xAC) {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
				myprintf("Data sent to Reset \r\nSuccess ACK received....\r\n");

#endif
			}
#endif

#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
				myprintf("data from Reset: ");
				for (int i = 0; i < RESET_DATA_RX_LEN; i++) {
					myprintf("%02x ", *(&RESET_Rx.HEAD + i));
				}
				myprintf("\r\n");
#endif //ifndef TRACE_MODE
			}
#endif //ifdef DEBUG_MODE
			return 1;

		} else if (RESET_Rx.ACK_BYTE == 0xEE) {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
				myprintf(
						" Data Sent to Reset \r\nACK Failure received....\r\n");
#endif//TRACE_MODe
			}
#endif //DEBUG_MODE

#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
				myprintf("data from Reset: ");
				for (int i = 0; i < RESET_DATA_RX_LEN; i++) {
					myprintf("%02x ", *(&RESET_Rx.HEAD + i));
				}
				myprintf("\r\n");
#endif //ifndef TRACE_MODE
			}
#endif //ifdef DEBUG_MODE

			return 0;
		} else {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
				myprintf(
						"Data Sent to Reset \r\nACK Received but random ACK data.........\r\n");
#endif//TRACE_MODe
			}
#endif//DEBUG_MODE

#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
				myprintf("data from Reset: ");
				for (int i = 0; i < RESET_DATA_RX_LEN; i++) {
					myprintf("%02x ", *(&RESET_Rx.HEAD + i));
				}
				myprintf("\r\n");
#endif //TRACE_MODE
			}
#endif //DEBUG_MODE
			return 0;
		}
	} else {
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Reset Header Footer / CheckSum Error.....\r\n");
#endif //TRACE_MODE
		}
#endif //DEBUG_MODE
		return 0;
	}
}

/*
 * @brief  sends data for specified buffer to the indicated data stream
 *
 * @param PORT UART port
 * @param DATA pointer to the data to be transmitted
 * @param len  length of data to be transmitted
 * @param mode	1: Interrupt Mode
 * 				0: polling mode
 *
 * @retval HAL_StatusTypeDef status of the operation
 *
 */
HAL_StatusTypeDef MainToSubSys(UART_HandleTypeDef *uart_port, uint8_t *data,
		uint16_t len, uint8_t mode) {
	switch (mode) {
	case 0:
		UART_STATE = HAL_UART_Transmit(uart_port, data, len, 2000);
		if (UART_STATE == HAL_OK) {
			if (uart_port == &COM_TCV_STREAM) {
				COM_DATA_TX_FLAG = 1;
			}
			if (uart_port == &RESET_STREAM) {
				RESET_DATA_TX_FLAG = 1;
			}
			if (uart_port == &RESET_ALT_STREAM) {
				RESET_DATA_TX_FLAG = 1;
			}
			if (uart_port == &MSN1_STREAM) {
				MSN1_DATA_TX_FLAG = 1;
			}
			if (uart_port == &MSN2_STREAM) {
				MSN2_DATA_TX_FLAG = 1;
			}
			if (uart_port == &MSN3_STREAM) {
				MSN3_DATA_TX_FLAG = 1;
			}
		} else {
			if (uart_port == &COM_TCV_STREAM) {
				COM_DATA_TX_FLAG = 0;
			}
			if (uart_port == &RESET_STREAM) {
				RESET_DATA_TX_FLAG = 0;
			}
			if (uart_port == &RESET_ALT_STREAM) {
				RESET_DATA_TX_FLAG = 0;
			}
			if (uart_port == &MSN1_STREAM) {
				MSN1_DATA_TX_FLAG = 0;
			}
			if (uart_port == &MSN2_STREAM) {
				MSN2_DATA_TX_FLAG = 0;
			}
			if (uart_port == &MSN3_STREAM) {
				MSN3_DATA_TX_FLAG = 0;
			}
		}
		break;
	case 1:
		UART_STATE = HAL_UART_Transmit_IT(uart_port, data, len);
		if (UART_STATE == HAL_OK) {
//#ifdef DEBUG_MODE
//#ifdef TRACE_MODE
//#else
//			myprintf("UART_IT started..\r\n");
//#endif
//#endif
		}
		break;
	}
	return UART_STATE;
}

/*
 * @brief	converts data from analog line connected to ADC1 initalized channels in Polling MODE
 *
 * @param	none
 * @retval	none
 */
/*
 void ADC1_POLL_MODE_CONVERT() {

 HAL_ADC_Start(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[0] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[1] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[2] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[3] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[4] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[5] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_PollForConversion(&I_ADC1, 10);
 I_ADC1_RAW[6] = HAL_ADC_GetValue(&I_ADC1);
 HAL_ADC_Stop(&I_ADC1);

 #ifdef DEBUG_MODE
 #ifdef TRACE_MODE
 #else
 myprintf(" ADC Poll done\r\n");
 HAL_Delay(10);
 for (int i = 0; i < I_ADC1_DATA_LEN; i++) {
 myprintf("ADC channel %i raw: %lu\r\n ", i, I_ADC1_RAW[i]);
 HAL_Delay(1);
 }
 #endif
 #endif

 ADC_SUP = 1.2 * 4095 / (I_ADC1_RAW[5]);
 for (int i = 0; i < I_ADC1_DATA_LEN; i++) {
 I_ADC1_Data[i] = ADC_SUP * I_ADC1_RAW[i] / 4095;
 }

 #ifdef DEBUG_MODE
 #ifdef TRACE_MODE
 #else
 myprintf("ADC True Supply: %0.2f\r\n", ADC_SUP);
 for (int i = 0; i < I_ADC1_DATA_LEN; i++) {
 myprintf("ADC channel %i data: %0.3f\r\n ", i, I_ADC1_Data[i]);
 HAL_Delay(1);
 }
 #endif
 #endif
 HAL_Delay(200);
 }*/

/*
 * @brief	converts data from analog line connected to ADC2 initalized channels in DMA MODE
 *
 * @param	none
 * @retval	none
 */
void ADC1_DMA_CONVERT() {
	HAL_ADC_Init(&I_ADC1);
	HAL_Delay(2);
	ADC1_CONV_CPLT_FLAG = 0;
	memset(ADC1_DMA_TEMP, '\0', sizeof(ADC1_DMA_TEMP));
	memset(averageRaw, '\0', sizeof(averageRaw));
	memset(I_ADC1_Data, '\0', sizeof(I_ADC1_Data));
	if (HAL_ADC_Start_DMA(&I_ADC1, (uint32_t*) ADC1_DMA_TEMP, I_ADC1_DATA_LEN)
			== HAL_OK) {
		while (ADC1_CONV_CPLT_FLAG < ADC_LOOP_MAX) {
		}

		ADC1_CONV_CPLT_FLAG = 0;

#ifdef DEBUG_MODE
		if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
		#else
			myprintf(" ADC1 DMA Completed\r\n");
#endif
		}
#endif

		ADC_SUP = 1.2 * 4095 / (averageRaw[6]);	// finding the true supply to MCU/ADC

		for (int i = 0; i < I_ADC1_DATA_LEN; i++) {
			I_ADC1_Data[i] = ADC_SUP * averageRaw[i] / 4095;
		}
	} else {
#ifdef DEBUG_MODE
		if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {

#ifdef TRACE_MODE
		#else
			myprintf("ADC 1 DMA Start Failed.\r\n");
#endif
		}
#endif
	}
	memset(averageRaw, '\0', sizeof(averageRaw));

}

/*
 * @brief	converts data from analog line connected to ADC2 initalized channels in DMA MODE
 *
 * @param	none
 * @retval	none
 */
void ADC2_DMA_CONVERT() {
	HAL_ADC_Init(&I_ADC2);
	HAL_Delay(2);
	ADC2_CONV_CPLT_FLAG = 0;
	memset(ADC2_DMA_TEMP, '\0', sizeof(ADC2_DMA_TEMP));
	memset(averageRaw_2, '\0', sizeof(averageRaw_2));
	memset(I_ADC2_Data, '\0', sizeof(I_ADC2_Data));
	HAL_ADC_Start_DMA(&I_ADC2, (uint32_t*) ADC2_DMA_TEMP, I_ADC2_DATA_LEN);

	while (ADC2_CONV_CPLT_FLAG < ADC_LOOP_MAX) {
	}
	ADC2_CONV_CPLT_FLAG = 0;
#ifdef DEBUG_MODE
	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
		myprintf(" ADC2 DMA Completed\r\n");
#endif
	}
#endif
	for (int i = 0; i < I_ADC2_DATA_LEN; i++) {
		I_ADC2_Data[i] = ADC_SUP * averageRaw_2[i] / 4095;
	}
	memset(averageRaw_2, '\0', sizeof(averageRaw_2));
}

/*
 * @brief convert the raw ADC converted data to correct data units/types
 *
 * @param	none
 * @retval	none
 *
 */
void DATA_CONVERSION() {
	float I_ADC_1_CURRENT[I_ADC1_DATA_LEN - 2];
	float I_ADC_2_CURRENT[I_ADC2_DATA_LEN];

	for (int i = 0; i < I_ADC1_DATA_LEN; i++) {

		if (i == 5) {
			// add internal temperature conversion formula
			SAT_HEALTH.OBC_TEMP = (uint16_t) I_ADC1_Data[i];
		} else {
			I_ADC_1_CURRENT[i] = (I_ADC1_Data[i]
					/ (2 * RES_LMP8640 * GAIN_LMP8640)) * 1000;
		}
	}

	for (int i = 0; i < I_ADC2_DATA_LEN; i++) {
		if (i == 5) {
			// use TMCS sensor formula
			// RAW current TMCS sensor
			I_ADC_2_CURRENT[i] = ((I_ADC2_Data[i] - 1.65) / SENS_TMCS) * 1000
					* 1000;
		} else {
			I_ADC_2_CURRENT[i] = (I_ADC2_Data[i]
					/ (2 * RES_LMP8640 * GAIN_LMP8640)) * 1000;
		}
	}

	for (uint8_t i = 0; i < 5; i++) {
		SOLAR_STATUS.SOLAR_CURRENT[i] = (uint16_t) (I_ADC_1_CURRENT[i + 2]);
	}

	SAT_HEALTH.HEAD = HEADER;
	SAT_HEALTH.RST_3V3_C = (int16_t) (I_ADC_1_CURRENT[4]);

	SAT_HEALTH.SOL_P1_C = (int16_t) (I_ADC_1_CURRENT[0]);
	SAT_HEALTH.SOL_P2_C = (int16_t) (I_ADC_1_CURRENT[1]);
	SAT_HEALTH.SOL_P3_C = (int16_t) (I_ADC_1_CURRENT[2]);
	SAT_HEALTH.SOL_P4_C = (int16_t) (I_ADC_1_CURRENT[3]);
	SAT_HEALTH.SOL_P5_C = (int16_t) (I_ADC_2_CURRENT[6]);

	SAT_HEALTH.SOL_T_C = (int16_t) (EXT_ADC_1[8] * 1000);
	SAT_HEALTH.BAT_C = (int16_t) (EXT_ADC_1[9] * 1000);
	SAT_HEALTH._3V3_1_C = (int16_t) (I_ADC_2_CURRENT[0]);
	SAT_HEALTH._3V3_2_C = (int16_t) (I_ADC_2_CURRENT[1]);
	SAT_HEALTH._5V_C = (int16_t) (I_ADC_2_CURRENT[2]);
	SAT_HEALTH.UNREG1_C = (int16_t) (I_ADC_2_CURRENT[3]);
	SAT_HEALTH.UNREG2_C = (int16_t) (I_ADC_2_CURRENT[4]);
	SAT_HEALTH.RAW_C = (int16_t) (I_ADC_2_CURRENT[5]);

	BEACON_TYPE_B.SOL_P1_C = SAT_HEALTH.SOL_P1_C / 100;
	BEACON_TYPE_B.SOL_P2_C = SAT_HEALTH.SOL_P2_C / 100;
	BEACON_TYPE_B.SOL_P3_C = SAT_HEALTH.SOL_P3_C / 100;
	BEACON_TYPE_B.SOL_P4_C = SAT_HEALTH.SOL_P4_C / 100;
	BEACON_TYPE_B.SOL_P5_C = SAT_HEALTH.SOL_P5_C / 100;

	BEACON_TYPE_A.RAW_C = SAT_HEALTH.RAW_C / 100;
	BEACON_TYPE_A.BAT_C = SAT_HEALTH.BAT_C;
	BEACON_TYPE_A.SOL_TOT_C = SAT_HEALTH.SOL_T_C;

	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Current Sensor converted data.\r\n");
		myprintf("Reset_CS: %d mA\r\n", SAT_HEALTH.RST_3V3_C);
		myprintf("BATT_CS: %d mA\r\n", SAT_HEALTH.BAT_C);
		myprintf("SP_1 CS: %d mA\r\n", SAT_HEALTH.SOL_P1_C);
		myprintf("SP_2 CS: %d mA\r\n", SAT_HEALTH.SOL_P2_C);
		myprintf("SP_3 CS: %d mA\r\n", SAT_HEALTH.SOL_P3_C);
		myprintf("SP_4 CS: %d mA\r\n", SAT_HEALTH.SOL_P4_C);
		myprintf("SP_5 CS: %d mA\r\n", SAT_HEALTH.SOL_P5_C);
		myprintf("SP_T CS: %d mA\r\n", SAT_HEALTH.SOL_T_C);
		myprintf("3v3_1 CS: %d mA\r\n", SAT_HEALTH._3V3_1_C);
		myprintf("3v3_2 CS: %d mA\r\n", SAT_HEALTH._3V3_2_C);
		myprintf("5V CS: %d mA\r\n", SAT_HEALTH._5V_C);
		myprintf("Unreg_1 CS: %d mA\r\n", SAT_HEALTH.UNREG1_C);
		myprintf("Unreg_2 CS: %d mA\r\n", SAT_HEALTH.UNREG2_C);
		myprintf("RAW CS: %d mA\r\n", SAT_HEALTH.RAW_C);
#else
	 #endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	}

	//copying the current data to 2D array
	//SP1
	SAT_CURR[0][0] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[2]) >> 8 & 0xff);
	SAT_CURR[0][1] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[2]) & 0xff);
	//SP2
	SAT_CURR[1][0] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[3]) >> 8 & 0xff);
	SAT_CURR[1][1] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[3]) & 0xff);
	//SP3
	SAT_CURR[2][0] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[4]) >> 8 & 0xff);
	SAT_CURR[2][1] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[4]) & 0xff);
	//SP4
	SAT_CURR[3][0] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[5]) >> 8 & 0xff);
	SAT_CURR[3][1] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[5]) & 0xff);
	//sp5
	SAT_CURR[4][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[6]) >> 8 & 0xff);
	SAT_CURR[4][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[6]) & 0xff);
	//SP_Total
	SAT_CURR[5][0] = (uint8_t) (SAT_HEALTH.SOL_T_C >> 8 & 0xff);
	SAT_CURR[5][1] = (uint8_t) (SAT_HEALTH.SOL_T_C & 0xff);
	//copying other currents
	//BATT_CS
	SAT_CURR[6][0] = (uint8_t) (SAT_HEALTH.BAT_C >> 8 & 0xff);
	SAT_CURR[6][1] = (uint8_t) (SAT_HEALTH.BAT_C & 0xff);
	//RAW_CS
	SAT_CURR[7][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[5]) >> 8 & 0xff);
	SAT_CURR[7][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[5]) & 0xff);
	//RESET_3v3
	SAT_CURR[8][0] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[0]) >> 8 & 0xff);
	SAT_CURR[8][1] = (uint8_t) (((int16_t) I_ADC_1_CURRENT[0]) & 0xff);
	//3V3_1
	SAT_CURR[9][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[0]) >> 8 & 0xff);
	SAT_CURR[9][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[0]) & 0xff);
	//3V3_2
	SAT_CURR[10][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[1]) >> 8 & 0xff);
	SAT_CURR[10][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[1]) & 0xff);
	//5V
	SAT_CURR[11][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[2]) >> 8 & 0xff);
	SAT_CURR[11][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[2]) & 0xff);
	//UNREG_1
	SAT_CURR[12][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[3]) >> 8 & 0xff);
	SAT_CURR[12][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[3]) & 0xff);
	//UNREG_2
	SAT_CURR[13][0] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[4]) >> 8 & 0xff);
	SAT_CURR[13][1] = (uint8_t) (((int16_t) I_ADC_2_CURRENT[4]) & 0xff);
}

/*
 * @brief copy temp_data to 2D array
 *
 * @param none
 * @retval none
 *
 */
void MAKE_SAT_TEMPS() {
	SAT_HEALTH.BATT_TEMP = ((int16_t) EXT_ADC_0_TEMP[6]);
	SAT_HEALTH.Y1_TEMP = ((int16_t) EXT_ADC_0_TEMP[0]);
	SAT_HEALTH.Y_TEMP = ((int16_t) EXT_ADC_0_TEMP[3]);
	SAT_HEALTH.Z1_TEMP = ((int16_t) EXT_ADC_0_TEMP[1]);
	SAT_HEALTH.Z_TEMP = ((int16_t) EXT_ADC_0_TEMP[4]);
	SAT_HEALTH.X1_TEMP = ((int16_t) EXT_ADC_0_TEMP[2]);
	SAT_HEALTH.X_TEMP = ((int16_t) EXT_ADC_0_TEMP[7]);
	SAT_HEALTH.BPB_TEMP = ((int16_t) EXT_ADC_0_TEMP[5]);
	SAT_HEALTH.COM_TEMP = ((int16_t) EXT_ADC_1[10]);
//	SAT_HEALTH.OBC_TEMP = ;

	BEACON_TYPE_A.BAT_T = SAT_HEALTH.BATT_TEMP;
	BEACON_TYPE_A.Y1_T = SAT_HEALTH.Y1_TEMP / 100;
	BEACON_TYPE_A.Y_T = SAT_HEALTH.Y_TEMP / 100;
	BEACON_TYPE_A.X1_T = SAT_HEALTH.X1_TEMP / 100;
	BEACON_TYPE_A.X_T = SAT_HEALTH.X_TEMP / 100;
	BEACON_TYPE_A.Z1_T = SAT_HEALTH.Z1_TEMP / 100;
	BEACON_TYPE_A.Z_T = SAT_HEALTH.Z_TEMP / 100;
	BEACON_TYPE_A.BPB_T = SAT_HEALTH.BPB_TEMP / 100;
	BEACON_TYPE_A.OBC_T = SAT_HEALTH.OBC_TEMP / 100;

	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
		myprintf("Satellite Temperatures:::\r\n");
		myprintf("\tBattery Temperature: %i C.\r\n", SAT_HEALTH.BATT_TEMP);
		myprintf("\t-Y Panel Temperature: %i C.\r\n", SAT_HEALTH.Y1_TEMP);
		myprintf("\t+Y Panel Temperature: %i C.\r\n", SAT_HEALTH.Y_TEMP);
		myprintf("\t-Z Panel Temperature: %i C.\r\n", SAT_HEALTH.Z1_TEMP);
		myprintf("\t+Z Panel Temperature: %i C.\r\n", (SAT_HEALTH.Z_TEMP));
		myprintf("\t-X Panel Temperature: %i C.\r\n", (SAT_HEALTH.X1_TEMP));
		myprintf("\t+X Panel Temperature: %i C.\r\n", (SAT_HEALTH.X_TEMP));
		myprintf("\tBPB Temperature: %i C.\r\n", (SAT_HEALTH.BPB_TEMP));
		myprintf("\tCOM Transmitter Temperature: %i C.\r\n",
				(SAT_HEALTH.COM_TEMP));
		myprintf("\tOBC Chip Internal Temperature: %i C.\r\n",
				(SAT_HEALTH.OBC_TEMP));

#endif
#endif
	}

	// -y Plane
	SAT_TEMPS[0][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[0]) >> 8 & 0xff);
	SAT_TEMPS[0][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[0]) & 0xff);

	//+y plane
	SAT_TEMPS[1][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[3]) >> 8 & 0xff);
	SAT_TEMPS[1][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[3]) & 0xff);

	//-z plane
	SAT_TEMPS[2][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[1]) >> 8 & 0xff);
	SAT_TEMPS[2][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[1]) & 0xff);

	//+z plane
	SAT_TEMPS[3][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[4]) >> 8 & 0xff);
	SAT_TEMPS[3][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[4]) & 0xff);

	//-x plane
	SAT_TEMPS[4][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[2]) >> 8 & 0xff);
	SAT_TEMPS[4][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[2]) & 0xff);

	//+x plane [ant]
	SAT_TEMPS[5][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[7]) >> 8 & 0xff);
	SAT_TEMPS[5][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[7]) & 0xff);

	// BPB
	SAT_TEMPS[6][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[5]) >> 8 & 0xff);
	SAT_TEMPS[6][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[5]) & 0xff);

	//Batt Temp
	SAT_TEMPS[7][0] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[6]) >> 8 & 0xff);
	SAT_TEMPS[7][1] = (uint8_t) (((int16_t) EXT_ADC_0_TEMP[6]) & 0xff);

	//com temp
	SAT_TEMPS[8][0] = (uint8_t) (((uint16_t) EXT_ADC_1[10]) >> 8 & 0xff);
	SAT_TEMPS[8][1] = (uint8_t) (((uint16_t) EXT_ADC_1[10]) & 0xff);
}

/*
 * @brief copy satellite volt data to 2D array
 *
 * @param	none
 * @retval	none
 *
 */
void MAKE_SAT_VOLTS() {
	SAT_HEALTH.BATT_VOLT = (uint16_t) (EXT_ADC_1[7] * 1000);
	SAT_HEALTH.SOL_P1_V = (int16_t) (EXT_ADC_1[1] * 1000);
	SAT_HEALTH.SOL_P2_V = (int16_t) (EXT_ADC_1[0] * 1000);
	SAT_HEALTH.SOL_P3_V = (int16_t) (EXT_ADC_1[2] * 1000);
	SAT_HEALTH.SOL_P4_V = (int16_t) (EXT_ADC_1[3] * 1000);
	SAT_HEALTH.SOL_P5_V = (int16_t) (EXT_ADC_1[4] * 1000);
	SAT_HEALTH.SOL_T_V = (int16_t) (EXT_ADC_1[6] * 1000);

	BEACON_TYPE_B.SOL_P1_V = SAT_HEALTH.SOL_P1_V / 100;
	BEACON_TYPE_B.SOL_P2_V = SAT_HEALTH.SOL_P2_V / 100;
	BEACON_TYPE_B.SOL_P3_V = SAT_HEALTH.SOL_P3_V / 100;
	BEACON_TYPE_B.SOL_P4_V = SAT_HEALTH.SOL_P4_V / 100;
	BEACON_TYPE_B.SOL_P5_V = SAT_HEALTH.SOL_P5_V / 100;

	BEACON_TYPE_A.BAT_V = SAT_HEALTH.BATT_VOLT;
	BEACON_TYPE_A.SOL_TOT_V = SAT_HEALTH.SOL_T_V;

//Solar Panel 1
	SOLAR_STATUS.SOLAR_VOLT[0] = (int16_t) (EXT_ADC_1[1] * 1000);
	if (SOLAR_STATUS.SOLAR_VOLT[0] >= 4600) {
//		SAT_HEALTH.SOL_1_STAT = ACTIVE;
		BEACON_TYPE_A.SOL_P1_STAT = 1;
	} else {
//		SAT_HEALTH.SOL_1_STAT = INACTIVE;
		BEACON_TYPE_A.SOL_P1_STAT = 0;
	}
//Solar Panel 2
	SOLAR_STATUS.SOLAR_VOLT[1] = (int16_t) (EXT_ADC_1[0] * 1000);
	if (SOLAR_STATUS.SOLAR_VOLT[1] >= 4600) {
//		SAT_HEALTH.SOL_2_STAT = ACTIVE;
		BEACON_TYPE_A.SOL_P2_STAT = 1;
	} else {
//		SAT_HEALTH.SOL_2_STAT = INACTIVE;
		BEACON_TYPE_A.SOL_P2_STAT = 0;
	}
//Solar Panel 3
	SOLAR_STATUS.SOLAR_VOLT[2] = (int16_t) (EXT_ADC_1[2] * 1000);
	if (SOLAR_STATUS.SOLAR_VOLT[2] >= 4600) {
//		SAT_HEALTH.SOL_3_STAT = ACTIVE;
		BEACON_TYPE_A.SOL_P3_STAT = 1;
	} else {
//		SAT_HEALTH.SOL_3_STAT = INACTIVE;
		BEACON_TYPE_A.SOL_P3_STAT = 0;
	}
//Solar Panel 4
	SOLAR_STATUS.SOLAR_VOLT[3] = (int16_t) (EXT_ADC_1[3] * 1000);
	if (SOLAR_STATUS.SOLAR_VOLT[3] >= 4600) {
//		SAT_HEALTH.SOL_4_STAT = ACTIVE;
		BEACON_TYPE_A.SOL_P4_STAT = 1;
	} else {
//		SAT_HEALTH.SOL_4_STAT = INACTIVE;
		BEACON_TYPE_A.SOL_P4_STAT = 0;
	}
//Solar Panel 5
	SOLAR_STATUS.SOLAR_VOLT[4] = (int16_t) (EXT_ADC_1[4] * 1000);
	if (SOLAR_STATUS.SOLAR_VOLT[4] >= 4600) {
//		SAT_HEALTH.SOL_5_STAT = ACTIVE;
		BEACON_TYPE_A.SOL_P5_STAT = 1;
	} else {
//		SAT_HEALTH.SOL_5_STAT = INACTIVE;
		BEACON_TYPE_A.SOL_P5_STAT = 0;
	}

	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Voltage data.\r\n");
		HAL_Delay(1);
		myprintf("RAW_V: %d mV\r\n", (int16_t) (EXT_ADC_1[5] * 1000));
		myprintf("BATT_v: %d mV\r\n", (int16_t) (EXT_ADC_1[7] * 1000));
		myprintf("SP_1 v: %d mV\r\n", (int16_t) (EXT_ADC_1[1] * 1000));
		myprintf("SP_2 V: %d mV\r\n", (int16_t) (EXT_ADC_1[0] * 1000));
		myprintf("SP_3 V: %d mV\r\n", (int16_t) (EXT_ADC_1[2] * 1000));
		myprintf("SP_4 V: %d mV\r\n", (int16_t) (EXT_ADC_1[3] * 1000));
		myprintf("SP_5 V: %d mV\r\n", (int16_t) (EXT_ADC_1[4] * 1000));
		myprintf("SP_T V: %d mV\r\n", (int16_t) (EXT_ADC_1[6] * 1000));
#else
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	}

	//SP1
	SAT_VOLTS[0][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[1] * 1000) >> 8 & 0xff);
	SAT_VOLTS[0][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[1] * 1000) & 0xff);
	//SP2
	SAT_VOLTS[1][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[0] * 1000) >> 8 & 0xff);
	SAT_VOLTS[1][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[0] * 1000) & 0xff);

	//SP3
	SAT_VOLTS[2][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[2] * 1000) >> 8 & 0xff);
	SAT_VOLTS[2][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[2] * 1000) & 0xff);

	//SP4
	SAT_VOLTS[3][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[3] * 1000) >> 8 & 0xff);
	SAT_VOLTS[3][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[3] * 1000) & 0xff);

	//SP5
	SAT_VOLTS[4][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[4] * 1000) >> 8 & 0xff);
	SAT_VOLTS[4][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[4] * 1000) & 0xff);

	//SP_Total
	SAT_VOLTS[5][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[6] * 1000) >> 8 & 0xff);
	SAT_VOLTS[5][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[6] * 1000) & 0xff);

	//RAW
	SAT_VOLTS[6][0] = (uint8_t) ((uint16_t) (EXT_ADC_1[5] * 1000) >> 8 & 0xff);
	SAT_VOLTS[6][1] = (uint8_t) ((uint16_t) (EXT_ADC_1[5] * 1000) & 0xff);

	//BATT
	SAT_VOLTS[7][0] =
			(uint8_t) (((uint16_t) (EXT_ADC_1[7] * 1000)) >> 8 & 0xff);
	SAT_VOLTS[7][1] = (uint8_t) (((uint16_t) (EXT_ADC_1[7] * 1000)) & 0xff);

	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Make Sat Volts [BATT]: %02x %02x\r\n", SAT_VOLTS[7][0],
				SAT_VOLTS[7][1]);
#else
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
	}
}

/*
 * @brief packs all the data used for housekeeping into a buffer to download
 * 			also revceives IMU data from PACDS to housekeep
 *
 * @param
 * @retval
 *
 */
void Make_HK_Data() {
	DATA_CONVERSION(); // convert Iadc converted data to current data
	MAKE_SAT_TEMPS();
	MAKE_SAT_VOLTS();
	GET_PACDS_GYRO_DATA();
	CHECK_POWER_STATE(SAT_HEALTH.BATT_VOLT);
	uint8_t count = 0;
	memset(HK_Data_Buff, '\0', HK_DATA_LEN);
	HK_Data_Buff[count] = HEADER;
	count++;
	HK_Data_Buff[count] = TIM_DAY;
	count++;
	HK_Data_Buff[count] = TIM_HOUR;
	count++;
	HK_Data_Buff[count] = TIM_MIN;
	count++;
	HK_Data_Buff[count] = TIM_SEC;
	count++;

//copy current data to buffer
	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_CURR[i][j];
			count++;
		}
	}

//copy voltage data to buffer
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_VOLTS[i][j];
			count++;
		}
	}

//copy temperature data to buffer
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_TEMPS[i][j];
			count++;
		}
	}

//copy IMU data to buffer
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_IMU[i][j];
			count++;
		}
	}
	HK_Data_Buff[count] = FOOTER;
	count++;

#ifdef DEBUG_MODE
	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {

#ifdef TRACE_MODE
#else
		myprintf("HK_DATA_READY\r\n");
		for (int i = 0; i < count; i++) {
			myprintf("%02x ", HK_Data_Buff[i]);
		}
		myprintf("\r\n");
#endif //ifndef TRACE_MODE
	}
#endif //ifdef DEBUG_MODE

	MAIN_FM_MODE();
	Check_HK_Data_Address();
	uint32_t temp = HK_DATA_CURRENT_ADDRESS;
	for (int i = 0; i < HK_DATA_LEN; i++) {
		Byte_Write_4B(&MAIN_FM, temp + i, HK_Data_Buff[i]);
	}
	HK_DATA_CURRENT_ADDRESS += HK_DATA_LEN;
	Write_Address_Data();

#ifdef DEBUG_MODE
	if (HK_FLAG && !MSN_STATUS && !COM_DATA_TX_FLAG) {

#ifndef TRACE_MODE
		myprintf("HK_Data_Stored_On_Flash.\r\n");
#else
		#endif //ifndef TRACE_MODE
	}
#endif //ifdef DEBUG_MODE
}

/*
 * @brief configuration of the Write Protect, Hold and Reset pins of Flash Memory
 *
 * @param	none
 * @retval	none
 *
 */

void Configure_Flash_Pins(void) {

//Setting the hold, WP pins high
	HAL_GPIO_WritePin(MAIN_FM_CS_GPIO_Port, MAIN_FM_CS_Pin, SET);
	HAL_GPIO_WritePin(MAIN_FM_HOLD_GPIO_Port, MAIN_FM_HOLD_Pin, SET);
	HAL_GPIO_WritePin(MAIN_FM_RESET_GPIO_Port, MAIN_FM_RESET_Pin, SET);
	HAL_GPIO_WritePin(MAIN_FM_WP_GPIO_Port, MAIN_FM_WP_Pin, SET);

	HAL_GPIO_WritePin(SMSN_FM_CS_GPIO_Port, SMSN_FM_CS_Pin, SET);
	HAL_GPIO_WritePin(SMSN_FM_HOLD_GPIO_Port, SMSN_FM_HOLD_Pin, SET);
	HAL_GPIO_WritePin(SMSN_FM_RST_GPIO_Port, SMSN_FM_RST_Pin, SET);
	HAL_GPIO_WritePin(SMSN_FM_WP_GPIO_Port, SMSN_FM_WP_Pin, SET);

}

/*
 * @brief Switching the MUX line to allow access of Shared MSN FM to Main STM
 *
 * @param	none
 * @retval	none
 *
 */

void MAIN_FM_MODE() {
//Activating MUX
	HAL_GPIO_WritePin(SFM_MUX_EN_GPIO_Port, SFM_MUX_EN_Pin, RESET);
//SMSN_FM access to OBC
	HAL_GPIO_WritePin(SMSN_FM_MODE_GPIO_Port, SMSN_FM_MODE_Pin, RESET);
}

/*
 * @brief Switching the MUX line to allow access of shared MSN FM to active MSN
 *
 * @param	none
 * @retval	none
 *
 */
void MSN_FM_MODE() {
//Activating MUX
	HAL_GPIO_WritePin(SFM_MUX_EN_GPIO_Port, SFM_MUX_EN_Pin, RESET);
//  SMSN_FM access to MSN(s)
	HAL_GPIO_WritePin(SMSN_FM_MODE_GPIO_Port, SMSN_FM_MODE_Pin, SET);
}
/*
 * @brief Disabling the MUX, results in Shared FM access being blocked
 *
 * @param	none
 * @retval	none
 *
 */

void MUX_DISABLE() {
	HAL_GPIO_WritePin(SFM_MUX_EN_GPIO_Port, SFM_MUX_EN_Pin, SET);
}

/*
 * @brief debug operation only
 *
 * @param	none
 * @retval	none
 *
 */

void SATELLITE_OPERATION_PC() {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("\r\nData Received from PC.\r\n");
	myprintf("Time:\r\n hr:min:sec:msec\r\n");
	myprintf("\r\n%2d:%2d:%2d:%3u::\r\n", TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#endif
#endif
	if (PC_DATA[0] == SAT_ID) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Command Received From PC for the Satellite: \r\n 0x");
		for (int i = 0; i < 13; i++) {
			myprintf("%x", PC_DATA[i]);
		}
		myprintf("\r\n");
#endif
#endif
		Save_SAT_Log(PC_DATA[1], PC_DATA[2], PC_DATA[3], PC_DATA[4]); //to be used for COM command but for testing use PC command to save the satellite log

		if (PC_DATA[6] != 0) {		//if it is a Reservation Command
			Reservation_Command(PC_DATA[1], PC_DATA[2], PC_DATA[3], PC_DATA[4],
					PC_DATA[6]);
		} else if (PC_DATA[6] == 0) {
			Execute_CMD(PC_DATA[1], PC_DATA[2], PC_DATA[3], PC_DATA[4],
					PC_DATA[7], PC_DATA[8], PC_DATA[9], PC_DATA[10],
					PC_DATA[11], PC_DATA[12]);
		}
	}

	/*
	 * Debug commands for Operation
	 */
	else if (PC_DATA[0] == HEADER && PC_DATA[4] == FOOTER) {
		OTHER_OPERATIONS(PC_DATA);
	}

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Debug Command: ");
	for (int i = 0; i < CMD_LEN; i++) {
		myprintf("%02x ", PC_DATA[i]);
	}
	myprintf("\r\n");
#endif
#endif

	memset(PC_DATA, '\0', DATA_BUFFER_LEN);
	PC_Data_Len = 0;
	PC_CMD = 0;
	DEBUG_DATA_RX_FLAG = 0;

}

/*
 * @brief operate satellite with CMD from GS
 *
 * @param	none
 * @retval	none
 *
 */
void SATELLITE_OPERATION_GS() {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("\r\nData Received from GS.\r\n");
	myprintf("Time:hr:min:sec:msec::");
	myprintf("%2d:%2d:%2d:%3u::\r\n", TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#endif
#endif
	if (GS_CMD[0] == SAT_ID) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Command Received From GS for the Satellite: \r\n 0x");
		HAL_Delay(1);
		for (int i = 0; i < 13; i++) {
			myprintf("%x ", GS_CMD[i]);
		}
		myprintf("\r\n");
		HAL_Delay(1);
#endif
#endif
		Save_SAT_Log(GS_CMD[1], GS_CMD[2], GS_CMD[3], GS_CMD[4]); //to be used for COM command but for testing use PC command to save the satellite log

		if (GS_CMD[6] != 0) {		//if it is a Reservation Command
			Reservation_Command(GS_CMD[1], GS_CMD[2], GS_CMD[3], GS_CMD[4],
					GS_CMD[6]);
		} else if (GS_CMD[6] == 0) {
			Execute_CMD(GS_CMD[1], GS_CMD[2], GS_CMD[3], GS_CMD[4], GS_CMD[7],
					GS_CMD[8], GS_CMD[9], GS_CMD[10], GS_CMD[11], GS_CMD[12]);
		}
	}
	/*
	 * other debug operations commands
	 */
	else if (GS_CMD[0] == HEADER && GS_CMD[4] == FOOTER) {
		OTHER_OPERATIONS(GS_CMD);
	}

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Debug operation commands.");
	for (int i = 0; i < CMD_LEN; i++) {
		myprintf("%02x ", GS_CMD[i]);
	}
	myprintf("\r\n");
#endif
#endif
	memset(GS_CMD, '\0', DATA_BUFFER_LEN);

}

/*
 * @brief packs the critical sat health data into beacon packaet and transmits
 *
 * @param	none
 * @retval	none
 *
 */
void Make_BEACON_Data(void) {
	BEACON_TYPE_A.HEAD = HEADER;
	BEACON_TYPE_A.TYPE = 0b0001;
	BEACON_TYPE_A.TIM_DAY = TIM_DAY & 0xFFFFFFF;
	BEACON_TYPE_A.TIM_HOUR = TIM_HOUR;
	BEACON_TYPE_A.MSN1_STAT = SAT_HEALTH.MSN1_STAT;
	BEACON_TYPE_A.MSN2_STAT = SAT_HEALTH.MSN2_STAT;
	BEACON_TYPE_A.MSN3_STAT = SAT_HEALTH.MSN3_STAT;
	BEACON_TYPE_A.OPER_MODE = SAT_HEALTH.OPER_MODE;

	BEACON_TYPE_A.ANT_STAT =
			(SAT_HEALTH.ANT_DEP_STAT == DEPLOYED) ? 0b010 : 0b000;
	BEACON_TYPE_A.UL_STAT = (SAT_HEALTH.UL_STAT == UL_RX) ? 0b010 : 0b000;

	BEACON_TYPE_A.KILL_1_STAT = HAL_GPIO_ReadPin(KILL_SWITCH1_STAT_GPIO_Port,
	KILL_SWITCH1_STAT_Pin);
	BEACON_TYPE_A.KILL_2_STAT = HAL_GPIO_ReadPin(KILL_SWITCH2_STAT_GPIO_Port,
	KILL_SWITCH2_STAT_Pin);

	BEACON_TYPE_A.OBC_RESET_COUNT = SAT_HEALTH.OBC_ABNL_RST;
	BEACON_TYPE_A.RESET_RESET_COUNT = SAT_HEALTH.RST_ABNL_RST;

	BEACON_TYPE_A.LAST_RESET = SAT_HEALTH.RST_TIME;
	BEACON_TYPE_A.CHK_CRC = MAKE_CRC(&BEACON_TYPE_A.HEAD, 30);

	BEACON_TYPE_B.HEAD = HEADER;
	BEACON_TYPE_B.TYPE = 0b0010;
	BEACON_TYPE_B.TIM_DAY = TIM_DAY & 0xFFFFFFF;
	BEACON_TYPE_B.CHK_CRC = MAKE_CRC(&BEACON_TYPE_B.HEAD, 30);
}

/*
 * @brief 	Updates the strucutre used for collecting Data to be sent to reset
 *
 * @param	 RESET_Tx	Pointer to the structure containing the data for reset
 * @retval	none
 *
 */
void UPDATE_RST_DATA(RSTTx_DATASET_TypeDef *RESET_Tx, RST_CMD TYPE) {

	if ((TYPE != OPER_MODE_1) && (TYPE != OPER_MODE_2) && (TYPE != OPER_MODE_3)
			&& (TYPE != OPER_MODE_4) && (TYPE != RST_SAT)
			&& (BEACON_FLAG != 1)) {
		ADC1_DMA_CONVERT();
		ADC2_DMA_CONVERT();
		GET_EXT_ADC_DATA();
		DATA_CONVERSION();
		MAKE_SAT_TEMPS();
		MAKE_SAT_VOLTS();
		CHECK_POWER_STATE(SAT_HEALTH.BATT_VOLT);

	}

	RESET_Tx->HEAD = HEADER;
	RESET_Tx->TIM_DAYS = TIM_DAY;
	RESET_Tx->TIM_HOUR = TIM_HOUR;
	RESET_Tx->TIM_SEC = TIM_SEC;
	RESET_Tx->CMD = TYPE;
	if (ANT_DEPLOY_STATUS) {
		RESET_Tx->ANT_DEP_STAT = 0xDE;
	} else {
		RESET_Tx->ANT_DEP_STAT = 0x00;
	}

	RESET_Tx->UL_STAT = DCOMMAND_RX_SUCCESS;
	RESET_Tx->OPER_MODE = OPER_MODES;
	RESET_Tx->MSN_FLAG = MSN_STATUS;
	RESET_Tx->RSV_FLAG = SAT_HEALTH.RSV_FLAG; //TODO: Verification necessary
	RESET_Tx->MSN3_STAT = MSN3_FLAG;
	RESET_Tx->MSN2_STAT = MSN2_FLAG;
	RESET_Tx->MSN1_STAT = MSN1_FLAG;
	RESET_Tx->BATT_TEMP = SAT_HEALTH.BATT_TEMP / 10;
	RESET_Tx->BATT_VOLT = SAT_HEALTH.BATT_VOLT / 100;
	RESET_Tx->SOL_1_STAT = BEACON_TYPE_A.SOL_P1_STAT ? ACTIVE : INACTIVE;
	RESET_Tx->SOL_2_STAT = BEACON_TYPE_A.SOL_P2_STAT ? ACTIVE : INACTIVE;
	RESET_Tx->SOL_3_STAT = BEACON_TYPE_A.SOL_P3_STAT ? ACTIVE : INACTIVE;
	RESET_Tx->SOL_4_STAT = BEACON_TYPE_A.SOL_P4_STAT ? ACTIVE : INACTIVE;
	RESET_Tx->SOL_5_STAT = BEACON_TYPE_A.SOL_P5_STAT ? ACTIVE : INACTIVE;
	RESET_Tx->RSV_CMD = SAT_HEALTH.RSV_CMD;
	RESET_Tx->CHK_SUM = 0;
	for (int i = 0; i < RESET_DATA_TX_LEN - 3; i++) {
		RESET_Tx->CHK_SUM += *(&RESET_Tx->HEAD + i);
	}
	RESET_Tx->FOOT = FOOTER;
}

/*
 * @brief Checks the power level, power modes and updates accordingly
 *
 * @param	MODE		variable of enumeration type OPERA_MODES, containing
 * 						definition of different Operation Modes used in MUNAL.
 * @retval  uint8_t 	integer to indicate the power mode update status
 * 						1: successfully updated power mode
 * 						0: power mode update failure.
 *
 * @note	To change the power mode, first communication with reset is accomplished
 * 			power mode update command is transmitted to RESET MCU, and ACK for the
 * 			sent command is awaited. After successful ACK is received for RESET, only
 * 			then OBC will update all the flags, operation mode data and store them to
 * 			internal flash memory, as the flags are critical for operation in case OBC
 * 			encounters reset before power mode change.
 */
uint8_t UPDATE_OPERATION_MODE(OPERA_MODES MODE) {
	switch (MODE) {
	case NRML_MODE:
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("\t\tShifting Operation MODE to Normal MODE.\r\n");
#else
#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
//
//		OPER_MODES = NRML_MODE;
//		OPERA_FLAGS.MSN1_EN_FLAG = ON;
//		OPERA_FLAGS.MSN2_EN_FLAG = ON;
//		OPERA_FLAGS.MSN3_EN_FLAG = ON;
//		OPERA_FLAGS.RSV_MSN_EXE_FLAG = ON;
//		OPERA_FLAGS.TEL_TX_FLAG = ON;
//		OPERA_FLAGS.LOG_EN_FLAG = ON;
//		OPERA_FLAGS.FM_EN_FLAG = ON;
//		OPERA_FLAGS.COM_EN_FLAG = ON;
//		SAT_HEALTH.OPER_MODE = NRML_MODE;

		RESET_Tx.CMD = OPER_MODE_1;
		break;
	case LOW_PWR_MODE:
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("\t\tShifting Operation Mode to Low Power Mode.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

//		OPER_MODES = LOW_PWR_MODE;
//		OPERA_FLAGS.MSN1_EN_FLAG = OFF;
//		OPERA_FLAGS.MSN2_EN_FLAG = ON;
//		OPERA_FLAGS.MSN3_EN_FLAG = OFF;
//		OPERA_FLAGS.RSV_MSN_EXE_FLAG = OFF;
//		OPERA_FLAGS.TEL_TX_FLAG = ON;
//		OPERA_FLAGS.LOG_EN_FLAG = ON;
//		OPERA_FLAGS.FM_EN_FLAG = ON;
//		OPERA_FLAGS.COM_EN_FLAG = ON;
//		SAT_HEALTH.OPER_MODE = LOW_PWR_MODE;

		RESET_Tx.CMD = OPER_MODE_2;
		break;
	default:
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("\t\tMode Error!!");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		break;
	}

	RESET_Tx.HEAD = HEADER;
	RESET_Tx.TIM_DAYS = TIM_DAY;
	RESET_Tx.TIM_HOUR = TIM_HOUR;
	RESET_Tx.TIM_SEC = TIM_SEC;
	RESET_Tx.UL_STAT = DCOMMAND_RX_SUCCESS;
	RESET_Tx.ANT_DEP_STAT = SAT_HEALTH.ANT_DEP_STAT;
	RESET_Tx.OPER_MODE = OPER_MODES;
	RESET_Tx.MSN_FLAG = MSN_STATUS;
	RESET_Tx.RSV_FLAG = SAT_HEALTH.RSV_FLAG; //TODO: Verification necessary
	RESET_Tx.MSN3_STAT = MSN3_FLAG;
	RESET_Tx.MSN2_STAT = MSN2_FLAG;
	RESET_Tx.MSN1_STAT = MSN1_FLAG;
	RESET_Tx.BATT_TEMP = SAT_HEALTH.BATT_TEMP / 10;
	RESET_Tx.BATT_VOLT = SAT_HEALTH.BATT_VOLT / 100;
	RESET_Tx.SOL_1_STAT = BEACON_TYPE_A.SOL_P1_STAT ? ACTIVE : INACTIVE;
	RESET_Tx.SOL_2_STAT = BEACON_TYPE_A.SOL_P2_STAT ? ACTIVE : INACTIVE;
	RESET_Tx.SOL_3_STAT = BEACON_TYPE_A.SOL_P3_STAT ? ACTIVE : INACTIVE;
	RESET_Tx.SOL_4_STAT = BEACON_TYPE_A.SOL_P4_STAT ? ACTIVE : INACTIVE;
	RESET_Tx.SOL_5_STAT = BEACON_TYPE_A.SOL_P5_STAT ? ACTIVE : INACTIVE;
	RESET_Tx.RSV_CMD = SAT_HEALTH.RSV_CMD;
	RESET_Tx.CHK_SUM = 0;
	for (int i = 0; i < RESET_DATA_TX_LEN - 3; i++) {
		RESET_Tx.CHK_SUM += *(&RESET_Tx.HEAD + i);
	}
	RESET_Tx.FOOT = FOOTER;

	RESET_DATA_TX_FLAG = 0;
	RESET_DATA_RX_FLAG = 0;

	HAL_UART_Transmit_IT(&RESET_STREAM, (uint8_t*) &RESET_Tx,
	RESET_DATA_TX_LEN);
	HAL_UART_Transmit_IT(&RESET_ALT_STREAM, (uint8_t*) &RESET_Tx,
	RESET_DATA_TX_LEN);

	uint32_t currenttick = HAL_GetTick();

	while (RESET_DATA_TX_FLAG == 0) {
		if (HAL_GetTick() - currenttick > 5000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("\t\t\tPower Mode Command TX Timeout\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			break;
		}
	}
	if (RESET_DATA_TX_FLAG) {
		RESET_DATA_TX_FLAG = 0;
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf(
				"\t\t\tPower Mode Command Sent to Reset.\r\n\t\t\tWaiting for ACK.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	} else {
		return 0;
	}
	currenttick = HAL_GetTick();
	while (RESET_DATA_RX_FLAG == 0) {
		if (HAL_GetTick() - currenttick > 3000) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("\t\t\tReset ACK Rx Timeout.\r\\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			break;
		}
	}
	if (RESET_DATA_RX_FLAG) {
		RESET_DATA_RX_FLAG = 0;
//		memcpy(&RESET_Tx, RESET_Data_Buff, RESET_DATA_RX_LEN);
		for (int i = 0; i < RESET_DATA_RX_LEN; i++) {
			*(&RESET_Rx.HEAD + i) = RESET_Data_Buff[i];
		}
		if (Check_Reset_Data()) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("\t\t\tPower Mode Update ACK Received from RESET.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("\t\t\tPower Mode Update Command Wrong ACK.\r\n");
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			return 0;
		}
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("\t\t\t Power Mode Update Command ACK Rx failure.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		return 0;
	}
	return 1;
}

/*
 * @brief 		Checks the latest battery voltage and updates the operation mode as
 * 				necessary for power mode calibration
 *
 * @param		BATT_VOLT	battery voltage in mV scale read from ADC
 * @retval		none
 *
 * @note		this function takes the battery voltage as input, compares it the
 * 				voltage mode scale, to update operational mode in needed. If the
 * 				operation mode is in SAT_KILL_MODE, the mode change is ignored. Only
 * 				after the SAT_UNKILL command comes, and satellite shifts to NRML_MODE,
 * 				after which this function is able to switch the operation Modes.
 */
void CHECK_POWER_STATE(uint16_t BATT_VOLT) {
#ifdef DEBUG_MODE
	if (!BEACON_FLAG) {
#ifndef TRACE_MODE
		myprintf("Checking the Power status of satellite.\r\n");
#else
	#endif //ifndef TRACE_MODE
	}
#endif //ifdef DEBUG_MODE

	if (BATT_VOLT > 3500) {
#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifndef TRACE_MODE
			myprintf("Battery in Normal Power Range.\r\n");
#else
			#endif //ifndef TRACE_MODE
		}
#endif //ifdef DEBUG_MODE
		if (OPER_MODES != NRML_MODE && OPER_MODES != SAT_KILL_MODE) {
			if (UPDATE_OPERATION_MODE(NRML_MODE)) {
#ifdef DEBUG_MODE
				if (!BEACON_FLAG) {
#ifndef TRACE_MODE
					myprintf(
							"\t\t\tPower Mode Shifted to Normal Power Mode.\r\n");
#else
						#endif //ifndef TRACE_MODE
				}
#endif //ifdef DEBUG_MODE

				OPER_MODES = NRML_MODE;
				OPERA_FLAGS.MSN1_EN_FLAG = ON;
				OPERA_FLAGS.MSN2_EN_FLAG = ON;
				OPERA_FLAGS.MSN3_EN_FLAG = ON;
				OPERA_FLAGS.RSV_MSN_EXE_FLAG = ON;
				OPERA_FLAGS.TEL_TX_FLAG = ON;
				OPERA_FLAGS.LOG_EN_FLAG = ON;
				OPERA_FLAGS.FM_EN_FLAG = ON;
				OPERA_FLAGS.COM_EN_FLAG = ON;
				SAT_HEALTH.OPER_MODE = NRML_MODE;

			} else {
#ifdef DEBUG_MODE
				if (!BEACON_FLAG) {
#ifndef TRACE_MODE
					myprintf("\t\t\tPower Mode shift error.\r\n");
#else
				#endif //ifndef TRACE_MODE
				}
#endif //ifdef DEBUG_MODE
			}
		} else if (OPER_MODES == NRML_MODE) {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifndef TRACE_MODE
				myprintf(
						"Satellite in Normal Operation Mode!! No Mode shift necessary.\r\n");
#else
			#endif //ifndef TRACE_MODE
			}
#endif //ifdef DEBUG_MODE
		} else if (OPER_MODES == SAT_KILL_MODE) {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifndef TRACE_MODE
				myprintf(
						"Satellite in Kill Mode!! No Mode Shifting necessary.\r\n");
#else
			#endif //ifndef TRACE_MODE
			}
#endif //ifdef DEBUG_MODE
		}

//	} else if (BATT_VOLT > 3100 && BATT_VOLT < 3500) {
	} else if (BATT_VOLT < 3500) {

#ifdef DEBUG_MODE
		if (!BEACON_FLAG) {
#ifndef TRACE_MODE
			myprintf("Battery in Low Power Range.\r\n");
#else
		#endif //ifndef TRACE_MODE
		}
#endif //ifdef DEBUG_MODE

		if (OPER_MODES != LOW_PWR_MODE && OPER_MODES != SAT_KILL_MODE) {
			if (UPDATE_OPERATION_MODE(LOW_PWR_MODE)) {
#ifdef DEBUG_MODE
				if (!BEACON_FLAG) {
#ifndef TRACE_MODE
					myprintf("\t\t\tPower Mode Shifted to Low Power Mode.\r\n");
#else
							#endif //ifndef TRACE_MODE
				}
#endif //ifdef DEBUG_MODE

				OPER_MODES = LOW_PWR_MODE;
				OPERA_FLAGS.MSN1_EN_FLAG = OFF;
				OPERA_FLAGS.MSN2_EN_FLAG = ON;
				OPERA_FLAGS.MSN3_EN_FLAG = OFF;
				OPERA_FLAGS.RSV_MSN_EXE_FLAG = OFF;
				OPERA_FLAGS.TEL_TX_FLAG = ON;
				OPERA_FLAGS.LOG_EN_FLAG = ON;
				OPERA_FLAGS.FM_EN_FLAG = ON;
				OPERA_FLAGS.COM_EN_FLAG = ON;
				SAT_HEALTH.OPER_MODE = LOW_PWR_MODE;

			} else {
#ifdef DEBUG_MODE
				if (!BEACON_FLAG) {
#ifndef TRACE_MODE
					myprintf("\t\t\tPower Mode shift error.\r\n");
#else
					#endif //ifndef TRACE_MODE
				}
#endif //ifdef DEBUG_MODE
			}
		} else if (OPER_MODES == LOW_PWR_MODE) {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifndef TRACE_MODE
				myprintf(
						"Satellite in LOW Power Operation Mode!! No Mode shift necessary.\r\n");
#else
				#endif //ifndef TRACE_MODE
			}
#endif //ifdef DEBUG_MODE
		} else if (OPER_MODES == SAT_KILL_MODE) {
#ifdef DEBUG_MODE
			if (!BEACON_FLAG) {
#ifndef TRACE_MODE
				myprintf(
						"Satellite in Kill Mode!! No Mode Shifting necessary.\r\n");
#else
				#endif //ifndef TRACE_MODE
			}
#endif //ifdef DEBUG_MODE
		}

	}

	// safe mode disabled
	/*	else if (BATT_VOLT < 3100) {

	 #ifdef DEBUG_MODE
	 if (!BEACON_FLAG) {
	 #ifndef TRACE_MODE
	 myprintf("Battery in Safe Power Range\r\n");
	 #else
	 #endif //ifndef TRACE_MODE
	 }
	 #endif //ifdef DEBUG_MODE

	 if (OPER_MODES != SAFE_MODE && OPER_MODES != SAT_KILL_MODE) {
	 if (UPDATE_OPERATION_MODE(SAFE_MODE)) {
	 #ifdef DEBUG_MODE
	 if (!BEACON_FLAG) {
	 #ifndef TRACE_MODE
	 myprintf(
	 "\t\t\tPower Mode Shifted to Safe Power Mode.\r\n");
	 #else
	 #endif //ifndef TRACE_MODE
	 }
	 #endif //ifdef DEBUG_MODE
	 } else {
	 #ifdef DEBUG_MODE
	 {
	 #ifndef TRACE_MODE
	 myprintf("\t\t\tPower Mode shift error.\r\n");
	 #else
	 #endif //ifndef TRACE_MODE
	 }
	 #endif //ifdef DEBUG_MODE
	 }
	 } else if (OPER_MODES == SAFE_MODE) {
	 #ifdef DEBUG_MODE
	 if (!BEACON_FLAG) {
	 #ifndef TRACE_MODE
	 myprintf(
	 "Satellite in SAFE Operation Mode!! No Mode shift necessary.\r\n");
	 #else
	 #endif //ifndef TRACE_MODE
	 }
	 #endif //ifdef DEBUG_MODE
	 } else if (OPER_MODES == SAT_KILL_MODE) {
	 #ifdef DEBUG_MODE
	 if (!BEACON_FLAG) {
	 #ifndef TRACE_MODE
	 myprintf(
	 "Satellite in Kill Mode!! No Mode Shifting necessary.\r\n");
	 #else
	 #endif //ifndef TRACE_MODE
	 }
	 #endif //ifdef DEBUG_MODE
	 }
	 }*/

}

/*
 * @brief	runs adc in the middle of other operation to collect current sensor and voltage data
 * 			and stores the read data to flash memory as HK data
 *
 * @param	none
 * @retval 	none
 */
void RUN_ADC() {
	HK_FLAG = 0;
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf(
			"%04i:%02i:%02i:%02i:%03i Starting ADC Data Collection mid operation.\r\n",
			TIM_DAY, TIM_HOUR, TIM_MIN, TIM_SEC, mSEC);
#endif	//TRACE_MODE
#endif	// DEBUG_MODE
	ADC1_DMA_CONVERT();
	uint8_t count = 0;
	memset(HK_Data_Buff, '\0', HK_DATA_LEN);
	HK_Data_Buff[count] = HEADER;
	count++;
	HK_Data_Buff[count] = TIM_DAY;
	count++;
	HK_Data_Buff[count] = TIM_HOUR;
	count++;
	HK_Data_Buff[count] = TIM_MIN;
	count++;
	HK_Data_Buff[count] = TIM_SEC;
	count++;
	ADC2_DMA_CONVERT();
	GET_EXT_ADC_DATA();
	DATA_CONVERSION();
	MAKE_SAT_TEMPS();
	MAKE_SAT_VOLTS();
	//copy current data to buffer
	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_CURR[i][j];
			count++;
		}
	}
	//copy voltage data to buffer
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_VOLTS[i][j];
			count++;
		}
	}
	//copy temperature data to buffer
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 2; j++) {
			HK_Data_Buff[count] = SAT_TEMPS[i][j];
			count++;
		}
	}
	count += 12;
	HK_Data_Buff[count] = FOOTER;
	count++;
//	MAIN_FM_MODE();
	Check_HK_Data_Address();
	uint32_t temp = HK_DATA_CURRENT_ADDRESS;
	for (int i = 0; i < HK_DATA_LEN; i++) {
		Byte_Write_4B(&MAIN_FM, temp + i, HK_Data_Buff[i]);
	}
	HK_DATA_CURRENT_ADDRESS += HK_DATA_LEN;
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("%04i:%02i:%02i:%02i:%03i ADC Data coll, store.\r\n", TIM_DAY,
			TIM_HOUR, TIM_MIN, TIM_SEC);
	myprintf("UNREG_1 C: %04i.\r\n", SAT_HEALTH.UNREG1_C);
	myprintf("UNREG_2 C: %04i.\r\n", SAT_HEALTH.UNREG2_C);
	myprintf("3v3_2 C: %04i.\r\n", SAT_HEALTH._3V3_2_C);
	myprintf("5V C: %04i.\r\n", SAT_HEALTH._5V_C);

#endif	//TRACE_MODE
#endif	// DEBUG_MODE
}
