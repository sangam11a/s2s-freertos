/*
 * packet_code.c
 *
 *  Created on: Nov 1, 2022
 *      Author: DELL
 */
#include "packet_code.h"

// variable declaration
uint8_t Ack_pckt[33];
uint8_t CMD_pckt[33] = { 0 };
uint16_t checksum;
uint8_t buffer;
uint8_t data_pckt[] = { 0x7E, 0x4D, 0x55, 0x4E, 0x41, 0x4C, 0x31, 0x30, 0x4D,
		0x55, 0x4E, 0x41, 0x4C, 0x31, 0x30, 0x3E, 0xF0 };	   //
//M    U   N    A    L

//successful acknowledge packet
void send_success_ack() { // not completed
	//header
	Ack_pckt[0] = 0x7E;

	//    Ground stationCall sign---------------[ 1-----6 ]
	Ack_pckt[1] = 0x4D;     	// M
	Ack_pckt[2] = 0x55;     	// U
	Ack_pckt[3] = 0x4E;     	// N
	Ack_pckt[4] = 0x41;     	// A
	Ack_pckt[5] = 0x4C;    		// L
	Ack_pckt[6] = 0x31;     	// 1

	//    Dest SSID-----------------------------[ 7 ]
	Ack_pckt[7] = 0x30; //char 0 An SSID (Secondary Station Identifier) is a number following a station's callsign.  The numbers range from -0 to -15 and are used to identify the type of packet service the station is running.0=>home station, 1=>>
						//1 Home station personal mailbox (usually a TNC-based PBBS).2 Gateways
	//    Satellite Call Sign-------------------[ 8-----13 ]
	Ack_pckt[8] = 0x4D;     	// M
	Ack_pckt[9] = 0x55;     	// U
	Ack_pckt[10] = 0x4E;     	// N
	Ack_pckt[11] = 0x41;     	// A
	Ack_pckt[12] = 0x4C;     	// L
	Ack_pckt[13] = 0x31;     	// 1

	//    Source SSID---------------------------[ 14 ]
	Ack_pckt[14] = 0x30;    //char 0

	//    Control-------------------------------[ 15 ]The control field identifies the type of frame being passed and controls several attributes of the Layer 2 connection. It is one or two octets in length
	Ack_pckt[15] = 0x3E;    //    Implies (I) Information Frame

	//    PID-----------------------------------[ 16 ] The Protocol Identifier (PID) field appears in information frames (I and UI) only. It identifies which kind of Layer 3 protocol, if any, is in use.
	Ack_pckt[16] = 0xF0;    ////11110000   => no layer 3 protocol implemented

	//    Header--------------------------------[ 17 ]
	Ack_pckt[17] = 0xAA;    //

	//    SAT Header----------------------------[ 18]
	Ack_pckt[18] = 0x03; //   Let's move on to the ETX character. The End-of-Text character (ETX) (hex value of 0x03, frequently showed as ^C) is an ASCII control character. Its function is to let the receiving computer know that the end of the data stream has been reached. This is an indicator, but not necessarily the stated fact that all of the data has been received.
	//    Pckt sequence number higher byte------[ 19 ]
	Ack_pckt[19] = CMD_pckt[1];    //

	//    Pckt sequence number lower byte-------[ 20 ]
	Ack_pckt[20] = CMD_pckt[2];    //

	//    CMD back------------------------------[ 21 - 29 ]
	Ack_pckt[21] = CMD_pckt[3];		//0
	Ack_pckt[22] = CMD_pckt[4];		//0
	Ack_pckt[23] = CMD_pckt[5];		//0
	Ack_pckt[24] = CMD_pckt[6];		//0
	Ack_pckt[25] = CMD_pckt[7];		//0
	Ack_pckt[26] = CMD_pckt[8];		//0
	Ack_pckt[27] = CMD_pckt[9];		//0
	Ack_pckt[28] = CMD_pckt[10];		//0
	Ack_pckt[29] = CMD_pckt[11];		//0

	// crc_calculation
	find_crc(Ack_pckt, 30); // will fill checksum in 30 and 31

	//    Footer--------------------------------[ 29 ]
	Ack_pckt[32] = 0xAA;    //
	//transmitting
}

