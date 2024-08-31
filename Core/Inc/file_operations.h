/*
 * file_operations.h
 *
 *  Created on: Aug 31, 2024
 *      Author: sanga
 */

#ifndef INC_FILE_OPERATIONS_H_
#define INC_FILE_OPERATIONS_H_

#include "../file_operations/header_files/lfs.h"
#include "../file_operations/header_files/lfs_util.h"
#include "main.h"
#include "../file_operations/header_files/nor.h"
#include "stdarg.h"

#define sector_size 32

#define PATH_MAX_LEN 256
extern SPI_HandleTypeDef hspi3, hspi4, hspi5, hspi2;
extern UART_HandleTypeDef huart7;
extern nor_t Nor;

void read_file_from_littlefs(lfs_t *lfs, const char *filename);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void nor_delay_us(uint32_t us);
void nor_cs_assert();
void nor_cs_deassert();
void nor_spi_tx(uint8_t *pData, uint32_t Size);
void nor_spi_rx(uint8_t *pData, uint32_t Size);
void __init_nor();
int _fs_read(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size);
int _fs_write(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size);
int _fs_erase(const struct lfs_config *c, lfs_block_t block);
int _fs_sync(const struct lfs_config *c);
void list_files(lfs_t *lfs);
void __init_littefs();

#endif /* INC_FILE_OPERATIONS_H_ */
