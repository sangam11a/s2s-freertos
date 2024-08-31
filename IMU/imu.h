/*
 * IMU.h
 *
 *  Created on: Sep 8, 2023
 *      Author: User
 */

//#ifndef IMU_H_
//#define IMU_H_

#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "math.h"

//Magnetometer library

#define LIS3MDL_ADDRESS    0x1C
#define LIS3MDL_CTRL_REG1  0x20
#define LIS3MDL_CTRL_REG2 0x21
#define LIS3MDL_CTRL_REG3  0x22
#define LIS3MDL_CTRL_REG4  0x23
#define LIS3MDL_SENSITIVITY  0.000488281  //±16 Gauss and a resolution of 16-bits

#define LIS3MDL_OUTX_L 0x28
#define LIS3MDL_OUTY_L 0x2A
#define LIS3MDL_OUTZ_L 0x2C
#define LIS3MDL_OUTX_H 0x29
#define LIS3MDL_OUTY_H 0x2B
#define LIS3MDL_OUTZ_H 0x2D

extern SPI_HandleTypeDef hspi1;

#define CS_SEL		0
#define CS_DES     1

#define RAD2DEG 57.2957795131

#define ACCEL_XOUT_H      0x3B
#define ACCEL_XOUT_L      0x3C
#define ACCEL_YOUT_H      0x3D
#define ACCEL_YOUT_L      0x3E
#define ACCEL_ZOUT_H      0x3F
#define ACCEL_ZOUT_L      0x40

#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48

typedef struct MPU6500 {
	struct RawData {
		int16_t ax, ay, az, gx, gy, gz;
	} rawData;

	struct SensorData {
		float aScaleFactor, gScaleFactor;
		float ax, ay, az, gx, gy, gz;
	} sensorData;

	struct GyroCal {
		float x, y, z;
	} gyroCal;

	struct Attitude {
		float tau, dt;
		float r, p, y;
	} attitude;

	struct Settings {
		uint8_t aFullScaleRange, gFullScaleRange;
		GPIO_TypeDef *CS_PORT;
		uint8_t CS_PIN;
	} settings;
} MPU6500_t;

// Full scale ranges
enum gyroscopeFullScaleRange {
	GFSR_250DPS, GFSR_500DPS, GFSR_1000DPS, GFSR_2000DPS
};

enum accelerometerFullScaleRange {
	AFSR_2G, AFSR_4G, AFSR_8G, AFSR_16G
};

typedef enum DLPFBandwidth_ {
	DLPF_BANDWIDTH_184HZ = 0,
	DLPF_BANDWIDTH_92HZ,
	DLPF_BANDWIDTH_41HZ,
	DLPF_BANDWIDTH_20HZ,
	DLPF_BANDWIDTH_10HZ,
	DLPF_BANDWIDTH_5HZ
} DLPFBandwidth;

typedef enum SampleRateDivider_ {
	LP_ACCEL_ODR_0_24HZ = 0,
	LP_ACCEL_ODR_0_49HZ,
	LP_ACCEL_ODR_0_98HZ,
	LP_ACCEL_ODR_1_95HZ,
	LP_ACCEL_ODR_3_91HZ,
	LP_ACCEL_ODR_7_81HZ,
	LP_ACCEL_ODR_15_63HZ,
	LP_ACCEL_ODR_31_25HZ,
	LP_ACCEL_ODR_62_50HZ,
	LP_ACCEL_ODR_125HZ,
	LP_ACCEL_ODR_250HZ,
	LP_ACCEL_ODR_500HZ
} SampleRateDivider;

uint8_t LIS3MDL_ReadReg(SPI_HandleTypeDef *hspi, uint8_t add);
void LIS3MDL_WriteReg(SPI_HandleTypeDef *hspi, uint8_t add, uint8_t val);
float LIS3MDL_ReadData(SPI_HandleTypeDef *hspi, uint8_t add);
void LIS3MDL_Init(SPI_HandleTypeDef *hspi, uint8_t ctrl2);

void SensortoSat_Mag();

/* Reads register value of Gyro and Accel */
uint8_t MPU6500_ReadReg(SPI_HandleTypeDef *hspi, uint8_t add);

/* read the data, each argument should point to a array for x, y, and x */
void MPU6500_GetData(MPU6500_t *pMPU6500);

/* sets the DLPF bandwidth to values other than default */
void MPU6500_SetDLPFBandwidth(DLPFBandwidth bandwidth);

//WHO AM I register MPU
uint8_t whoAmI();
uint8_t SPIx_WriteRead(uint8_t Byte);
void MPU_SPI_Write(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void MPU_SPI_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
void writeRegister(uint8_t subAddress, uint8_t data);
void readRegisters(uint8_t subAddress, uint8_t count, uint8_t *dest);

void MPU_calibrateGyro(MPU6500_t *pMPU6500, uint16_t numCalPoints);
void MPU_calcAttitude(MPU6500_t *pMPU6500);
void MPU_writeGyroFullScaleRange(MPU6500_t *pMPU6500, uint8_t gScale);
void MPU_writeAccFullScaleRange(MPU6500_t *pMPU6500, uint8_t aScale);
uint8_t MPU_begin(SPI_HandleTypeDef *SPIx, MPU6500_t *pMPU6550);
void MPU_readProcessedData(MPU6500_t *pMPU6500);

void IMU_Setup();
void IMU_Data();
void HK_IMU();

//#endif /* INC_IMU_H_ */
