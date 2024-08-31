/*
 * mem_addr.c
 *
 *  Created on: Sep 22, 2022
 *      Author: Sushant
 *
 *
 * Code for checking, creating and updating the addresses of address, Missions and flag data
 *
 *	ALERT: 	If mission is accessing the Flash Memory then data is not stored
 *	in the SMSN FM so copy data once Mission is completed
 */

#include "mem_addr.h"

uint32_t ADDRESS_COUNTER;

void Get_Address_Current_Address() {

	uint8_t buff[4] = { '\0' };

	uint32_t MAIN_ADD = 0;
	uint32_t SMSN_ADD = 0;
	uint32_t INT_ADD = 0;

	uint8_t ADDRESS_BUF_MAIN[4] = { '\0' };
	uint8_t ADDRESS_BUF_SMSN[4] = { '\0' };
	uint8_t ADDRESS_BUF_INT[4] = { '\0' };

	READ_FRM_INTERNAL_FLASH(ADDRESS_DATA_LOC_ADDRESS_INT, ADDRESS_BUF_INT, 4);
	Bulk_Read_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, ADDRESS_BUF_MAIN, 4); //reading address data address from Main FM
	MAIN_FM_MODE();
	Bulk_Read_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, ADDRESS_BUF_SMSN, 4); //reading address data address from Shared

	MAIN_ADD = ADDRESS_BUF_MAIN[0] << 24 | ADDRESS_BUF_MAIN[1] << 16
			| ADDRESS_BUF_MAIN[2] << 8 | ADDRESS_BUF_MAIN[3];

	SMSN_ADD = ADDRESS_BUF_SMSN[0] << 24 | ADDRESS_BUF_SMSN[1] << 16
			| ADDRESS_BUF_SMSN[2] << 8 | ADDRESS_BUF_SMSN[3];

	INT_ADD = ADDRESS_BUF_INT[0] << 24 | ADDRESS_BUF_INT[1] << 16
			| ADDRESS_BUF_INT[2] << 8 | ADDRESS_BUF_INT[3];
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Data Read from Internal MCU Memory: %08x \r\n", INT_ADD);
	myprintf("Data Read from Main External Flash Memory: %08x \r\n", MAIN_ADD);
	myprintf("Data Read from Shared External Flash Memory: %08x \r\n",
			SMSN_ADD);
#endif // TRACE_MODE
#endif // DEBUG_MODE

	//TODO :add the flash memories priority wise to check for the address value
	//confusion may be created if data itself is "ff" so it needs to be sorted out

//	if ((ADDRESS_BUF_MAIN[0] == 0xff && ADDRESS_BUF_MAIN[1] == 0xff
//			&& ADDRESS_BUF_MAIN[2] == 0xff && ADDRESS_BUF_MAIN[3] == 0xff)
//			|| (ADDRESS_BUF_MAIN[0] == 0 && ADDRESS_BUF_MAIN[1] == 0
//					&& ADDRESS_BUF_MAIN[2] == 0 && ADDRESS_BUF_MAIN[3] == 0)) {

//		if ((ADDRESS_BUF_SMSN[0] == 0xff && ADDRESS_BUF_SMSN[1] == 0xff
//				&& ADDRESS_BUF_SMSN[2] == 0xff && ADDRESS_BUF_SMSN[3] == 0xff)
//				|| (ADDRESS_BUF_SMSN[0] == 0 && ADDRESS_BUF_SMSN[1] == 0
//						&& ADDRESS_BUF_SMSN[2] == 0 && ADDRESS_BUF_SMSN[3] == 0)) {

	if (INT_ADD == 0xFFFFFFFF || INT_ADD == 0x00000000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("INTERNAL FLASH does not contain address info address.\r\n");
#endif
#endif
		if (MAIN_ADD == 0xFFFFFFFF || MAIN_ADD == 0x00000000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf(
					"MAIN External FLASH does not contain address info address.\r\n");
#endif
#endif
			if (SMSN_ADD == 0xFFFFFFFF || SMSN_ADD == 0x00000000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf(
						"Shared External FLASH does not contain address info address.\r\n");
#endif
#endif
				//both the SMSN and MAIN FM are empty
				//if nothing is there use the default address

				ADDRESS_DATA_CURRENT_ADDRESS = ADDRESS_DATA_START_ADDRESS;
				buff[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24 & 0xff;
				buff[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16 & 0xff;
				buff[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8 & 0xff;
				buff[3] = ADDRESS_DATA_CURRENT_ADDRESS & 0xff;

				//writing the data into the MAIN &SMSN FM
				Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Main FM at addres 0
				Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);

				Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Shared Fm at address 0
				Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);

				//Erasing Address data location storing sector
				ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
				//writing data to internal flash
				WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
						ADDRESS_DATA_LOC_ADDRESS_INT, buff, 4);

				//TODO :need to write the data in the internal flash as well

			} else {
				//MAIN FM is empty but SMSN FM has data
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf(
						"Shared External FLASH contains address info address.\r\n");
#endif
#endif
				ADDRESS_DATA_CURRENT_ADDRESS = SMSN_ADD;
				buff[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24 & 0xff;
				buff[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16 & 0xff;
				buff[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8 & 0xff;
				buff[3] = ADDRESS_DATA_CURRENT_ADDRESS & 0xff;

				Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Main FM at address 0
				Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);	//writing data into the Main FM

				//Erasing Address data location storing sector
				ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
				//writing data to internal flash
				WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
						ADDRESS_DATA_LOC_ADDRESS_INT, buff, 4);
			}

		} else {
			//MAIN_FM has data
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("Main External FLASH contains address info address.\r\n");
#endif
#endif
//		if ((ADDRESS_BUF_SMSN[0] == 0xff && ADDRESS_BUF_SMSN[1] == 0xff
//				&& ADDRESS_BUF_SMSN[2] == 0xff && ADDRESS_BUF_SMSN[3] == 0xff)
//				|| (ADDRESS_BUF_SMSN[0] == 0 && ADDRESS_BUF_SMSN[1] == 0
//						&& ADDRESS_BUF_SMSN[2] == 0 && ADDRESS_BUF_SMSN[3] == 0)) {
			if (SMSN_ADD == 0xFFFFFFFF || SMSN_ADD == 0x00000000) {
				//MAIN_FM has data but SMSN_FM doesn't
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf(
						"Shared External FLASH does not contain address info address.\r\n");
#endif
#endif
//				ADDRESS_DATA_CURRENT_ADDRESS = Read_Byte_4B(&MAIN_FM, 0) << 24
//						| Read_Byte_4B(&MAIN_FM, 1) << 16
//						| Read_Byte_4B(&MAIN_FM, 2) << 8
//						| Read_Byte_4B(&MAIN_FM, 3);
				ADDRESS_DATA_CURRENT_ADDRESS = MAIN_ADD;
				buff[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24 & 0xff;
				buff[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16 & 0xff;
				buff[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8 & 0xff;
				buff[3] = ADDRESS_DATA_CURRENT_ADDRESS & 0xff;

				Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4); //erasing sector of Shared Fm at address 0
				Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4); // writing data to Shared FM at address 0

				//Erasing Address data location storing sector
				ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
				//writing data to internal flash
				WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
						ADDRESS_DATA_LOC_ADDRESS_INT, buff, 4);

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf("Writing Data to internal flash memory address.\r\n");
#endif
#endif
				//Erasing Address data location storing sector
				ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
				//writing data to internal flash
				WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
						ADDRESS_DATA_LOC_ADDRESS_INT, buff, 4);

				//Erasing Address data location storing sector
				ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
				//writing data to internal flash
				WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
						ADDRESS_DATA_LOC_ADDRESS_INT, buff, 4);

			} else {
				//data is available in both the FMs
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf(
						"Main and Shared External FLASH contains address info address.\r\n");
#endif
#endif

				if (MAIN_ADD != SMSN_ADD) {	//checking for consistency of address between both the Flash memory
					//ADDRESS data mismatch between the MAIN_FM and SMSN_FM
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
					myprintf("ADDRESS of Main and SMSN flash mismatch\r\n");
					myprintf(
							"Address of MAIN FM: %08x\r\nAddress of SMSN_FM: %08x\r\n",
							MAIN_ADD, SMSN_ADD);
#endif
#endif
					//confusion remains on what to do if data mismatch
					ADDRESS_DATA_CURRENT_ADDRESS = MAIN_ADD; // using main FM data as correct one
					buff[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24 & 0xff;
					buff[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16 & 0xff;
					buff[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8 & 0xff;
					buff[3] = ADDRESS_DATA_CURRENT_ADDRESS & 0xff;

					Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);
					Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);

				} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
					myprintf(
							"\r\nBoth the MAIN FM and SMSN FM has data and they match\r\n");
#endif
#endif
					ADDRESS_DATA_CURRENT_ADDRESS = MAIN_ADD;
					buff[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24 & 0xff;
					buff[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16 & 0xff;
					buff[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8 & 0xff;
					buff[3] = ADDRESS_DATA_CURRENT_ADDRESS & 0xff;
				}
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf("Writing Data to internal flash memory address.\r\n");
#endif
#endif
				//Erasing Address data location storing sector
				ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
				//writing data to internal flash
				WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
						ADDRESS_DATA_LOC_ADDRESS_INT, buff, 4);

			}
		}
	} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("INTERNAL FLASH contains address info address.\r\n");
#endif
#endif

		ADDRESS_DATA_CURRENT_ADDRESS = INT_ADD;
		buff[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24 & 0xff;
		buff[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16 & 0xff;
		buff[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8 & 0xff;
		buff[3] = ADDRESS_DATA_CURRENT_ADDRESS & 0xff;

		if (MAIN_ADD == 0xFFFFFFFF || MAIN_ADD == 0x00000000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
			myprintf(
					"MAIN External FLASH does not contain address info address.\r\n");
#endif
#endif
			if (SMSN_ADD == 0xFFFFFFFF || SMSN_ADD == 0x00000000) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
				myprintf(
						"Shared External FLASH does not contain address info address.\r\n");
#endif
#endif
				//both the SMSN and MAIN FM are empty
				//writing the data into the MAIN &SMSN FM
				Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Main FM at addres 0
				Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);

				Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Shared Fm at address 0
				Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);

				//TODO :need to write the data in the internal flash as well

			} else {
				//MAIN FM is empty but SMSN FM has data
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
				myprintf(
						"Shared External FLASH contains address info address.\r\n");
#endif
#endif
				Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Main FM at address 0
				Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);	//writing data into the Main FM

				if (INT_ADD != SMSN_ADD) {
					Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Shared Fm at address 0
					Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);
				}
			}

		} else {
			//MAIN_FM has data
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
			myprintf("Main External FLASH contains address info address.\r\n");
#endif
#endif
			//		if ((ADDRESS_BUF_SMSN[0] == 0xff && ADDRESS_BUF_SMSN[1] == 0xff
			//				&& ADDRESS_BUF_SMSN[2] == 0xff && ADDRESS_BUF_SMSN[3] == 0xff)
			//				|| (ADDRESS_BUF_SMSN[0] == 0 && ADDRESS_BUF_SMSN[1] == 0
			//						&& ADDRESS_BUF_SMSN[2] == 0 && ADDRESS_BUF_SMSN[3] == 0)) {

			if (SMSN_ADD == 0xFFFFFFFF || SMSN_ADD == 0x00000000) {
				//MAIN_FM has data but SMSN_FM doesn't
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
				myprintf(
						"Shared External FLASH does not contain address info address.\r\n");
#endif
#endif
				//				ADDRESS_DATA_CURRENT_ADDRESS = Read_Byte_4B(&MAIN_FM, 0) << 24
				//						| Read_Byte_4B(&MAIN_FM, 1) << 16
				//						| Read_Byte_4B(&MAIN_FM, 2) << 8
				//						| Read_Byte_4B(&MAIN_FM, 3);

				if (INT_ADD != MAIN_ADD) {
					Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Main FM at address 0
					Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);	//writing data into the Main FM
				}

				Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4); //erasing sector of Shared Fm at address 0
				Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4); // writing data to Shared FM at address 0

			} else {
				//data is available in both the FMs
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
				myprintf(
						"Internal, Main and Shared External FLASH contains address info address.\r\n");
#endif
#endif
				if (INT_ADD != MAIN_ADD) {
					Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);	// erasing sector of Main FM at address 0
					Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4);	//writing data into the Main FM

				}
				if (INT_ADD != SMSN_ADD) {
					Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4); //erasing sector of Shared Fm at address 0
					Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buff, 4); // writing data to Shared FM at address 0
				}

			}
		}

	}
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("\r\nAddress data current address\r\n : %08x\r\n",
			ADDRESS_DATA_CURRENT_ADDRESS);
