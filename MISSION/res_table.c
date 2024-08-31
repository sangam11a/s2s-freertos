/*
 * res_table.c
 *
 *  Created on: Jan 7, 2023
 *      Author: asus
 */
//#include "../../MISSION/res_table.h"
#include "res_table.h"

/*
 * to count the no of Reservation Data that are there
 */
uint8_t Get_RSV_Number() {
	int count = 0;
	for (int i = 0; i < RSV_TABLE_SIZE; i += 5) {
		if (RSV_TABLE[i] != 0xff || RSV_TABLE[i + 1] != 0xff) {
			count++;
		}
	}
	return count;
}

/*
 * @brief	to sort the table in the ascending order of time
 * 			in the last index i.e. 5th position, 4th index, the data of the time is stored of every data
 * 			there is no gap(buffer) given in between the two reservation commands
 */
void sort_table() {
	uint8_t t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
	int i, j;
	for (i = 4; i < RSV_TABLE_SIZE; i = i + 5) {
		for (j = i + 5; j < RSV_TABLE_SIZE; j = j + 5) {
			if (RSV_TABLE[i] > RSV_TABLE[j]) {
				t1 = RSV_TABLE[j - 4];
				t2 = RSV_TABLE[j - 3];
				t3 = RSV_TABLE[j - 2];
				t4 = RSV_TABLE[j - 1];
				t5 = RSV_TABLE[j];
				HAL_Delay(10);

				RSV_TABLE[j - 4] = RSV_TABLE[i - 4];
				RSV_TABLE[j - 3] = RSV_TABLE[i - 3];
				RSV_TABLE[j - 2] = RSV_TABLE[i - 2];
				RSV_TABLE[j - 1] = RSV_TABLE[i - 1];
				RSV_TABLE[j] = RSV_TABLE[i];
				HAL_Delay(10);

				RSV_TABLE[i - 4] = t1;
				RSV_TABLE[i - 3] = t2;
				RSV_TABLE[i - 2] = t3;
				RSV_TABLE[i - 1] = t4;
				RSV_TABLE[i] = t5;
				HAL_Delay(10);

				t1 = 0;
				t2 = 0;
				t3 = 0;
				t4 = 0;
				t5 = 0;
			}
		}
	}
	return;
}

/*
 * @brief		to get the reservation table data stored in the Flash Memory
 *
 */
void Get_RSV_From_Flash() {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Retrieving Reservation Commands From Flash Memory\r\n");
	HAL_Delay(1);
#endif
#endif
	Bulk_Read_4B(&MAIN_FM, RSV_DATA_CURRENT_ADDRESS, RSV_TABLE,
	RSV_TABLE_SIZE);

	RSV_MIN = Read_Byte_4B(&MAIN_FM, RSV_DATA_CURRENT_ADDRESS + 50);

	uint8_t count = Get_RSV_Number();

	//if the table is empty
	if ((count == 0 || count == 0xff) && RSV_TABLE[0] == 0xff) {
		RSV_CHECK = 0;
		for (int i = 0; i < RSV_TABLE_SIZE; i++) {
			RSV_TABLE[i] = 0xff;	//fill the table with 0xff
		}
		RSV_MIN = 0;
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Reservation Table is empty\r\n");
		HAL_Delay(1);
#endif
#endif
		RESET_Tx.RSV_CMD =0;
		RESET_Tx.RSV_FLAG =0;
		//if reservation command is there in the reservation table
	} else if ((count != 0 || count != 0xff) && RSV_TABLE[0] != 0xff) {

		RSV_CHECK = 1;	//to indicate that the reservation table is not empty
		RSV_CMD_1 = RSV_TABLE[0];
		RSV_CMD_2 = RSV_TABLE[1];
		RSV_CMD_3 = RSV_TABLE[2];
		RSV_CMD_4 = RSV_TABLE[3];
		RSV_TIME = RSV_TABLE[4];			//closest reservation time

//		RSV_MIN = RSV_TABLE[RSV_TABLE_SIZE];//index 50 of the reservation table where the last RSV_MIN is stored
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("There are %d commands in the reservation table\r\n", count);
		HAL_Delay(1);
		Print_RSV_Table();
		myprintf("Reservation Time: %d\r\nReservation Minute Flag%d\r\n",
				RSV_TIME, RSV_MIN);
		HAL_Delay(1);
#endif
#endif
		RESET_Tx.RSV_CMD = count;
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"The reservation table is not empty ..there is some problem in addressing......\r\n");
		HAL_Delay(1);
#endif
#endif
		Sector_Erase_4B(&MAIN_FM, RSV_DATA_CURRENT_ADDRESS, 64);
		Sector_Erase_4B(&SMSN_FM, RSV_DATA_CURRENT_ADDRESS, 64);
	}
}

