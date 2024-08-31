/*
 * packet_code.h
 *
 *  Created on: Nov 1, 2022
 *      Author: DELL
 */
#pragma once

#ifndef INC_PACKET_CODE_H_
#define INC_PACKET_CODE_H_

//#include "../../COM/packet_code.h"

#include "main.h"
#include "../../COM/error_detection.h"
//#include "../../COM/morse.h"
#include "../FLASH_MEMORY/MT25QL.h"
#include "../COMMON_FUNCTIONS/USR_FUNCTIONS.h"



void send_success_ack(); 	//not completed
void send_failed_ack();		//not completed
void filter_CMD(uint8_t *packet, uint8_t *CMD);
void send_data_pckt(uint32_t address, uint32_t size, uint32_t packets);
void arrange_tx_packets(uint8_t *info, uint8_t *destination);
uint8_t Check_CMD(uint8_t *RX_CMD);

#endif /* INC_PACKET_CODE_H_ */