#endif
#endif
}

/*
 *@brief		to write the latest address data into the FMs
 *
 *@retval		none
 *
 *@param		none
 */
void Write_Address_Data() {
#ifdef DEBUG_MODE

	if (!COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
		myprintf(
				"\r\nWriting address Data.....\r\nData written %d times before latest writing\r\n",
				ADDRESS_COUNTER);
#endif	//TRACE_MODE
	}
#endif	//DEBUG_MODE
	Check_Address_Data_Address(); //changes the address data (0,4) after 95000 times written

	ADDRESS_COUNTER++;

	ADDR_BUFFER[0] = HK_DATA_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[1] = HK_DATA_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[2] = HK_DATA_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[3] = HK_DATA_CURRENT_ADDRESS;

	ADDR_BUFFER[4] = SAT_LOG_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[5] = SAT_LOG_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[6] = SAT_LOG_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[7] = SAT_LOG_CURRENT_ADDRESS;

	ADDR_BUFFER[8] = CAM_MSN_IMG_INFO_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[9] = CAM_MSN_IMG_INFO_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[10] = CAM_MSN_IMG_INFO_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[11] = CAM_MSN_IMG_INFO_CURRENT_ADDRESS;

	ADDR_BUFFER[12] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[13] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[14] = PACDS_MSN_DATA_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[15] = PACDS_MSN_DATA_CURRENT_ADDRESS;

	ADDR_BUFFER[16] = SPDM_MSN_DATA_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[17] = SPDM_MSN_DATA_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[18] = SPDM_MSN_DATA_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[19] = SPDM_MSN_DATA_CURRENT_ADDRESS;

	ADDR_BUFFER[20] = CAM_MSN_DATA_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[21] = CAM_MSN_DATA_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[22] = CAM_MSN_DATA_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[23] = CAM_MSN_DATA_CURRENT_ADDRESS;

	ADDR_BUFFER[24] = FLAG_DATA_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[25] = FLAG_DATA_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[26] = FLAG_DATA_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[27] = FLAG_DATA_CURRENT_ADDRESS;

	ADDR_BUFFER[28] = RSV_DATA_CURRENT_ADDRESS >> 24;
	ADDR_BUFFER[29] = RSV_DATA_CURRENT_ADDRESS >> 16;
	ADDR_BUFFER[30] = RSV_DATA_CURRENT_ADDRESS >> 8;
	ADDR_BUFFER[31] = RSV_DATA_CURRENT_ADDRESS;

	ADDR_BUFFER[32] = ADDRESS_COUNTER >> 24;
	ADDR_BUFFER[33] = ADDRESS_COUNTER >> 16;
	ADDR_BUFFER[34] = ADDRESS_COUNTER >> 8;
	ADDR_BUFFER[35] = ADDRESS_COUNTER;

	//TODO :add functions to write in the internal flash as well

	Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_CURRENT_ADDRESS, 4);
	Page_Write_4B(&MAIN_FM, ADDRESS_DATA_CURRENT_ADDRESS, ADDR_BUFFER,
	ADDRESS_DATA_SIZE);

	Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_CURRENT_ADDRESS, 4);
	Page_Write_4B(&SMSN_FM, ADDRESS_DATA_CURRENT_ADDRESS, ADDR_BUFFER,
	ADDRESS_DATA_SIZE);

	//Erasing Address data location storing sector
	ERASE_INTERNAL_FLASH(ADDRESS_DATA_SECTOR);
	//writing data to internal flash
	WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR, ADDRESS_DATA_ADDRESS_INT,
			ADDR_BUFFER, ADDRESS_DATA_SIZE); //Erasing Address data location storing sector

#ifdef DEBUG_MODE

	if (!COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
		myprintf("Latest address data address......%08x\r\n",
				ADDRESS_DATA_CURRENT_ADDRESS);
#endif	//TRACE_MODE
	}
#endif	//DEBUG_MODE

}

/*
 * @brief		function to check if the address is written more than 95000 times
 * 				Address must be changed if data is stored more than 95000 times
 *
 * @param 		none
 *
 * @retval     none
 *
 */
void Check_Address_Data_Address() {
	uint32_t Counter = 0; //counter values from shared and internal can be added to verify the counter value is correct
	uint8_t buffer[4] = { '\0' };
	Counter = Read_Byte_4B(&MAIN_FM,
			ADDRESS_DATA_CURRENT_ADDRESS + ADDRESS_DATA_SIZE - 4) << 24
			| Read_Byte_4B(&MAIN_FM,
					ADDRESS_DATA_CURRENT_ADDRESS + ADDRESS_DATA_SIZE - 3) << 16
			| Read_Byte_4B(&MAIN_FM,
					ADDRESS_DATA_CURRENT_ADDRESS + ADDRESS_DATA_SIZE - 2) << 8
			| Read_Byte_4B(&MAIN_FM,
					ADDRESS_DATA_CURRENT_ADDRESS + ADDRESS_DATA_SIZE - 1);
	if (Counter >= 95000 && Counter != 0xffffffff) {

		ADDRESS_COUNTER = 0;	//resetting the address counter

		ADDRESS_DATA_CURRENT_ADDRESS += 4096;//increasing the address data current address

		buffer[0] = ADDRESS_DATA_CURRENT_ADDRESS >> 24;
		buffer[1] = ADDRESS_DATA_CURRENT_ADDRESS >> 16;
		buffer[2] = ADDRESS_DATA_CURRENT_ADDRESS >> 8;
		buffer[3] = ADDRESS_DATA_CURRENT_ADDRESS;

		Sector_Erase_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);
		Page_Write_4B(&MAIN_FM, ADDRESS_DATA_LOC_ADDRESS, buffer, 4);//Rewriting the Addresss's address data

		Sector_Erase_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, 4);
		Page_Write_4B(&SMSN_FM, ADDRESS_DATA_LOC_ADDRESS, buffer, 4);//Rewriting the Addresss's address data

		ERASE_INTERNAL_FLASH(ADDRESS_LOC_SECTOR);
		WRITE_TO_INTERNAL_FLASH(ADDRESS_LOC_SECTOR,
				ADDRESS_DATA_LOC_ADDRESS_INT, buffer, 4);

		//TODO : add writing function for the internal flash as well