/*
 * @brief	to put command into the reservation table and sort it
 *
 * @param	CMD1,CMD2,CMD3 and CMD4 -- commands that are to be stored in the table
 * 			TIME --- time after which the command is to be executed
 *
 * @retval	0 -- if the table is full and command cannot be stored
 * 			1 -- if the table is empty and is the first reservation command
 * 			2 -- if the table is not empty and the reservation is not the first reservation
 */
uint8_t Reservation_Command(uint8_t CMD1, uint8_t CMD2, uint8_t CMD3,
		uint8_t CMD4, uint8_t TIME) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Command to execute on Reservation:%x%x%x%x\r\n", CMD1, CMD2, CMD3,
			CMD4);
	HAL_Delay(1);
	myprintf("Reservation to be executed after %d minutes\r\n", TIME);
	HAL_Delay(1);
#endif
#endif
	uint8_t count = Get_RSV_Number();
	if (count == 10) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Reservation Table is full\r\n Cannot store the Reservation Command\r\n");
		HAL_Delay(1);
#endif
#endif
		return 0;
	} else {
		if (RSV_CHECK != 0 && count != 0) {	//it means the table is in use right now, table is not empty
//#ifdef DEBUG_MODE
//#ifdef TRACE_MODE
//#else
//			myprintf("Sorting the table....\r\n");
//			HAL_Delay(1);
//#endif
//#endif
			//updating the latest time of previous reservations before updating the data of the last command received
			for (int i = 4; i < (count*5); i = i + 5)  {
				if (RSV_TABLE[i] > RSV_MIN) {
					RSV_TABLE[i] = RSV_TABLE[i] - RSV_MIN;
				} else {
					RSV_TABLE[i] = 0;
				}
			}
//			myprintf("Table before taking in the next Reservation\r\n");
//			HAL_Delay(1);
//			Print_RSV_Table();
			//since the table is not full, the last location is empty
			RSV_TABLE[45] = CMD1;
			RSV_TABLE[46] = CMD2;
			RSV_TABLE[47] = CMD3;
			RSV_TABLE[48] = CMD4;
			RSV_TABLE[49] = TIME;

			//sorting the table according to the latest time
			sort_table();

			//resetting the minute flag so that the counter starts from zero once again
			RSV_MIN = 0;
			RSV_CHECK = 1;

			//assigning the latest reservation data to the variables
			RSV_CMD_1 = RSV_TABLE[0];
			RSV_CMD_2 = RSV_TABLE[1];
			RSV_CMD_3 = RSV_TABLE[2];
			RSV_CMD_4 = RSV_TABLE[3];
			RSV_TIME = RSV_TABLE[4];

			SAVE_RSV();
			return 2;
		} else {//it means that there is no reservation data and the table is empty/ not in use
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("First Reservation Command Received\r\n");
			HAL_Delay(1);
#endif
#endif
			RSV_CHECK = 1;	//flag to indicate that reservation table is in use
			RSV_MIN = 0;		//setting the timer counter to zero

			RSV_TABLE[0] = CMD1;
			RSV_TABLE[1] = CMD2;
			RSV_TABLE[2] = CMD3;
			RSV_TABLE[3] = CMD4;
			RSV_TABLE[4] = TIME;//time after which the command is to be executed

			RSV_CMD_1 = RSV_TABLE[0];
			RSV_CMD_2 = RSV_TABLE[1];
			RSV_CMD_3 = RSV_TABLE[2];
			RSV_CMD_4 = RSV_TABLE[3];
			RSV_TIME = RSV_TABLE[4];

			SAVE_RSV();
			return 1;
		}
	}

}

