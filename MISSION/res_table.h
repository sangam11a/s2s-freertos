/*
 * res_table.h
 *
 *  Created on: Jan 7, 2023
 *      Author: Sushant
 */

#pragma once

#ifndef RES_TABLE_H_
#define RES_TABLE_H_

//#include "variables.h"
#include "../COMMON_FUNCTIONS/USR_FUNCTIONS.h"


void Print_RSV_Table();
void SAVE_RSV();
uint8_t Get_RSV_Number();
void sort_table();
void Get_RSV_From_Flash();
uint8_t Reservation_Command(uint8_t CMD1, uint8_t CMD2, uint8_t CMD3,
		uint8_t CMD4, uint8_t TIME);
void Delete_Executed_RSV_Data();

#endif /* RES_TABLE_H_ */