#ifdef DEBUG_MODE

		if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Data written more than 95000 times......\r\n");
			myprintf("New address data made......\r\n");
			myprintf("%02x %02x %02x %02x\r\n %08x \r\n", buffer[0], buffer[1],
					buffer[2], buffer[3], ADDRESS_DATA_CURRENT_ADDRESS);
			myprintf("\r\n");
#endif	// TRACE_MODE
		}
#endif	//DEBUG_MODE
	} else {
#ifdef DEBUG_MODE

		if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf(
					"\r\nAddress data NOT changed...\r\nAddress writing Counter Value(in hex)....: %x\r\n",
					Counter);
#endif	//TRACE_MODE
		}
#endif	//DEBUG_MODE
	}
}

/*
 * @brief		to check and get the mission data address from the Flash
 * 				got to check if the address data is 0 or it is 0xff because all zeros also means there is no data
 */
void Get_Mission_Data_Address() {
	uint8_t Check = 0;
	uint8_t ADDRESS_BUF_MAIN[4];
	uint8_t ADDRESS_BUF_SMSN[4];
	uint8_t ADDRESS_BUF_INT[4];

	uint32_t ADDRESS_COUNTER_INT;
	uint32_t ADDRESS_COUNTER_MAIN;
	uint32_t ADDRESS_COUNTER_SMSN;

	READ_FRM_INTERNAL_FLASH(ADDRESS_DATA_ADDRESS_INT + ADDRESS_DATA_SIZE - 4,
			ADDRESS_BUF_INT, 4);
	Bulk_Read_4B(&MAIN_FM, ADDRESS_DATA_CURRENT_ADDRESS + ADDRESS_DATA_SIZE - 4,
			ADDRESS_BUF_MAIN, 4);
	Bulk_Read_4B(&SMSN_FM, ADDRESS_DATA_CURRENT_ADDRESS + ADDRESS_DATA_SIZE - 4,
			ADDRESS_BUF_SMSN, 4);

	ADDRESS_COUNTER_INT = ADDRESS_BUF_INT[0] << 24 | ADDRESS_BUF_INT[1] << 16
			| ADDRESS_BUF_INT[2] << 8 | ADDRESS_BUF_INT[3];
	ADDRESS_COUNTER_MAIN = ADDRESS_BUF_MAIN[0] << 24 | ADDRESS_BUF_MAIN[1] << 16
			| ADDRESS_BUF_MAIN[2] << 8 | ADDRESS_BUF_MAIN[3];
	ADDRESS_COUNTER_SMSN = ADDRESS_BUF_SMSN[0] << 24 | ADDRESS_BUF_SMSN[1] << 16
			| ADDRESS_BUF_SMSN[2] << 8 | ADDRESS_BUF_SMSN[3];

#ifdef DEBUG_MODE
	if (HK_FLAG && !COM_DATA_TX_FLAG) {

#ifdef TRACE_MODE
#else
		myprintf("Address Data Read from Internal MCU Memory: %08i \r\n",
				ADDRESS_COUNTER_INT);
		myprintf("Address Data Read from Main External Flash Memory: %08i \r\n",
				ADDRESS_COUNTER_MAIN);
		myprintf(
				"Address Data Read from Shared External Flash Memory: %08i \r\n",
				ADDRESS_COUNTER_SMSN);
#endif // TRACE_MODE
	}
#endif // DEBUG_MODE

	if (ADDRESS_COUNTER_INT == 0xFFFFFFFF
			|| ADDRESS_COUNTER_INT == 0x00000000) {
		if (ADDRESS_COUNTER_MAIN == 0xFFFFFFFF
				|| ADDRESS_COUNTER_MAIN == 0x00000000) {
			if (ADDRESS_COUNTER_SMSN == 0xFFFFFFFF
					|| ADDRESS_COUNTER_SMSN == 0x00000000) {
#ifdef DEBUG_MODE
				if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
					myprintf("Data not found in All Memory locations.\r\n");
#endif // TRACE_MODE
				}
#endif // DEBUG_MODE

				//No mission data in both the Flash Memories
				HK_DATA_CURRENT_ADDRESS = HK_DATA_START_ADDRESS;
				SAT_LOG_CURRENT_ADDRESS = SAT_LOG_START_ADDRESS;
//				RAD_MSN_DATA_CURRENT_ADDRESS = RAD_MSN_DATA_START_ADDRESS;
				PACDS_MSN_DATA_CURRENT_ADDRESS = PACDS_MSN_DATA_START_ADDRESS;
				CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_DATA_START_ADDRESS;
				CAM_MSN_IMG_INFO_CURRENT_ADDRESS =
				CAM_MSN_IMG_INFO_START_ADDRESS;
				SPDM_MSN_DATA_CURRENT_ADDRESS = SPDM_MSN_DATA_START_ADDRESS;
				FLAG_DATA_CURRENT_ADDRESS = FLAG_DATA_START_ADDRESS;
				RSV_DATA_CURRENT_ADDRESS = RSV_TABLE_START_ADDRESS;
				ADDRESS_COUNTER = 0;
#ifdef DEBUG_MODE

				if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
					myprintf(
							"There is no Address data in all of the FMs\r\nMaking Default address of FM.........\r\n");
#endif //TRACE_MODE
				}
#endif //DEBUG_MODE
			} else {
				//Address in SMSN FM but empty Main FM
				Bulk_Read_4B(&SMSN_FM, ADDRESS_DATA_CURRENT_ADDRESS,
						ADDR_BUFFER,
						ADDRESS_DATA_SIZE);

				HK_DATA_CURRENT_ADDRESS = ADDR_BUFFER[0] << 24
						| ADDR_BUFFER[1] << 16 | ADDR_BUFFER[2] << 8
						| ADDR_BUFFER[3];
				SAT_LOG_CURRENT_ADDRESS = ADDR_BUFFER[4] << 24
						| ADDR_BUFFER[5] << 16 | ADDR_BUFFER[6] << 8
						| ADDR_BUFFER[7];
				CAM_MSN_IMG_INFO_CURRENT_ADDRESS = ADDR_BUFFER[8] << 24
						| ADDR_BUFFER[9] << 16 | ADDR_BUFFER[10] << 8
						| ADDR_BUFFER[11];
				PACDS_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[12] << 24
						| ADDR_BUFFER[13] << 16 | ADDR_BUFFER[14] << 8
						| ADDR_BUFFER[15];
				SPDM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[16] << 24
						| ADDR_BUFFER[17] << 16 | ADDR_BUFFER[18] << 8
						| ADDR_BUFFER[19];
				CAM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[20] << 24
						| ADDR_BUFFER[21] << 16 | ADDR_BUFFER[22] << 8
						| ADDR_BUFFER[23];
				FLAG_DATA_CURRENT_ADDRESS = ADDR_BUFFER[24] << 24
						| ADDR_BUFFER[25] << 16 | ADDR_BUFFER[26] << 8
						| ADDR_BUFFER[27];
				RSV_DATA_CURRENT_ADDRESS = ADDR_BUFFER[28] << 24
						| ADDR_BUFFER[29] << 16 | ADDR_BUFFER[30] << 8
						| ADDR_BUFFER[31];
				ADDRESS_COUNTER = ADDR_BUFFER[32] << 24 | ADDR_BUFFER[33] << 16
						| ADDR_BUFFER[34] << 8 | ADDR_BUFFER[35];

#ifdef DEBUG_MODE

				if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifndef TRACE_MODE
					myprintf("Address data available in SMSN only.\r\n");
#else
#endif //ifndef TRACE_MODE
				}
#endif //ifdef DEBUG_MODE
			}
			Write_Address_Data();
			Check++;
		} else {
			//there is data in the MAIN FM

			if (ADDRESS_COUNTER_SMSN == 0xFFFFFFFF
					|| ADDRESS_COUNTER_SMSN == 0x00000000) {
				//Address in MAIN_FM but empty SMSN_FM
#ifdef DEBUG_MODE

				if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifndef TRACE_MODE
					myprintf("ADDRESS data available in MFM only.\r\n");
#else
			#endif //ifndef TRACE_MODE
				}
#endif //ifdef DEBUG_MODE
				Bulk_Read_4B(&MAIN_FM, ADDRESS_DATA_CURRENT_ADDRESS,
						ADDR_BUFFER,
						ADDRESS_DATA_SIZE);

				HK_DATA_CURRENT_ADDRESS = ADDR_BUFFER[0] << 24
						| ADDR_BUFFER[1] << 16 | ADDR_BUFFER[2] << 8
						| ADDR_BUFFER[3];
				SAT_LOG_CURRENT_ADDRESS = ADDR_BUFFER[4] << 24
						| ADDR_BUFFER[5] << 16 | ADDR_BUFFER[6] << 8
						| ADDR_BUFFER[7];
				CAM_MSN_IMG_INFO_CURRENT_ADDRESS = ADDR_BUFFER[8] << 24
						| ADDR_BUFFER[9] << 16 | ADDR_BUFFER[10] << 8
						| ADDR_BUFFER[11];
				PACDS_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[12] << 24
						| ADDR_BUFFER[13] << 16 | ADDR_BUFFER[14] << 8
						| ADDR_BUFFER[15];
				SPDM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[16] << 24
						| ADDR_BUFFER[17] << 16 | ADDR_BUFFER[18] << 8
						| ADDR_BUFFER[19];
				CAM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[20] << 24
						| ADDR_BUFFER[21] << 16 | ADDR_BUFFER[22] << 8
						| ADDR_BUFFER[23];
				FLAG_DATA_CURRENT_ADDRESS = ADDR_BUFFER[24] << 24
						| ADDR_BUFFER[25] << 16 | ADDR_BUFFER[26] << 8
						| ADDR_BUFFER[27];
				RSV_DATA_CURRENT_ADDRESS = ADDR_BUFFER[28] << 24
						| ADDR_BUFFER[29] << 16 | ADDR_BUFFER[30] << 8
						| ADDR_BUFFER[31];
				ADDRESS_COUNTER = ADDR_BUFFER[32] << 24 | ADDR_BUFFER[33] << 16
						| ADDR_BUFFER[34] << 8 | ADDR_BUFFER[35];

			} else {
				//data in both MAIN_FM and SMSN_FM
#ifdef DEBUG_MODE

				if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
					myprintf("There is data in both of the FMs.........\r\n");
#endif	// TRACE_MODE
				}
