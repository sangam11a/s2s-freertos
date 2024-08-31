/*
 * lfs_port.h
 *
 *  Created on: Aug 8, 2024
 *      Author: sanga
 */

#ifndef ../FILE_OPERATIONS/HEADER_FILES/LFS_PORT_H_
#define ../FILE_OPERATIONS/HEADER_FILES/LFS_PORT_H_


#include "MT25QL.h"
#include "lfs.h"
//typedef struct {
//	uint8_t MAN_ID;
//	uint8_t M_TYPE;
//	uint8_t M_CAP;
//	uint8_t REM_BYTES;
//	uint8_t EXT_ID;
//	uint8_t DEV_INFO;
//	uint8_t UID[14];
//}DEVICE_ID;

#define W25Q32_SECTOR_SIZE 4096
 #define W25Q32_sector_num 1024 // Total size: 4096 * 1024 = 4M Byte
#define W25Q32_SECTOR_NUM 34

#define MT25QL_SECTOR_SIZE     65536    // 64KB per sector
#define MT25QL_SUBSECTOR_32KB  32768    // 32KB subsector
#define MT25QL_SUBSECTOR_4KB   4096     // 4KB subsector

#define MT25QL_TOTAL_SECTORS   2048     // Total sectors in 1Gb memory
#define MT25QL_TOTAL_32KB_SUBSECTORS (MT25QL_TOTAL_SECTORS * 2) // Each sector has 2 subsectors of 32KB
#define MT25QL_TOTAL_4KB_SUBSECTORS  (MT25QL_TOTAL_SECTORS * 16) // Each sector has 16 subsectors of 4KB

int lfs_spi_flash_init(struct lfs_config *cfg);
int lfs_spi_flash_read(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_spi_flash_prog(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_spi_flash_erase(const struct lfs_config *cfg, lfs_block_t block);
int lfs_spi_flash_sync(const struct lfs_config *cfg);

#endif // __LFS_PORT_H

//#endif /* INC_LFS_PORT_H_ */
