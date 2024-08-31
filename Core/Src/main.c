/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "stdarg.h"
//#include "lfs_util.h"
//#include "lfs.h"
//#include "MT25Q.h"
//#include "nor.h"
#include "../../IMU/imu.h"
#include "command_parser.h"
#include "file_operations.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define sector_size 32

#define PATH_MAX_LEN 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//void myprintf(const char *fmt, ...);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;

SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi4;

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
//uint8_t tx[]={'S','A','N','G','A','M'};
HAL_StatusTypeDef catch;
uint8_t HANDSHAKE_STATUS = 0;
uint8_t COM_RX_DATA[30];
uint16_t adc_buf[17];
float adc_conv_buf[17];

uint8_t hk_counter=0;
uint16_t tim_val = 0;
uint16_t address = 0x00;
uint8_t status_reg=0;
uint8_t READ_FLAG=0;
int tx[70];

uint8_t DEBUG_DATA_RX_FLAG = 0;
// variables used by the filesystem
typedef struct{
	uint32_t secCount;
	uint32_t bootCount;
}app_count_t;
	lfs_file_t File,File2[2];
		char Text[20];
		app_count_t Counter = {0};
		lfs_t Lfs,Lfs2[2];
		nor_t Nor;
		uint16_t adc_channels[17] = {'66535'};
		uint16_t adc1_channels[17] = {'66535'};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

void __init_littefs(){
	// because of static qualifier, this variable
	// will have a dedicated address
	static struct lfs_config LfsConfig = {0};
		int Error;

		LfsConfig.read_size = 256;
		LfsConfig.prog_size = 256;
		LfsConfig.block_size = Nor.info.u16SectorSize;
		LfsConfig.block_count =  16384;//Nor.info.u32SectorCount;
		LfsConfig.cache_size = Nor.info.u16PageSize;
		LfsConfig.lookahead_size = 256;//Nor.info.u32SectorCount/8;
		LfsConfig.block_cycles = 15000;

		LfsConfig.read = _fs_read;
		LfsConfig.prog = _fs_write;
		LfsConfig.erase = _fs_erase;
		LfsConfig.sync = _fs_sync;

		Error = lfs_mount(&Lfs, &LfsConfig);
		if (Error != LFS_ERR_OK){
			lfs_format(&Lfs, &LfsConfig);
			Error = lfs_mount(&Lfs, &LfsConfig);
			if (Error != LFS_ERR_OK){
				Error_Handler();
			}
		}

}

// Recursive function to list files and directories with full paths
void list_files_with_size(lfs_t *lfs, const char *path) {
    lfs_dir_t dir;
    struct lfs_info info;

    // Open the directory at the given path
    int err = lfs_dir_open(lfs, &dir, path);
    if (err) {
        printf("Failed to open directory: %s\n", path);
        return;
    }

    // Loop through all files in the directory
    while (true) {
        err = lfs_dir_read(lfs, &dir, &info);
        if (err < 0) {
            printf("Failed to read directory: %s\n", path);
            break;
        }

        // If no more files, break
        if (err == 0) {
            break;
        }

        // Build the full path for the current file/directory
        char full_path[PATH_MAX_LEN];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, info.name);
        char pa[500];
        // Check if the entry is a file or directory
        if (info.type == LFS_TYPE_REG) {
            sprintf(pa,"File: %s, Size: %ld bytes\n", full_path, info.size);
            HAL_UART_Transmit(&huart7, pa, strlen(pa),1000);
        } else if (info.type == LFS_TYPE_DIR && strcmp(info.name, ".") != 0 && strcmp(info.name, "..") != 0) {
            sprintf(pa,"Directory: %s\n", full_path);
            HAL_UART_Transmit(&huart7, pa, strlen(pa),1000);
            // Recursively list the contents of the directory
            list_files_with_size(lfs, full_path);
        }
    }

    // Close the directory
    lfs_dir_close(lfs, &dir);
}
void __init_storage(){
	__init_nor();
	__init_littefs();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // Check which version of the timer triggered this callback and toggle LED
  if (htim == &htim6 )
  {
//    HAL_UART_Transmit(&huart7, "\nCounter is : ", sizeof("\nCounter is : "), 100);
    tim_val++;
//    char uart_buf[20] ;
//    sprintf(uart_buf, "%u us\0\r\n", tim_val);
//    HAL_UART_Transmit(&huart7, uart_buf, sizeof(uart_buf), 100);

  }
  if(htim == &htim7){
	  hk_counter++;
	  if(hk_counter%30 == 0){ char uart_buf[20] ;
		  sprintf(uart_buf, "%u us\0\r\n", tim_val);
		  HAL_UART_Transmit(&huart7, uart_buf, sizeof(uart_buf), 100);
		  HK_IMU();

		  HAL_UART_Transmit(&huart7, "\n-----------------------------Collected HK DATA-----------n", sizeof("\n-----------------------------Collected HK DATA-----------n"), 100);
		  hk_counter=0;
		  RUN_ADC2();
		  tim_val = 0 ;
	  }

  }
}