#endif	// DEBUG_MODE
				if (ADDRESS_COUNTER_MAIN == ADDRESS_COUNTER_SMSN) {
					Bulk_Read_4B(&MAIN_FM, ADDRESS_DATA_CURRENT_ADDRESS,
							ADDR_BUFFER,
							ADDRESS_DATA_SIZE);

				} else if (ADDRESS_COUNTER_MAIN > ADDRESS_COUNTER_SMSN) {
					Bulk_Read_4B(&MAIN_FM, ADDRESS_DATA_CURRENT_ADDRESS,
							ADDR_BUFFER,
							ADDRESS_DATA_SIZE);

				} else if (ADDRESS_COUNTER_MAIN < ADDRESS_COUNTER_SMSN) {
					Bulk_Read_4B(&SMSN_FM, ADDRESS_DATA_CURRENT_ADDRESS,
							ADDR_BUFFER,
							ADDRESS_DATA_SIZE);
				}

				HK_DATA_CURRENT_ADDRESS = ADDR_BUFFER[0] << 24
						| ADDR_BUFFER[1] << 16 | ADDR_BUFFER[2] << 8
						| ADDR_BUFFER[3];
				SAT_LOG_CURRENT_ADDRESS = ADDR_BUFFER[4] << 24
						| ADDR_BUFFER[5] << 16 | ADDR_BUFFER[6] << 8
						| ADDR_BUFFER[7];
				CAM_MSN_IMG_INFO_CURRENT_ADDRESS = ADDR_BUFFER[8] << 24
						| ADDR_BUFFER[9] << 16 | ADDR_BUFFER[10] << 8
						| ADDR_BUFFER[11];
				PACDS_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[12] << 24
						| ADDR_BUFFER[13] << 16 | ADDR_BUFFER[14] << 8
						| ADDR_BUFFER[15];
				SPDM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[16] << 24
						| ADDR_BUFFER[17] << 16 | ADDR_BUFFER[18] << 8
						| ADDR_BUFFER[19];
				CAM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[20] << 24
						| ADDR_BUFFER[21] << 16 | ADDR_BUFFER[22] << 8
						| ADDR_BUFFER[23];
				FLAG_DATA_CURRENT_ADDRESS = ADDR_BUFFER[24] << 24
						| ADDR_BUFFER[25] << 16 | ADDR_BUFFER[26] << 8
						| ADDR_BUFFER[27];
				RSV_DATA_CURRENT_ADDRESS = ADDR_BUFFER[28] << 24
						| ADDR_BUFFER[29] << 16 | ADDR_BUFFER[30] << 8
						| ADDR_BUFFER[31];
				ADDRESS_COUNTER = ADDR_BUFFER[32] << 24 | ADDR_BUFFER[33] << 16
						| ADDR_BUFFER[34] << 8 | ADDR_BUFFER[35];

			}
			Write_Address_Data();
			Check++;
		}

	} else {
		//internal flash contains address data
#ifdef DEBUG_MODE

		if (HK_FLAG && !COM_DATA_TX_FLAG) {
#ifdef TRACE_MODE
#else
			myprintf("Internal flash contains address data.\r\n");
#endif // TRACE_MODE
		}
#endif // DEBUG_MODE
		READ_FRM_INTERNAL_FLASH(ADDRESS_DATA_ADDRESS_INT, ADDR_BUFFER,
		ADDRESS_DATA_SIZE);

		HK_DATA_CURRENT_ADDRESS = ADDR_BUFFER[0] << 24 | ADDR_BUFFER[1] << 16
				| ADDR_BUFFER[2] << 8 | ADDR_BUFFER[3];
		SAT_LOG_CURRENT_ADDRESS = ADDR_BUFFER[4] << 24 | ADDR_BUFFER[5] << 16
				| ADDR_BUFFER[6] << 8 | ADDR_BUFFER[7];
		CAM_MSN_IMG_INFO_CURRENT_ADDRESS = ADDR_BUFFER[8] << 24
				| ADDR_BUFFER[9] << 16 | ADDR_BUFFER[10] << 8 | ADDR_BUFFER[11];
		PACDS_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[12] << 24
				| ADDR_BUFFER[13] << 16 | ADDR_BUFFER[14] << 8
				| ADDR_BUFFER[15];
		SPDM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[16] << 24
				| ADDR_BUFFER[17] << 16 | ADDR_BUFFER[18] << 8
				| ADDR_BUFFER[19];
		CAM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[20] << 24
				| ADDR_BUFFER[21] << 16 | ADDR_BUFFER[22] << 8
				| ADDR_BUFFER[23];
		FLAG_DATA_CURRENT_ADDRESS = ADDR_BUFFER[24] << 24
				| ADDR_BUFFER[25] << 16 | ADDR_BUFFER[26] << 8
				| ADDR_BUFFER[27];
		RSV_DATA_CURRENT_ADDRESS = ADDR_BUFFER[28] << 24 | ADDR_BUFFER[29] << 16
				| ADDR_BUFFER[30] << 8 | ADDR_BUFFER[31];
		ADDRESS_COUNTER = ADDR_BUFFER[32] << 24 | ADDR_BUFFER[33] << 16
				| ADDR_BUFFER[34] << 8 | ADDR_BUFFER[35];

		if (ADDRESS_COUNTER_MAIN == 0xFFFFFFFF
				|| ADDRESS_COUNTER_MAIN == 0x00000000) {
			//main fm does not contain address data
			Write_Address_Data();
		} else {
			if (ADDRESS_COUNTER_SMSN == 0xFFFFFFFF
					|| ADDRESS_COUNTER_SMSN == 0x00000000) {
//				SMSN FM does not contain Address Data
				// MFM and internal storage contains address data
				Write_Address_Data();
			} else {
//				All locations contatin address
				if (ADDRESS_COUNTER_INT != ADDRESS_COUNTER_MAIN
						|| ADDRESS_COUNTER_INT != ADDRESS_COUNTER_SMSN) {
					Write_Address_Data();
				}
			}
			Check++;
		}
	}

	if (Check != 0) {
		//TODO :Re-Check the index of buffer to ensure there is no error in assigning address

		HK_DATA_CURRENT_ADDRESS = ADDR_BUFFER[0] << 24 | ADDR_BUFFER[1] << 16
				| ADDR_BUFFER[2] << 8 | ADDR_BUFFER[3];
		SAT_LOG_CURRENT_ADDRESS = ADDR_BUFFER[4] << 24 | ADDR_BUFFER[5] << 16
				| ADDR_BUFFER[6] << 8 | ADDR_BUFFER[7];
		CAM_MSN_IMG_INFO_CURRENT_ADDRESS = ADDR_BUFFER[8] << 24
				| ADDR_BUFFER[9] << 16 | ADDR_BUFFER[10] << 8 | ADDR_BUFFER[11];
		PACDS_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[12] << 24
				| ADDR_BUFFER[13] << 16 | ADDR_BUFFER[14] << 8
				| ADDR_BUFFER[15];
		SPDM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[16] << 24
				| ADDR_BUFFER[17] << 16 | ADDR_BUFFER[18] << 8
				| ADDR_BUFFER[19];
		CAM_MSN_DATA_CURRENT_ADDRESS = ADDR_BUFFER[20] << 24
				| ADDR_BUFFER[21] << 16 | ADDR_BUFFER[22] << 8
				| ADDR_BUFFER[23];
		FLAG_DATA_CURRENT_ADDRESS = ADDR_BUFFER[24] << 24
				| ADDR_BUFFER[25] << 16 | ADDR_BUFFER[26] << 8
				| ADDR_BUFFER[27];
		RSV_DATA_CURRENT_ADDRESS = ADDR_BUFFER[28] << 24 | ADDR_BUFFER[29] << 16
				| ADDR_BUFFER[30] << 8 | ADDR_BUFFER[31];
		ADDRESS_COUNTER = ADDR_BUFFER[32] << 24 | ADDR_BUFFER[33] << 16
				| ADDR_BUFFER[34] << 8 | ADDR_BUFFER[35];
	}
}

/*
 * @brief	Prints all the current addresses to the selected debug port
 *
 * @param	none
 * @retval	none
 */