/*
 * @brief	to delete the latest reservation table after execution and bring on the next closest RSV command
 *
 */
void Delete_Executed_RSV_Data() {
	for (int i = 0; i < 5; i++) {
		RSV_TABLE[i] = 0xff;
	}
	sort_table();
	uint8_t count = Get_RSV_Number();
	for (int i = 4; i < (count*5); i = i + 5) {
		if (RSV_TABLE[i] > RSV_MIN) {
			RSV_TABLE[i] = RSV_TABLE[i] - RSV_MIN;
		} else {
			RSV_TABLE[i] = 0;
		}
	}
	RSV_MIN = 0;
	if ((count != 0xff || count != 0) && RSV_TABLE[0] != 0xff) {
		//it means there is data
		RSV_CHECK=1;
		RSV_CMD_1 = RSV_TABLE[0];
		RSV_CMD_2 = RSV_TABLE[1];
		RSV_CMD_3 = RSV_TABLE[2];
		RSV_CMD_4 = RSV_TABLE[3];
		RSV_TIME = RSV_TABLE[4];
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf(
				"Next latest Reservation Command is %x%x%x%x after %d minutes\r\nRight now time is %d\r\n",
				RSV_CMD_1, RSV_CMD_2, RSV_CMD_3, RSV_CMD_4, RSV_TIME, RSV_MIN);
		HAL_Delay(1);
#endif
#endif

	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("No more Reserve Commands Remaining\r\n");
		HAL_Delay(1);
#endif
#endif
		RSV_CHECK = 0;
	}
}

/*
 * @brief		to save the reservation table data to the FM
 *
 * 				The data on the RSV_TABLE is saved in the FM along with the latest RSV_MIN
 * 				in case of Satellite Reset
 */
void SAVE_RSV() {
	MAIN_FM_MODE();		//mux mainside

	Sector_Erase_4B(&MAIN_FM, RSV_DATA_CURRENT_ADDRESS, 4);
	Sector_Erase_4B(&SMSN_FM, RSV_DATA_CURRENT_ADDRESS, 4);

	Page_Write_4B(&MAIN_FM, RSV_DATA_CURRENT_ADDRESS, RSV_TABLE,
	RSV_TABLE_SIZE);
	Page_Write_4B(&SMSN_FM, RSV_DATA_CURRENT_ADDRESS, RSV_TABLE,
	RSV_TABLE_SIZE);

	Page_Write_4B(&MAIN_FM, RSV_DATA_CURRENT_ADDRESS + 50, (uint8_t *)&RSV_MIN, 1);
	Page_Write_4B(&SMSN_FM, RSV_DATA_CURRENT_ADDRESS + 50, (uint8_t *)&RSV_MIN, 1);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf(
			"Reservation Data stored in the MAIN and SHARED Flash Memory \r\n");
	HAL_Delay(1);
	Print_RSV_Table();
#endif
#endif
}

/*
 * @brief		to check the reservation table and verify
 */
void Print_RSV_Table() {
	uint8_t k = 0, count = 0;
	for (uint8_t i = 0; i < 10; i++) {
		myprintf("RSV Table Command number %d................\r\n", i + 1);
		HAL_Delay(1);
		for (uint8_t j = 0; j < 5; j++) {
			myprintf("%x", RSV_TABLE[k]);
			HAL_Delay(1);
			k++;
		}
		myprintf("\r\n");
		HAL_Delay(1);
	}
	count = Get_RSV_Number();
	if (count != 0 || count != 0xff) {
		myprintf("Closest Next Reservation Command is ...................\r\n");
		HAL_Delay(1);
		myprintf("Command For MSN: %x %x %x %x \r\n", RSV_CMD_1, RSV_CMD_2,
				RSV_CMD_3, RSV_CMD_4);
		HAL_Delay(1);
		myprintf("Reservation Check Flag = %x \r\n", RSV_CHECK);
		HAL_Delay(1);
		myprintf(
				"Next latest Reservation Command Execution Time: %x \r\n Reservation Minute Count: %d....\r\n",
				RSV_TIME, RSV_MIN);
		HAL_Delay(1);
		myprintf("Total no of Reservation Commands: %d.....\r\n", count);
		HAL_Delay(1);
	}
}