// failed acknowledgement packet-------------------------------//
void send_failed_ack() { // not completed
	//header
	Ack_pckt[0] = 0x7E;

	//    Ground stationCall sign---------------[ 1-----6 ]
	Ack_pckt[1] = 0x4D;     	// M
	Ack_pckt[2] = 0x55;     	// U
	Ack_pckt[3] = 0x4E;     	// N
	Ack_pckt[4] = 0x41;     	// A
	Ack_pckt[5] = 0x4C;    		// L
	Ack_pckt[6] = 0x31;     	// 1

	//    Dest SSID-----------------------------[ 7 ]
	Ack_pckt[7] = 0x30; //char 0 An SSID (Secondary Station Identifier) is a number following a station's callsign.  The numbers range from -0 to -15 and are used to identify the type of packet service the station is running.0=>home station, 1=>>
						//1 Home station personal mailbox (usually a TNC-based PBBS).2 Gateways
	//    Satellite Call Sign-------------------[ 8-----13 ]
	Ack_pckt[8] = 0x4D;     	// M
	Ack_pckt[9] = 0x55;     	// U
	Ack_pckt[10] = 0x4E;     	// N
	Ack_pckt[11] = 0x41;     	// A
	Ack_pckt[12] = 0x4C;     	// L
	Ack_pckt[13] = 0x31;     	// 1

	//    Source SSID---------------------------[ 14 ]
	Ack_pckt[14] = 0x30;    //char 0

	//    Control-------------------------------[ 15 ]The control field identifies the type of frame being passed and controls several attributes of the Layer 2 connection. It is one or two octets in length
	Ack_pckt[15] = 0x3E;    //    Implies (I) Information Frame

	//    PID-----------------------------------[ 16 ] The Protocol Identifier (PID) field appears in information frames (I and UI) only. It identifies which kind of Layer 3 protocol, if any, is in use.
	Ack_pckt[16] = 0xF0;    ////11110000   => no layer 3 protocol implemented

	//    Header--------------------------------[ 17 ]
	Ack_pckt[17] = 0xAA;    //

	//    SAT Header----------------------------[ 18]
	Ack_pckt[18] = 0x03; //   Let's move on to the ETX character. The End-of-Text character (ETX) (hex value of 0x03, frequently showed as ^C) is an ASCII control character. Its function is to let the receiving computer know that the end of the data stream has been reached. This is an indicator, but not necessarily the stated fact that all of the data has been received.
	//    Pckt sequence number higher byte------[ 19 ]
	Ack_pckt[19] = CMD_pckt[1];    //

	//    Pckt sequence number lower byte-------[ 20 ]
	Ack_pckt[20] = CMD_pckt[2];    //

	//    CMD back------------------------------[ 21 - 29 ]
	//    CMD back------------------------------[ 20 - 28 ]
	Ack_pckt[20] = 0x69;		//i
	Ack_pckt[21] = 0x69;
	Ack_pckt[22] = 0x69;
	Ack_pckt[23] = 0x69;
	Ack_pckt[24] = 0x69;
	Ack_pckt[25] = 0x69;
	Ack_pckt[26] = 0x69;
	Ack_pckt[27] = 0x69;
	Ack_pckt[28] = 0x69;

	// crc_calculation
	find_crc(Ack_pckt, 30); // will fill checksum in 30 and 31;

	//    Footer--------------------------------[ 29 ]
	Ack_pckt[32] = 0xAA;    //
}

// the arrangement of the packet for transmitting telemetry data (function is incomplete)
void send_data_pckt(uint32_t address, uint32_t size, uint32_t packets) {
	uint32_t num_of_packets = 102;
	uint32_t pckt_seq_number;
	uint8_t data_pckt[100];

	for (pckt_seq_number = 0; pckt_seq_number < num_of_packets;
			pckt_seq_number++) {
		//=============================Header===============================
		data_pckt[0] = 0x7E;

		//===============s==Ground station Call sign part=============================
		//========================= 1 --- 6 ========================================
		data_pckt[1] = 0x4D;     	// M
		data_pckt[2] = 0x55;     	// U
		data_pckt[3] = 0x4E;     	// N
		data_pckt[4] = 0x41;     	// A
		data_pckt[5] = 0x4C;    	// L
		data_pckt[6] = 0x31;     	// 1
		//=========================Dest SSID========================================
		//=========================   7     ========================================
		data_pckt[7] = 0x30;     //char 0
		//===================Satellite Call Sign====================================
		//========================= 8 --- 13  ======================================
		data_pckt[8] = '9';     	// M
		data_pckt[9] = 'N';     	// U
		data_pckt[10] = 'X';     	// N
		data_pckt[11] = 'X';     	// A
		data_pckt[12] = 'X';    	// L
		data_pckt[13] = 'X';     	// 1
		//========================Source SSID=======================================
		//=========================   14   =========================================
		data_pckt[14] = 0x30;    //char 0
		//==========================Control=========================================
		//=========================   15   =========================================
		data_pckt[15] = 0x3E;	   //
		//============================PID===========================================
		//=========================   16   =========================================
		data_pckt[16] = 0xF0;	   //

		// form data_pckt[17] to data_pckt[103] is info

		//crc value 104--105

//		findcrc

//		footer = 0x7E;
		data_pckt[99] = 0x7E;
	}
}

//retieve command from receive packet
void filter_CMD(uint8_t *packet, uint8_t *CMD) {
	int i;
	for (i = 0; i < 4; i++) {
		CMD[i] = packet[17 + i];
	}
}

//arrange packets
void arrange_tx_packets(uint8_t *info, uint8_t *destination) {
	uint8_t i;
	destination[0] = 0x7e;
	//call sign--
	destination[1] = 'M';
	destination[2] = 'U';
	destination[3] = 'N';
	destination[4] = 'A';
	destination[5] = 'L';
	destination[6] = '1';
	destination[7] = 0x00;
	destination[8] = 'M';
	destination[9] = 'U';
	destination[10] = 'N';
	destination[11] = 'A';
	destination[12] = 'L';
	destination[13] = '1';
	destination[14] = 0x00;
	//CONTROL
	destination[15] = 0x3e;
	//PID
	destination[16] = 0xf0;

	//info
	for (i = 0; i < 40; i++) {
		destination[i + 17] = info[i];
	}

	// checksum
	find_crc(destination, 57);

	//footer
	destination[59] = 0x7e;

}
uint8_t simple_csk(uint8_t *RX_CMD) {
	uint16_t csk = 0;
	for (int i = 0; i <= 33; i++) {
		csk = csk + RX_CMD[i];

	}
	return csk;
}
uint8_t Check_CMD(uint8_t *RX_CMD) {
	for (int i = 0; i <= 33; i++) {
		RX_CMD[0] = 0x7e;
	}
	return 0;
}

uint8_t Get_GS_CMD() {
	MainToSubSys(&COM_TCV_STREAM, CMD_pckt, 33, 1);
	Check_CMD(CMD_pckt);
	return 0;
}