void Print_Address_Location() {
#ifdef DEBUG_MODE

	if (!COM_DATA_TX_FLAG) {
#ifndef TRACE_MODE
		myprintf("FLAG data current address: %08x\r\n",
				FLAG_DATA_CURRENT_ADDRESS);
		myprintf("HK data current address: %08x\r\n", HK_DATA_CURRENT_ADDRESS);
		myprintf("SAT log data current address: %08x\r\n",
				SAT_LOG_CURRENT_ADDRESS);
		myprintf("RSV TABLE data current address: %08x\r\n",
				RSV_DATA_CURRENT_ADDRESS);
		myprintf("PADCS data current address: %08x\r\n",
				PACDS_MSN_DATA_CURRENT_ADDRESS);
		myprintf("SPDM data current address: %08x\r\n",
				SPDM_MSN_DATA_CURRENT_ADDRESS);
		myprintf("CAM data current address: %08x\r\n",
				CAM_MSN_DATA_CURRENT_ADDRESS);
		myprintf("CAM Image Info Current Address: %08x\r\n",
				CAM_MSN_IMG_INFO_CURRENT_ADDRESS);
		myprintf("Address data current address: %08x\r\n",
				ADDRESS_DATA_CURRENT_ADDRESS);
		myprintf("ADDRESS COUNTER(in decimal): %d\r\n", ADDRESS_COUNTER);
		myprintf("\r\n");
#else
#endif //ifndef TRACE_MODE
	}
#endif //ifdef DEBUG_MODE
}

/*
 * @brief		get the flag data from the Flash Memory
 * 				for Flag data
 * @param		none
 * @retval 		none
 */
void Check_Flag_Data() {
	Get_Mission_Data_Address();
	uint32_t temp_addr;
	uint8_t count = 0;
	uint8_t i = 0;
	uint8_t FLAG_BUF_MAIN[FLAG_DATA_LEN];
	uint8_t FLAG_BUF_SMSN[FLAG_DATA_LEN];
	uint8_t FLAG_BUF_INT[FLAG_DATA_LEN];

	if (FLAG_DATA_CURRENT_ADDRESS > FLAG_DATA_START_ADDRESS) {
		temp_addr = FLAG_DATA_CURRENT_ADDRESS - 8;
	} else {
		temp_addr = FLAG_DATA_CURRENT_ADDRESS;
	}

	READ_FRM_INTERNAL_FLASH(INT_FLAG_STORE_ADDRESS, FLAG_BUF_INT,
	FLAG_DATA_LEN);

	Bulk_Read_4B(&MAIN_FM, temp_addr, FLAG_BUF_MAIN, FLAG_DATA_LEN);
	Bulk_Read_4B(&SMSN_FM, temp_addr, FLAG_BUF_SMSN, FLAG_DATA_LEN);

	//if starting address is not the current address then there must be data
	if (FLAG_BUF_INT[0] == 0xFF) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
		myprintf("Flag Data not available in Internal Flash.\r\n");
#endif
#endif

		if (FLAG_BUF_MAIN[0] == 0xFF) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
			myprintf("Flag Data not available in Main external Flash.\r\n");
#endif
#endif

			if (FLAG_BUF_SMSN[0] == 0xFF) {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
		#else
				myprintf(
						"Flag Data not available in Shared External Flash.\r\n");
#endif
#endif
				//it means there is no flag data in either of the FMs
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf("Initialising the Flag data to default state...\r\n");
#endif
#endif
				for (i = 0; i < FLAG_DATA_LEN; i++) {
					FLAG_DATA_BUFF[i] = 0xff;
				}

				GLOBAL_SYS_RST_FLAG = 0;
				ANT_DEPLOY_STATUS = UNDEPLOYED;
				DCOMMAND_RX_SUCCESS = UL_NOT_RX;
				MSN1_FLAG = OFF;
				MSN2_FLAG = OFF;
				MSN3_FLAG = ON;
				KILL_SWITCH = OFF;
				OPER_MODES = NRML_MODE;
				MSN_STATUS = 0;

				I_ADC1_CONV_CPLT = 0;
				HK_FLAG = 0;
				RESET_Tx.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
				RESET_Tx.UL_STAT = DCOMMAND_RX_SUCCESS;
				RESET_Tx.MSN_FLAG = MSN_STATUS;
				RESET_Tx.RSV_FLAG = 0;
				RESET_Tx.MSN1_STAT = MSN1_FLAG;
				RESET_Tx.MSN2_STAT = MSN2_FLAG;
				RESET_Tx.MSN3_STAT = MSN3_FLAG;

				SAT_HEALTH.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
				SAT_HEALTH.UL_STAT = DCOMMAND_RX_SUCCESS;
				SAT_HEALTH.OPER_MODE = NRML_MODE;
				SAT_HEALTH.MSN_FLAG = MSN_STATUS;
				SAT_HEALTH.RSV_FLAG = 0;
				SAT_HEALTH.MSN3_STAT = MSN3_FLAG;
				SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
				SAT_HEALTH.MSN1_STAT = MSN1_FLAG;

				FLAG_DATA_BUFF[0] = GLOBAL_SYS_RST_FLAG;
				FLAG_DATA_BUFF[1] = ANT_DEPLOY_STATUS;
				FLAG_DATA_BUFF[2] = DCOMMAND_RX_SUCCESS;
				FLAG_DATA_BUFF[3] = MSN1_FLAG;
				FLAG_DATA_BUFF[4] = MSN2_FLAG;
				FLAG_DATA_BUFF[5] = MSN3_FLAG;
				FLAG_DATA_BUFF[6] = KILL_SWITCH;
				FLAG_DATA_BUFF[7] = OPER_MODES;

				// storing flags in internal
				ERASE_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR);
				WRITE_TO_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR,
						INT_FLAG_STORE_ADDRESS, FLAG_DATA_BUFF, FLAG_DATA_LEN);

				//storing flags in external MAIN
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Page_Write_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS,
						FLAG_DATA_BUFF,
						FLAG_DATA_LEN);
				//storing flags in external Shared

				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				Page_Write_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS,
						FLAG_DATA_BUFF,
						FLAG_DATA_LEN);
				FLAG_DATA_CURRENT_ADDRESS += 8;
				Write_Address_Data();

			} else {
				//it means there is flag data in SMSN FM but not in the Main FM
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
				myprintf(
						"There is Flag Data in the SMSN FM but not in the Main FM......\r\n");
#endif
#endif
				MSN_STATUS = 0;

				I_ADC1_CONV_CPLT = 0;
				HK_FLAG = 0;

				GLOBAL_SYS_RST_FLAG = FLAG_BUF_SMSN[0];
				ANT_DEPLOY_STATUS = FLAG_BUF_SMSN[1];
				DCOMMAND_RX_SUCCESS = FLAG_BUF_SMSN[2];
				MSN1_FLAG = FLAG_BUF_SMSN[3];
				MSN2_FLAG = FLAG_BUF_SMSN[4];
				MSN3_FLAG = FLAG_BUF_SMSN[5];
				KILL_SWITCH = FLAG_BUF_SMSN[6];
				OPER_MODES = FLAG_BUF_SMSN[7];

				RESET_Tx.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
				RESET_Tx.UL_STAT = DCOMMAND_RX_SUCCESS;
				RESET_Tx.MSN_FLAG = MSN_STATUS;
				RESET_Tx.RSV_FLAG = 0;
				RESET_Tx.MSN1_STAT = MSN1_FLAG;
				RESET_Tx.MSN2_STAT = MSN2_FLAG;
				RESET_Tx.MSN3_STAT = MSN3_FLAG;

				SAT_HEALTH.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
				SAT_HEALTH.UL_STAT = DCOMMAND_RX_SUCCESS;
				SAT_HEALTH.OPER_MODE = OPER_MODES;
				SAT_HEALTH.MSN_FLAG = MSN_STATUS;
				SAT_HEALTH.RSV_FLAG = 0;
				SAT_HEALTH.MSN3_STAT = MSN3_FLAG;
				SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
				SAT_HEALTH.MSN1_STAT = MSN1_FLAG;

				ERASE_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR);
				WRITE_TO_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR,
						INT_FLAG_STORE_ADDRESS, FLAG_BUF_SMSN, FLAG_DATA_LEN);

				Sector_Erase_4B(&MAIN_FM, temp_addr, 64); //erasing the sector where flag data is to be stored
				Page_Write_4B(&MAIN_FM, temp_addr, FLAG_BUF_SMSN,
				FLAG_DATA_LEN);	// storing the flag data to the Main FM

			}
		} else {
			//it means there is data in the Main FM

			MSN_STATUS = 0;
			I_ADC1_CONV_CPLT = 0;
			HK_FLAG = 0;
			GLOBAL_SYS_RST_FLAG = FLAG_BUF_MAIN[0];
			ANT_DEPLOY_STATUS = FLAG_BUF_MAIN[1];
			DCOMMAND_RX_SUCCESS = FLAG_BUF_MAIN[2];
			MSN1_FLAG = FLAG_BUF_MAIN[3];
			MSN2_FLAG = FLAG_BUF_MAIN[4];
			MSN3_FLAG = FLAG_BUF_MAIN[5];
			KILL_SWITCH = FLAG_BUF_MAIN[6];
			OPER_MODES = FLAG_BUF_MAIN[7];

			RESET_Tx.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
			RESET_Tx.UL_STAT = DCOMMAND_RX_SUCCESS;
			RESET_Tx.MSN_FLAG = 0;
			RESET_Tx.RSV_FLAG = 0;
			RESET_Tx.MSN1_STAT = MSN1_FLAG;
			RESET_Tx.MSN2_STAT = MSN2_FLAG;
			RESET_Tx.MSN3_STAT = MSN3_FLAG;
			SAT_HEALTH.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
			SAT_HEALTH.UL_STAT = DCOMMAND_RX_SUCCESS;
			SAT_HEALTH.OPER_MODE = OPER_MODES;
			SAT_HEALTH.MSN_FLAG = 0;
			SAT_HEALTH.RSV_FLAG = 0;
			SAT_HEALTH.MSN3_STAT = MSN3_FLAG;
			SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
			SAT_HEALTH.MSN1_STAT = MSN1_FLAG;

			ERASE_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR);
			WRITE_TO_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR,
					INT_FLAG_STORE_ADDRESS, FLAG_BUF_MAIN,
					FLAG_DATA_LEN);

			if (FLAG_BUF_SMSN[0] == 0xFF) {

				//it means there is no data in the SMSN FM but there is in the Main FM
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64); // erasing the sector where the flag data is to be stored in SFM
				Page_Write_4B(&SMSN_FM, temp_addr, FLAG_BUF_MAIN,
				FLAG_DATA_LEN);	// storing the flag data to the Shared FM

			} else {

				//it means there is Flag data in both the Main FM and the SMSN FM
				for (i = 0; i < FLAG_DATA_LEN; i++) {
					if (FLAG_BUF_MAIN[i] == FLAG_BUF_SMSN[i]) {
						count++;
					}
				}
				if (count == FLAG_DATA_LEN) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
					myprintf(
							"There is flag data MAIN FM and the SMSN FM and they match....\r\n");
#endif
#endif
				} else {
					// taking MAIN_FM flag data as the correct ones in case of data mismatch
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
					myprintf(
							"The data in the MAIN FM and the SMSN FM do not match\r\n");
					myprintf("Data in the MAIN FM (in HEX): \r\n");
					for (i = 0; i < FLAG_DATA_LEN; i++) {
						myprintf("%02x", FLAG_BUF_MAIN[i]);
					}
					myprintf("\r\nData in the SMSN FM:\r\n");
					for (i = 0; i < FLAG_DATA_LEN; i++) {
						myprintf("%02x", FLAG_BUF_SMSN[i]);
					}
					myprintf("\r\nMaking the Flag data from the MAIN FM\r\n");
#endif
#endif
					// storing the flags to shared external FM
					Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
					Page_Write_4B(&SMSN_FM, temp_addr, FLAG_BUF_MAIN,
					FLAG_DATA_LEN);
				}
			}
		}
	} else {
		// data availabe in INT FM
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Flag Data available in Intenal FM. Data: ");
		for (int i = 0; i < FLAG_DATA_LEN; i++) {
			myprintf("%02x ", FLAG_BUF_INT[i]);
		}
		myprintf("\r\n");
#endif
#endif
		MSN_STATUS = 0;
		I_ADC1_CONV_CPLT = 0;
		HK_FLAG = 0;

		GLOBAL_SYS_RST_FLAG = FLAG_BUF_INT[0];
		ANT_DEPLOY_STATUS = FLAG_BUF_INT[1];
		DCOMMAND_RX_SUCCESS = FLAG_BUF_INT[2];
		MSN1_FLAG = FLAG_BUF_INT[3];
		MSN2_FLAG = FLAG_BUF_INT[4];
		MSN3_FLAG = FLAG_BUF_INT[5];
		KILL_SWITCH = FLAG_BUF_INT[6];
		OPER_MODES = FLAG_BUF_INT[7];

		RESET_Tx.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
		RESET_Tx.UL_STAT = DCOMMAND_RX_SUCCESS;
		RESET_Tx.MSN_FLAG = MSN_STATUS;
		RESET_Tx.RSV_FLAG = 0;
		RESET_Tx.MSN1_STAT = MSN1_FLAG;
		RESET_Tx.MSN2_STAT = MSN2_FLAG;
		RESET_Tx.MSN3_STAT = MSN3_FLAG;
		SAT_HEALTH.ANT_DEP_STAT = ANT_DEPLOY_STATUS;
		SAT_HEALTH.UL_STAT = DCOMMAND_RX_SUCCESS;
		SAT_HEALTH.OPER_MODE = OPER_MODES;
		SAT_HEALTH.MSN_FLAG = MSN_STATUS;
		SAT_HEALTH.RSV_FLAG = 0;
		SAT_HEALTH.MSN3_STAT = MSN3_FLAG;
		SAT_HEALTH.MSN2_STAT = MSN2_FLAG;
		SAT_HEALTH.MSN1_STAT = MSN1_FLAG;
		count = 0;
		for (int i = 0; i < FLAG_DATA_LEN; i++) {
			if (FLAG_BUF_INT[i] == FLAG_BUF_MAIN[i]) {
				count++;
			}
		}
		if (count == FLAG_DATA_LEN) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
			myprintf("MAIN FM also has same set of Flag info.\r\n");
#endif
#endif
		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
			myprintf("Main FM does not have same flag data. Overwriting.\r\n");
#endif
#endif
			Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
			Page_Write_4B(&MAIN_FM, temp_addr, FLAG_BUF_INT, FLAG_DATA_LEN);
		}
		count = 0;

		for (int i = 0; i < FLAG_DATA_LEN; i++) {
			if (FLAG_BUF_INT[i] == FLAG_BUF_SMSN[i]) {
				count++;
			}
		}
		if (count == FLAG_DATA_LEN) {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
			myprintf("SMSN FM contains same flag data as internal.\r\n");
#endif
#endif
		} else {
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
			#else
			myprintf("SMSN FM does not have same flag data. Overwriting.\r\n");
#endif
#endif
			Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
			Page_Write_4B(&SMSN_FM, temp_addr, FLAG_BUF_INT,
			FLAG_DATA_LEN);
		}

	}

