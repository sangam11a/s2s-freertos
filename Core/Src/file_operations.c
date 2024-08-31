/*
 * file_operations.c
 *
 *  Created on: Aug 31, 2024
 *      Author: sanga
 */

#include "file_operations.h"
void read_file_from_littlefs(lfs_t *lfs, const char *filename) {
    lfs_file_t file;
    HAL_UART_Transmit(&huart7, filename,sizeof(filename),1000);
    // Open the file for reading
    int err = lfs_file_open(lfs, &file, filename, LFS_O_RDONLY);
    if (err < 0) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    // Get the file size
    lfs_soff_t file_size = lfs_file_size(lfs, &file);
    if (file_size < 0) {
        printf("Failed to get file size for: %s\n", filename);
        lfs_file_close(lfs, &file);
        return;
    }

    // Allocate a buffer to hold the file data
    float *buffer = malloc(file_size);
    if (buffer == NULL) {
        printf("Failed to allocate buffer for reading file: %s\n", filename);
        lfs_file_close(lfs, &file);
        return;
    }

    // Read the file content into the buffer
    lfs_ssize_t bytes_read = lfs_file_read(lfs, &file, buffer, file_size);
    if (bytes_read < 0) {
        printf("Failed to read file: %s\n", filename);
    } else {
    	char x;
        // Successfully read the file, print its content (if it's text data)
        HAL_UART_Transmit(&huart7, buffer, (int)bytes_read,1000);
        for(int i=0;i<(int) bytes_read;){
//        	printf(buffer[i]);
        	x=buffer[i];
        	i++;
        }


        printf("File Content (%s):\n%.*s\n", filename, (int)bytes_read, buffer);
    }

    // Clean up
    free(buffer);
    lfs_file_close(lfs, &file);
}

volatile uint8_t DmaEnd = 0;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
	DmaEnd = 1;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	DmaEnd = 1;
}

void nor_delay_us(uint32_t us){
//	if (us >= __HAL_TIM_GET_AUTORELOAD(&htim2)){
//		us = __HAL_TIM_GET_AUTORELOAD(&htim2) - 1;
//	}
//	__HAL_TIM_SET_COUNTER(&htim2, 0);
//	HAL_TIM_Base_Start(&htim2);
//	while (__HAL_TIM_GET_COUNTER(&htim2) < us);
//	HAL_TIM_Base_Stop(&htim2);
	HAL_Delay(1000);
}

void nor_cs_assert(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
}

void nor_cs_deassert(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
}

void nor_spi_tx(uint8_t *pData, uint32_t Size){
//	HAL_SPI_Transmit(&hspi3, pData, Size, 100);
	DmaEnd = 0;
	HAL_SPI_Transmit(&hspi3, pData, Size, 1000);
//	while (DmaEnd == 0);
}

void nor_spi_rx(uint8_t *pData, uint32_t Size){
//	HAL_SPI_Receive(&hspi3, pData, Size, 100);
	DmaEnd = 0;
	HAL_SPI_Receive(&hspi3, pData, Size, 1000);
//	DmaEnd =0;
//	while (DmaEnd == 0);
}

void __init_nor(){
	Nor.config.CsAssert = nor_cs_assert;
	Nor.config.CsDeassert = nor_cs_deassert;
	Nor.config.DelayUs = nor_delay_us;
	Nor.config.SpiRxFxn = nor_spi_rx;
	Nor.config.SpiTxFxn = nor_spi_tx;

	if (NOR_Init(&Nor) != NOR_OK){ //NOR_Init
		Error_Handler();
	}
}

/** Start LittleFs **/

int _fs_read(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size){

	if (NOR_ReadSector(&Nor, (uint8_t*)buffer, block, off, size) == NOR_OK){
		return 0;
	}

	return LFS_ERR_IO;
}

int _fs_write(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size){

	if (NOR_WriteSector(&Nor, (uint8_t*)buffer, block, off, size) == NOR_OK){
		return 0;
	}

	return LFS_ERR_IO;
}

int _fs_erase(const struct lfs_config *c, lfs_block_t block){
	if (NOR_EraseSector(&Nor, block) == NOR_OK){
		return 0;
	}

	return LFS_ERR_IO;
}

int _fs_sync(const struct lfs_config *c){
	return 0;
}

// Function to list all files and directories in the filesystem
void list_files(lfs_t *lfs) {
    lfs_dir_t dir;
    struct lfs_info info;

    // Open the root directory
    int err = lfs_dir_open(lfs, &dir, "/");
    if (err) {
        printf("Failed to open directory\n");
        return;
    }

    // Loop through all files in the directory
    while (true) {
        err = lfs_dir_read(lfs, &dir, &info);
        if (err < 0) {
            printf("Failed to read directory\n");
            break;
        }

        // If no more files, break
        if (err == 0) {
            break;
        }
        uint8_t dir[100];
        // Print the type and name of the file
        if (info.type == LFS_TYPE_REG) {
            sprintf(dir,"File: %s\n\0", info.name);
            HAL_UART_Transmit(&huart7, dir, strlen(dir),1000);
        } else if (info.type == LFS_TYPE_DIR) {
        	sprintf(dir,"Directory: %s\n\0", info.name);

            HAL_UART_Transmit(&huart7, dir, strlen(dir),1000);
        }
    }

    // Close the directory
    lfs_dir_close(lfs, &dir);
}


