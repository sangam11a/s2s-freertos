/*
 * command_parser.h
 *
 *  Created on: Aug 30, 2024
 *      Author: sanga
 */

#ifndef INC_COMMAND_PARSER_H
#define INC_COMMAND_PARSER_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "../file_operations/header_files/lfs.h"

  lfs_t lfs;
	  lfs_file_t file;
enum MCU_IDS
{
  OBC_MCU = 0x01,
  COM_MCU = 0x02,
  ADCS_MCU = 0x03,
  CAM_MCU = 0x04,
  EPDM_MCU = 0x05
};
extern UART_HandleTypeDef huart1;
#define BEACON_DATA_SIZE 85
#define COM_RX_CMD_SIZE 33
#define COM_uart huart1
#define GPIO_COM_4V_EN_PORT GPIOH
#define GPIO_COM_4V_EN_PIN GPIO_PIN_14
#define GPIO_DCDC_4V_EN_PORT GPIOI
#define GPIO_DCDC_4V_EN_PIN GPIO_PIN_4
//#define FM_MAIN_STRPATH
//#define FM_MSN_STRPATH
//#define SFM_MAIN_STRPATH
//#define SFM_MSN_STRPATH

/*
 *
@brief
clear/empty or truncate the file
Two arguments are to be supplied:
  1.select_flash : determines the flash memory to be selected or from where the data is to be deleted
  2.select_file :  determines the name of the text file which is to be emptied

  Requirement: An enumeration named select_flash is to be defined with two components
*/
enum SELECT_FLASH
{
  MAIN_FLASH_MEMORY,
  SHARED_FLASH_MEMORY,
  INTERNAL_FLASH
};
/*
@brief
This enumeration is created for easy implementation to select required files.
*/
enum SELECT_FILE
{
  FLAGS,
  SATELLITE_HEALTH,
  SATELLITE_LOG,
  RESERVATION_TABLE,
  CAMERA_TXT,
  EPDM_TXT,
  ADCS_TXT
};
/*
@brief:
cmd: main command,
select_file: enum to select the required text file,
select_flash: enum to select the required partition to read the text file,
rsv_table: this contains the information how lately this command is to be executed i.e. time latency info
filepath: the full path to the textfile: MOUNT_POINT/TEXT_FILENAME
address: 4 byte of adress data here it might be the data download counter
number of packets: may be DNC(0x00,0x00) or number of packets in 2 bytes.
*/

struct FILE_OPERATIONS
{
  uint8_t cmd;
  enum SELECT_FILE select_file;
  enum SELECT_FLASH select_flash;
  uint8_t rsv_table[2];
  char filepath[200];
  uint8_t address[4];
  uint8_t number_of_packets[2];
};

typedef struct
{
  uint8_t SATELLITE_HEALTH_1; // SAT HEALTH POINTER status
  uint8_t SATELLITE_HEALTH_2; // SAT HEALTH POINTER status

  uint8_t MSN1_DATA_1; // MSN1 DATA POINTER status
  uint8_t MSN1_DATA_2; // MSN1 DATA POINTER status

  uint8_t MSN2_DATA_1; // MSN2 DATA POINTER status
  uint8_t MSN2_DATA_2; // MSN2 DATA POINTER status

  uint8_t MSN3_DATA_1; // MSN3 DATA POINTER status
  uint8_t MSN3_DATA_2; // MSN3 DATA POINTER status
                       // to make sure data is stored in internal flash
} SEEK_POINTER;

void parser();
int receive_telecommand_rx(uint8_t *COM_RX_DATA, lfs_t Lfs1, lfs_file_t File1);
void parse_command(uint8_t COM_RX_DATA[30], lfs_t Lfs1, lfs_file_t File1);
void download_file_from_flash(struct FILE_OPERATIONS *file_operations, uint8_t *data_retrieved, uint8_t size_of_buffer,  lfs_t Lfs, lfs_file_t File);
void perform_file_operations(struct FILE_OPERATIONS *file_operations, lfs_t Lfs1, lfs_file_t File1);
void track_read_seek_pointer(struct FILE_OPERATIONS *file_pointer, int8_t seek_pointer[16]);
void truncate_text_file( struct FILE_OPERATIONS *file_operations, lfs_t Lfs1, lfs_file_t File1);
void create_empty_files();
#endif /* INC_COMMAND_PARSER_H_ */