#ifdef	DEBUG_MODE
#ifdef  TRACE_MODE
#else
	myprintf("extracted the flag data from Flash::\r\n");
	Print_Flag_Data();
#endif	// TRACE_MODE
#endif	//DEBUG_MODE
}

/*
 * @brief Prints the current flag status of the satellites
 *
 * @param	none
 * @retval	none
 *
 */
void Print_Flag_Data() {
	myprintf("Global sys reset:%02x\r\n", GLOBAL_SYS_RST_FLAG);
	myprintf("ant deployment status: %02x\r\n", ANT_DEPLOY_STATUS);
	myprintf("Uplink status flag: %02x \r\n", DCOMMAND_RX_SUCCESS);
	myprintf("MSN1 flag: %02x\r\n", MSN1_FLAG);
	myprintf("MSN2 flag: %02x\r\n", MSN2_FLAG);
	myprintf("MSN3 flag: %02x\r\n", MSN3_FLAG);
	myprintf("KILL_Switch Flag: %02x\r\n", KILL_SWITCH);
	myprintf("Operation Mode Flag: %02x\r\n", OPER_MODES);
}

/*
 * @brief		to store the flag data into the corresponding Flash Memory
 */
void Store_Flag_Data() {
	Get_Mission_Data_Address();
#ifdef	DEBUG_MODE
#ifdef  TRACE_MODE
#else
	myprintf("Storing flag data to FM ........\r\n");
	HAL_Delay(1);
#endif
#endif
	if (FLAG_DATA_CURRENT_ADDRESS - FLAG_DATA_END_ADDRESS <= 8) {
#ifdef	DEBUG_MODE
#ifdef  TRACE_MODE
#else
		myprintf("End of the Flag data address reached........\r\n");
		myprintf("Deleting the Starting Sector Of Flag Data address\r\n");
		myprintf(
				"Looping the Address data from the Starting address......\r\n");
#endif
#endif
		FLAG_DATA_CURRENT_ADDRESS = FLAG_DATA_START_ADDRESS;
		Sector_Erase_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS, 64);
		Sector_Erase_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS, 64);

	}

	if (Read_Byte_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS) != 0xff) {
		if (Read_Byte_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS) != 0xff) {
			//it means there is previous flag data in both the MAIN FM and SMSN FM
			//TODO: add code for the internal flash as well
			Sector_Erase_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS, 4);
			Sector_Erase_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS, 4);

		} else if (Read_Byte_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS) == 0xff) {

			//there is previOus data in MAIN FM only
			Sector_Erase_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS, 4);

		}
	} else {
		if (Read_Byte_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS) != 0xff) {
			//there is previus data in the SMSN flash only
			Sector_Erase_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS, 4);
		}
	}
	//TODO :sort out the important flags and store them only....

	FLAG_DATA_BUFF[0] = GLOBAL_SYS_RST_FLAG;
	FLAG_DATA_BUFF[1] = ANT_DEPLOY_STATUS;
	FLAG_DATA_BUFF[2] = DCOMMAND_RX_SUCCESS;
	FLAG_DATA_BUFF[3] = MSN1_FLAG;
	FLAG_DATA_BUFF[4] = MSN2_FLAG;
	FLAG_DATA_BUFF[5] = MSN3_FLAG;
	FLAG_DATA_BUFF[6] = KILL_SWITCH;
	FLAG_DATA_BUFF[7] = OPER_MODES;

	ERASE_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR);
	WRITE_TO_INTERNAL_FLASH(INT_FLAG_STORE_SECTOR, INT_FLAG_STORE_ADDRESS,
			FLAG_DATA_BUFF, FLAG_DATA_LEN);

	Page_Write_4B(&MAIN_FM, FLAG_DATA_CURRENT_ADDRESS, FLAG_DATA_BUFF,
	FLAG_DATA_LEN);
	Page_Write_4B(&SMSN_FM, FLAG_DATA_CURRENT_ADDRESS, FLAG_DATA_BUFF,
	FLAG_DATA_LEN);
	FLAG_DATA_CURRENT_ADDRESS += 8;

	Write_Address_Data();
