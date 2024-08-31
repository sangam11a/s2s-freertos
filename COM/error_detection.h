/*
 * error_detection.h
 *
 *  Created on: Nov 2, 2022
 *      Author: DELL
 */

#ifndef ERROR_DETECTION_H_
#define ERROR_DETECTION_H_
#include "stdio.h"
#include "main.h"

void find_crc(uint8_t *data, uint8_t index_of_checksum);
uint8_t cmp_checksum_of_CMD(uint8_t *data);
uint8_t cmp_checksum_packets(uint8_t *data);


#endif /* ERROR_DETECTION_H_ */