void RUN_ADC2(){
	HAL_UART_Transmit(&huart7,"\n\n********************S2S ADC is starting ************\n", sizeof("********************S2S ADC is starting ************\n"),1000);

			ADC_Operate(adc_channels, adc1_channels);
			 ADC_Conv_Data(adc_conv_buf, adc_buf);
			for(int i =0; i<sizeof(adc_channels); i++)
			myprintf("%d | %d **", adc_channels[i],adc1_channels[i]);
			//		HK_IMU();
			HAL_Delay(10000);
			HAL_UART_Transmit(&huart7,"\n\n********************S2S ADC data collection complete ************\n", sizeof("********************S2S ADC data collection complete************\n"),1000);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//			if(huart->Instance == &huart1)
			{
				printf("Data received on COM UART1\n");
				parse_command(&COM_RX_DATA,Lfs, File);

			    HAL_UART_Receive_DMA(&COM_uart, COM_RX_DATA, sizeof(COM_RX_DATA));
			}
		}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	  char uart_buf[50];
	  int uart_buf_len;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI3_Init();
  MX_UART7_Init();
  MX_SPI4_Init();
  MX_ADC1_Init();
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_ADC3_Init();
  MX_TIM7_Init();
  MX_ADC2_Init();
  MX_UART8_Init();
  MX_USART6_UART_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_SPI5_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
/*  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);
  HAL_Delay(500);

  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);
        uint8_t HANDSHAKE[7] = {0x53,0xac,0x01,0x02,0x03,0x7e},ret_handshake[7]={'\0'};
        do{
        	HAL_Delay(1000);
     	   if(HAL_UART_Transmit(&COM_uart, HANDSHAKE, sizeof(HANDSHAKE),1000) == HAL_OK){
     		   if(HAL_UART_Receive(&COM_uart, ret_handshake, sizeof(ret_handshake), 1000) == HAL_OK)
     			   for(int i =0 ; i<sizeof(HANDSHAKE); i++){
     				   if(HANDSHAKE[i] != ret_handshake[i]){
     					   HANDSHAKE_STATUS =1 ;
     					   break;
     				   }
     				   else{
     					   HANDSHAKE_STATUS  = 2;
     				   }
     		   }
//     		   if(HANDSHAKE_STATUS == 2){
//     			   break;
//     		   }
     		   HAL_Delay(500);
     	   }
        }while(HANDSHAKE_STATUS == 1);
        myDebug("Handshake with COM successful.\n");

//        create_empty_files();


  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
//  HAL_UART_Receive_DMA(&COM_uart, COM_RX_DATA, sizeof(COM_RX_DATA));

  HAL_UART_Transmit(&huart7,"********************S2S OBC is starting ************\n", sizeof("********************S2S OBC is starting ************\n"),1000);

//	while(1){
//		HAL_UART_Transmit(&huart7,"\n\n********************S2S ADC is starting ************\n", sizeof("********************S2S ADC is starting ************\n"),1000);
//
//		ADC_Operate(adc_channels, adc1_channels);
//		 ADC_Conv_Data(adc_conv_buf, adc_buf);
//		for(int i =0; i<sizeof(adc_channels); i++)
//		myprintf("%d | %d **", adc_channels[i],adc1_channels[i]);
//		//		HK_IMU();
//		HAL_Delay(10000);
//
//	}




 __init_storage();
 list_files(&Lfs);

//  char path[200];
// char txt[]="sangam is writing it manually";
//
//	__init_littefs();
 list_files_with_size(&Lfs, "/");
// 		  lfs_file_open(&Lfs, &File, "flags.txt", LFS_O_TRUNC  | LFS_O_CREAT );// |LFS_O_APPEND |LFS_O_APPEND
// 		  lfs_file_write(&Lfs, &File, &txt, sizeof(txt));
// 		  lfs_file_close(&Lfs, &File);
// read_file_from_littlefs(&Lfs, "satHealth.txt");
////  read_file_from_littlefs(&Lfs, "sat_health.txt");
// read_file_from_littlefs(&Lfs, "flags.txt");

 list_files_with_size(&Lfs, "/");
 truncate_text_file1(Lfs, File );
 uint8_t count=0;uint16_t timer_val;

  uint8_t command[30]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29};
  command[16]=0x01;
  command[17]=0xca;
  command[18]=0xd1;
  command[19]=0xf2;
  parse_command(command, Lfs, File );

  command[16]=0x01;
    command[17]=0x1d;
    command[18]=0xd1;
    command[19]=0xf2;
    parse_command(command, Lfs, File );

    command[16]=0x01;
      command[17]=0xee;
      command[18]=0xee;
      command[19]=0xee;
      parse_command(command, Lfs, File );

      command[16]=0x01;
        command[17]=0xee;
        command[18]=0xaa;
        command[19]=0xaa;
        parse_command(command, Lfs, File );

        command[16]=0x01;
          command[17]=0x1a;
          command[18]=0xe0;
          command[19]=0x1e;
          parse_command(command, Lfs, File );

          command[16]=0x02;
          command[17]=0xfd;
          command[18]=0xba;
          command[19]=0xd0;
          parse_command(command, Lfs, File );

          command[16]=0x03;
          command[17]=0x0e;
          command[18]=0x53;
          command[19]=0xce;
          parse_command(command, Lfs, File );

          command[16]=0x04;
          command[17]=0xcc;
          command[18]=0x5e;
          command[19]=0xbd;
          parse_command(command, Lfs, File );

          command[16]=0x05;
          command[17]=0xac;
          command[18]=0xcf;
          command[19]=0xcf;
          parse_command(command, Lfs, File );
          */
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  if(HAL_UART_Receive(&huart2, COM_RX_DATA,30,1000)==HAL_OK){
//		  myprintf("Received data %s\n",COM_RX_DATA);
//	  }
	  catch = HAL_UART_Receive(&COM_uart, COM_RX_DATA,10,5000);
	  if(catch==HAL_OK){
			  myprintf("Received data %s\n",COM_RX_DATA);
		  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM9 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  /* USER CODE BEGIN Callback 0 */
////////
//  /* USER CODE END Callback 0 */
//  if (htim->Instance == TIM9) {
//    HAL_IncTick();
//  }
//  /* USER CODE BEGIN Callback 1 */
////////
//  /* USER CODE END Callback 1 */
//}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