#ifdef	DEBUG_MODE
#ifdef  TRACE_MODE
#else
	myprintf("flag data stored:\r\n");
	HAL_Delay(1);
	Print_Flag_Data();
#endif
#endif
}

/*
 * @brief		to check if the CAM MSN address is ending or not
 *
 */
void Check_CAM_Address() {
	Get_Mission_Data_Address();	//taking the latest mission address data to send to CAM MCU to store the photo
	uint32_t temp_addr;
	if (CAM_MSN_DATA_CURRENT_ADDRESS == 0x00000000) {
		CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_DATA_START_ADDRESS;
		Write_Address_Data();

	} else {
		if (CAM_MSN_DATA_END_ADDRESS - CAM_MSN_DATA_CURRENT_ADDRESS
				< SECTOR_SIZE * 14) {
			CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_DATA_START_ADDRESS;
			temp_addr = CAM_MSN_DATA_CURRENT_ADDRESS;
			for (int i = 0; i < 15; i++) {
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				temp_addr = temp_addr + SECTOR_SIZE;
			}
			Write_Address_Data();
			Save_SAT_Log(0xEE, 0x30, 0x30, 0xEE);//to indicate that CAM MSN data end is reached

		} else {
			temp_addr = CAM_MSN_DATA_CURRENT_ADDRESS;
			for (int i = 0; i < 5; i++) {//TODO: check for the maximum photo size from CAM
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				temp_addr = temp_addr + SECTOR_SIZE;
			}
		}
	}

	if (CAM_MSN_IMG_INFO_CURRENT_ADDRESS == 0x00000000) {
		CAM_MSN_IMG_INFO_CURRENT_ADDRESS = CAM_MSN_IMG_INFO_START_ADDRESS;
		Write_Address_Data();
	} else {
		if (CAM_MSN_IMG_INFO_END_ADDRESS - CAM_MSN_IMG_INFO_CURRENT_ADDRESS
				< 30) {
			CAM_MSN_DATA_CURRENT_ADDRESS = CAM_MSN_IMG_INFO_START_ADDRESS;
			Sector_Erase_4B(&MAIN_FM, CAM_MSN_IMG_INFO_CURRENT_ADDRESS, 64);
			Write_Address_Data();

		}
	}
}

/*
 *  @brief		to check the current address of SPDM MSN and delete the sectors to store data
 */
void Check_SPDM_Address() {
	Get_Mission_Data_Address();	//taking the latest mission address data to send to CAM MCU to store the photo
	uint32_t temp_addr;
	if (SPDM_MSN_DATA_CURRENT_ADDRESS == 0x00000000) {
		SPDM_MSN_DATA_CURRENT_ADDRESS = SPDM_MSN_DATA_START_ADDRESS;
		Write_Address_Data();
	} else {
		if (SPDM_MSN_DATA_END_ADDRESS
				- SPDM_MSN_DATA_CURRENT_ADDRESS< SECTOR_SIZE) {
			SPDM_MSN_DATA_CURRENT_ADDRESS = SPDM_MSN_DATA_START_ADDRESS;
			temp_addr = SPDM_MSN_DATA_CURRENT_ADDRESS;
			for (int i = 0; i < 40; i++) {//erasing all the sectors of SPDM msn but the time to erase 40 sectors have to be taken into consideration
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				temp_addr = temp_addr + SECTOR_SIZE;
			}
			Write_Address_Data();
			Save_SAT_Log(0xEE, 0x50, 0x50, 0xEE);//to indicate that SPDM MSN data end is reached
		} else {
			temp_addr = SPDM_MSN_DATA_CURRENT_ADDRESS;
			if (Read_Byte_4B(&MAIN_FM, temp_addr) != 0xff) {
				Sector_Erase_4B(&MAIN_FM, ADDRESS_COUNTER, 4);//erasing 4KB subsector for SPDM because data size is small
			}
		}
	}
}

/*
 * @brief		to check the current address of PACDS MSN and delete the sectors to store data
 */
void Check_PACDS_Address() {
	Get_Mission_Data_Address();	//taking the latest mission address data to send to CAM MCU to store the photo
	uint32_t temp_addr;
	if (PACDS_MSN_DATA_CURRENT_ADDRESS == 0x00000000) {
		PACDS_MSN_DATA_CURRENT_ADDRESS = PACDS_MSN_DATA_START_ADDRESS;
		Write_Address_Data();
	} else {
		if (PACDS_MSN_DATA_END_ADDRESS
				- PACDS_MSN_DATA_CURRENT_ADDRESS< SECTOR_SIZE) {
			PACDS_MSN_DATA_CURRENT_ADDRESS = PACDS_MSN_DATA_START_ADDRESS;
			temp_addr = PACDS_MSN_DATA_CURRENT_ADDRESS;
			for (int i = 0; i < 100; i++) {
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				temp_addr = temp_addr + SECTOR_SIZE;
			}
			Write_Address_Data();
			Save_SAT_Log(0xEE, 0x40, 0x40, 0xEE);//to indicate that PACDS MSN data end is reached

		} else {
			temp_addr = PACDS_MSN_DATA_CURRENT_ADDRESS;
			if (Read_Byte_4B(&MAIN_FM, temp_addr) != 0xff) {
				Sector_Erase_4B(&MAIN_FM, temp_addr, 4);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 4);
			}
		}
	}
}

/*
 * @brief	Checks the current address of SAT Log storage and delete the sectors to store data
 *
 *
 */
void Check_SAT_LOG_Address() {
	Get_Mission_Data_Address();
	uint32_t temp_addr;
	if (SAT_LOG_CURRENT_ADDRESS == 0x00000000) {
		SAT_LOG_CURRENT_ADDRESS = SAT_LOG_START_ADDRESS;
		Write_Address_Data();
	} else {
		if (SAT_LOG_CURRENT_ADDRESS >= SAT_LOG_END_ADDRESS
				|| (SAT_LOG_END_ADDRESS - SAT_LOG_CURRENT_ADDRESS) < SAT_LOG_LEN) {

			SAT_LOG_CURRENT_ADDRESS = SAT_LOG_START_ADDRESS;
			temp_addr = SAT_LOG_CURRENT_ADDRESS;

			for (int i = 0; i < 50; i++) {
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				temp_addr = temp_addr + SECTOR_SIZE;
			}
			Write_Address_Data();
			Save_SAT_Log(0xEE, 0xBE, 0xBE, 0xEE);//to indicate that satellite log data address end is reached
		} else {
			temp_addr = SAT_LOG_CURRENT_ADDRESS;
			if (Read_Byte_4B(&MAIN_FM, temp_addr) != 0xff) {
				Sector_Erase_4B(&MAIN_FM, temp_addr, 4);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 4);
			}
		}
	}
}

void Check_HK_Data_Address() {
	Get_Mission_Data_Address();
	uint32_t temp_addr;
	if (HK_DATA_CURRENT_ADDRESS == 0x00000000) {
		HK_DATA_CURRENT_ADDRESS = HK_DATA_START_ADDRESS;
		Write_Address_Data();
	} else {
		if (HK_DATA_CURRENT_ADDRESS >= HK_DATA_END_ADDRESS
				|| (HK_DATA_END_ADDRESS - HK_DATA_CURRENT_ADDRESS) < HK_DATA_LEN) {
			HK_DATA_CURRENT_ADDRESS = HK_DATA_START_ADDRESS;
			temp_addr = HK_DATA_CURRENT_ADDRESS;
			for (int i = 0; i < 199; i++) {
				Sector_Erase_4B(&MAIN_FM, temp_addr, 64);
				Sector_Erase_4B(&SMSN_FM, temp_addr, 64);
				temp_addr = temp_addr + SECTOR_SIZE;
			}
			if (!MSN_STATUS) {
				Write_Address_Data();
			}
			Save_SAT_Log(0xEE, 0xBE, 0xBE, 0xEE);//to indicate that HK data address end is reached
		} else {
			temp_addr = HK_DATA_CURRENT_ADDRESS;
		}
	}
}

/*
 *	@brief		to send the data from the FM to the PC or the COM through UART
 *
 *	@param		*hspi	--	MAIN FM or the SMSN FM
 *				address	-- address from which the data is to be read and sent
 *				PCKT_NUM--no of packets to send
 *
 *				PORT:
 *					0x01: for transmitting data through USB
 *					0x02: for transmitting data through UART
 */
void FM_Data_To_PC(SPI_HandleTypeDef *hspi, uint32_t *ADDRESS,
		uint32_t PCKT_NUM, uint8_t PORT) {
	uint8_t Tx_PC_Data[40];
	uint8_t Tx_COM_Data[40];
	uint32_t addr = *ADDRESS;
	memset(Tx_PC_Data, '\0', 40);
	myprintf("Address: %08x\r\n PCKT_NUM %i\r\n", addr, PCKT_NUM);
	switch (PORT) {
	case 0x01:
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		for (int i = 0; i < PCKT_NUM; i++) {
			Bulk_Read_4B(hspi, addr, Tx_PC_Data, 40);
			HAL_Delay(100);		//delay to be decreased after testing
			for (int j = 0; j < 40; j++) {
				myprintf("%02x", Tx_PC_Data[j]);
				HAL_Delay(1);
			}
			myprintf("\r\n");
			HAL_Delay(1);
//			CDC_Transmit_FS((uint8_t *)Tx_PC_Data, 40);		//this did not work (random data was coming)
			addr = addr + 40;
			memset(Tx_PC_Data, '\0', 40);
		}
#endif
#endif
		break;
	case 0x02:
		for (int i = 0; i < PCKT_NUM; i++) {
			Bulk_Read_4B(hspi, addr, Tx_COM_Data, 40);
			MainToSubSys(&COM_TCV_STREAM, Tx_COM_Data, 40, 0);
			addr = addr + 40;
			memset(Tx_COM_Data, '\0', 40);
		}
//		MainToSubSys(&COM_TCV_STREAM, FOOTER, 1, 0);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("Data sent to COM........\r\n");
		HAL_Delay(1);
#endif
#endif
		break;
	default:
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("wrong port number selected...\r\n");
		HAL_Delay(1);
#endif
#endif
		break;
	}
	myprintf("Data Transfer Complete.......\r\n");
	HAL_Delay(1);
}

