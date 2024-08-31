/*
 * error_detction.c
 *
 *  Created on: Nov 2, 2022
 *      Author: DELL
 */
#include "main.h"
#include "math.h"
//find crc_16
//leave 2 bytes in the given data to add crc in the given data
void find_crc(uint8_t *data, uint8_t index_of_checksum) {
	uint32_t crc, ce;
	uint8_t count;
	crc = 0;
	uint32_t poly = 0b1010011010100110;
	poly = poly << 8; //shifting 8 bits forward (4096 = 0b 1 0000 0000)
	count = 0;
	data[index_of_checksum] = 0x00;
	data[index_of_checksum+1] = 0x00;
	int8_t a;
	uint8_t flag;

	crc = data[0];
	crc = crc << 8;
	crc = crc | data[1];
	crc = crc << 8;
	crc = crc | data[2];

	//dividing until the first 1  of data is parallel to 1st 1 of polymonial
	for (flag = 3; flag < index_of_checksum + 3; flag++) {
		for (; crc > 0x10000;) {
			if (crc < 0x20000) {
				a = 7 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x40000) {
				a = 6 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x80000) {
				a = 5 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x100000) {
				a = 4 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x200000) {
				a = 3 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x400000) {
				a = 2 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x800000) {
				a = 1 - count;
				poly = poly / pow(2, a);
				count = count + a;
			} else if (crc < 0x1000000) {
				a = 0 - count;
				poly = poly / pow(2, a);
				count = count + a;
			}

			crc = crc ^ poly;

		}
		ce = crc;
		crc = crc << 8;
		crc = crc | data[flag];

	}

	data[index_of_checksum + 1] = ce;
	data[index_of_checksum] = ce >> 8;
	crc = 0;
	a = 0;
}
// Compare checksum when satellite receive command
//return 1 when no error is found iin checksum
//else return 0
uint8_t cmp_checksum_of_CMD(uint8_t *data) {
	find_crc(data, 21);

	if ((data[21] == 0) && (data[22] == 0)) {
		return 1;
	}

	else {
		return 0;
	}
}

uint8_t cmp_checksum_packets(uint8_t *data) {
	find_crc(data, 57);

	if ((data[58] == 0) && (data[58] == 0)) {
		return 1;
	}

	else {
		return 0;
	}
}