/*
 * @brief Transfers MSN data from Shared Flash Memory to Main Flash Memory
 *
 * @param	ADDRESS 		Starting address of the sector from which data is to be transferred from
 * 			no_of_Sectors	no of sectors ( to transfer from SFM to MFM
 * @retval	uint8_t 		Status of the transfer process
 *
 *
 */
uint8_t Transfer_Sector_SFM_MFM(uint32_t ADDRESS, uint8_t no_of_sectors) {
	uint8_t TEMP[256];
	uint32_t TEMP_ADDR = 0x00;
	MAIN_FM_MODE(); 			// shifting the access of SFM to OBC
	TEMP_ADDR = ADDRESS;
	for (int i = 0; i < no_of_sectors; i++) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Copying data starting from sector %i, with address %08x", i,
				TEMP_ADDR);
		HAL_Delay(1);
#else
    #endif
#endif
		for (int j = 0; j < 256; j++) {	//copy data from 256 pages of first sector before moving to next sector
			Bulk_Read_4B(&SMSN_FM, TEMP_ADDR, TEMP, 256);
			Page_Write_4B(&MAIN_FM, TEMP_ADDR, TEMP, 256);
			HAL_Delay(50); 	// wait 1800 uS for page program operation to finish
			TEMP_ADDR += 256;
		}
		if (TEMP_ADDR == (ADDRESS + i * SECTOR_SIZE)) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("%i Sector copy completed.", i);
			HAL_Delay(1);
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
			myprintf("Addressing Error during Sector Programming");
			HAL_Delay(1);
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
			return 0;
		}
	}
	if (TEMP_ADDR == (ADDRESS + no_of_sectors * SECTOR_SIZE)) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("MSN data copy successful from SFM to MFM");
		HAL_Delay(1);
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		return 1;
	} else {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Addressing Error during Sector Programming");
		HAL_Delay(1);
#else
			#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
		return 0;

	}
}

/*
 * @brief Transfers MSN data from Shared Flash Memory to Main Flash Memory
 *
 * @param	ADDRESS 		Starting address of the sector from which data is to be transferred from
 * 			size			no of bytes to copy from SFM to MFM
 * @retval	none
 *
 *
 */
void Transfer_Bytes_SFM_MFM(uint32_t ADDRESS, uint32_t size) {
	uint8_t TEMP[PAGE_SIZE];
	uint32_t TEMP_ADDR = 0x00;
	MAIN_FM_MODE(); 			// shifting the access of SFM to OBC
	TEMP_ADDR = ADDRESS;
	while (size > PAGE_SIZE) {
		Bulk_Read_4B(&SMSN_FM, TEMP_ADDR, TEMP, 256);
		Page_Write_4B(&MAIN_FM, TEMP_ADDR, TEMP, 256);
		HAL_Delay(50);
		size -= PAGE_SIZE;
		TEMP_ADDR += 256;
		memset(TEMP, '\0', PAGE_SIZE);
	}

	Bulk_Read_4B(&SMSN_FM, TEMP_ADDR, TEMP, size);
	Page_Write_4B(&MAIN_FM, TEMP_ADDR, TEMP, size);
	HAL_Delay(50);
	TEMP_ADDR += size;
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
	myprintf("MSN data copied from SFM to MFM, starting from %08x to %08x.\r\n",
			ADDRESS, TEMP_ADDR);
	HAL_Delay(1);
#else
	#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE
}

/*
 * @brief transfers data from specified address to COM for downlink
 *
 * @param	SPI		pointer to SPI handle of Flash Memory from where data is to be downlinked
 * 			ADDRESS	starting address from where data is to be downlinked
 * 			PKT		no of packets of data to be downlinked.	[MAX 255 PKT at a time]
 * @retval	none
 *
 */
void TRANSFER_DATA_COM(SPI_HandleTypeDef *SPI, uint32_t ADDRESS, uint8_t PKT) {
	uint8_t DATA_BUFFER[PACKET_DATA_LEN];
	uint32_t temp_addr = ADDRESS;
	memset(DATA_BUFFER, '\0', PACKET_DATA_LEN);
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Download Address: %08x PCKT_NUM:%i\r\n", ADDRESS, PKT);
#endif
#endif

#ifdef S_COM_PLAN_F_MODE
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Shifting COM system to Transmission Mode.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
	S_COM_TX_MODE();
	HAL_Delay(500);
#endif //S_COM_PLAN_F_MODE

	for (int i = 0; i < PKT; i++) {
		BEACON_FLAG =0;
		HK_FLAG=0;
		Bulk_Read_4B(SPI, temp_addr, DATA_BUFFER, PACKET_DATA_LEN);
#ifdef ADDNICS_COM_MODE
		COM_Transmit_Data(DATA_BUFFER, PACKET_DATA_LEN, i);
#endif //ADDNICS_COM_MODE

#ifdef S_COM_PLAN_F_MODE
		COM_Transmit(DATA_BUFFER, PACKET_DATA_LEN, i);
#endif	//S_COM_PLAN_F_MODE
//
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
//		for (int j = 0; j < PACKET_DATA_LEN; j++) {
//			myprintf("%02x ", DATA_BUFFER[j]);
//		}
//		myprintf("\r\n");
		myprintf("Data Packet %i transfer Complete.......\r\n", i + 1);
#endif
#endif
		memset(DATA_BUFFER, '\0', PACKET_DATA_LEN);
		temp_addr += PACKET_DATA_LEN;
		HAL_Delay(150);
	}

	Write_Address_Data();
#ifdef S_COM_PLAN_F_MODE
#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
	myprintf("Shifting COM system to Receiver Mode.\r\n");
#endif	//TRACE_MODE
#endif	//DEBUG_MODE
	S_COM_RX_MODE();
#endif	//S_COM_PLAN_F_MODE

	if (temp_addr == (ADDRESS + PKT * PACKET_DATA_LEN)) {
#ifdef DEBUG_MODE
#ifndef TRACE_MODE
		myprintf("Data transmitted with correct address count.\r\n");
#else
		#endif //ifndef TRACE_MODE
#endif //ifdef DEBUG_MODE

	} else {

#ifdef DEBUG_MODE
#ifdef TRACE_MODE
#else
		myprintf("DATA transmitted, but address count wrong.\r\n");
#endif //TRACE_MODE
#endif //DEBUG_MODE

	}

}

#ifdef ACTIVATE_INT_FLASH
/*
 * @brief Erases the internal flash memory of the MCU
 *
 * @param	SECTOR_NUM
 * @retval	none
 *
 * @note This function is configured to erase the sectors if MCU operating in 3.3 V
 *
 */
void ERASE_INTERNAL_FLASH(uint32_t SECTOR_NUM) {
	HAL_FLASH_Unlock();

	FLASH_Erase_Sector(SECTOR_NUM, FLASH_VOLTAGE_RANGE_3);

	HAL_FLASH_Lock();
}

/*
 * @brief Writes the assigned data buffer to the internal flash of MCU
 *
 * @param	ADDRESS			Address where data is to be written
 * 			SECTOR			Sector where address lies
 * 			DATA_LEN		length of data to be written
 * 			DATA			pointer to the location containing data to be written
 *
 *
 * @retval HAL_StatusTypeDef	status of write operation
 *
 */
HAL_StatusTypeDef WRITE_TO_INTERNAL_FLASH(uint32_t sector,
		uint32_t write_address, uint8_t *DATA, uint8_t data_len) {
	HAL_StatusTypeDef OPER_STAT;
	uint8_t buff[data_len];
	for (int i = 0; i < data_len; i++) {
		buff[i] = *DATA++;
	}

	HAL_FLASH_Unlock();

	for (int i = 0; i < data_len; i++) {
		OPER_STAT = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, write_address,
				(uint8_t) buff[i]);
		HAL_Delay(10);
		if (OPER_STAT != HAL_OK) {
			HAL_FLASH_Lock();
			HAL_Delay(10);
			return OPER_STAT;
		}
		write_address++;
	}
	HAL_FLASH_Lock();
	return OPER_STAT;
}

/*
 * @brief Reads data from given address of the internal flash
 *
 * @param	Address		Address from where data is to be read
 * 			Read_Buffer	pointer to the location where address is to be written
 * 			len			no of bytes to be read
 * @retval	none
 *
 */
void READ_FRM_INTERNAL_FLASH(uint32_t address, uint8_t *read_buff, uint32_t len) {

	for (int i = 0; i < len; i++) {
		*(read_buff + i) = *(uint8_t*) address;
		address++;
	}

}

#endif
