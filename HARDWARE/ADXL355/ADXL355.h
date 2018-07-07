/**
File:		ADXL355.h
Author:		Guanghao Hu
Date:		18-5-21
Describe:	Device head file of ADXL355
**/
#ifndef _ADXL355_H_
#define _ADXL355_H_
#include "myiic.h"   

/**
 ASEL (pin) = 0, device address = 0x1D
 ASEL (pin) = 1, device address = 0x53
**/
#define ASEL_LOW 	0x1D
#define ASEL_HIGH	0x53	

/********************************* Definitions ********************************/

/* ADXL355 registers addresses */
#define DEVID_AD                 0x00
#define DEVID_MST                0x01
#define PARTID                   0x02
#define REVID                    0x03
#define STATUS                   0x04
#define FIFO_ENTRIES             0x05
#define TEMP2                    0x06
#define TEMP1                    0x07
#define XDATA3                   0x08
#define XDATA2                   0x09
#define XDATA1                   0x0A
#define YDATA3                   0x0B
#define YDATA2                   0x0C
#define YDATA1                   0x0D
#define ZDATA3                   0x0E
#define ZDATA2                   0x0F
#define ZDATA1                   0x10
#define FIFO_DATA                0x11
#define OFFSET_X_H               0x1E
#define OFFSET_X_L               0x1F
#define OFFSET_Y_H               0x20
#define OFFSET_Y_L               0x21
#define OFFSET_Z_H               0x22
#define OFFSET_Z_L               0x23
#define ACT_EN                   0x24
#define ACT_THRESH_H             0x25
#define ACT_THRESH_L             0x26
#define ACT_COUNT                0x27
#define FILTER                   0x28
#define FIFO_SAMPLES             0x29
#define INT_MAP                  0x2A
#define SYNC                     0x2B
#define RANGE                    0x2C
#define POWER_CTL                0x2D
#define SELF_TEST                0x2E
#define RESET                    0x2F

/* Temperature parameters */
#define ADXL355_TEMP_BIAS       (float)1852.0      /* Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /* Accelerometer temperature change from datasheet (LSB/degC) */
	
/* Accelerometer parameters */
#define ADXL_RANGE     		2     /* ADXL362 sensitivity: 2, 4, 8 [g] */
#define ACT_VALUE          50     /* Activity threshold value */
#define INACT_VALUE        50     /* Inactivity threshold value */
#define ACT_TIMER          100    /* Activity timer value in ms */
#define INACT_TIMER        10     /* Inactivity timer value in seconds */

typedef enum {
   SPI_READ_ONE_REG = 1,            /* Read one ACC register */
   SPI_READ_TWO_REG,                /* Read two ACC registers */
   SPI_READ_THREE_REG,              /* Read X,Y,Z ACC registers */
} enRegsNum;

void ADXL355_Init(u8 Deviceaddr); //初始化IIC

u8 ADXL355_ReadOneByte(u8 ReadAddr);								//指定地址读取一个字节
static void ADXL355_WriteOneByte(u8 WriteAddr,u8 DataToWrite);			//指定地址写入一个字节
static void ADXL355_WriteLenByte(u8 WriteAddr,u32 DataToWrite,u8 Len);	//指定地址开始写入指定长度的数据
static u32 ADXL355_ReadLenByte(u8 ReadAddr,u8 Len);						//指定地址开始读取指定长度数据

void ADXL355_Start_Sensor(void);
void ADXL355_Stop_Sensor(void);

float ADXL_RANGE_SET(u8 Range);
u8 ADXL355_DEVICE_ID(void);
u8 ADXL355_MI(void);
u8 ADXL355_PART_ID(void);

void ADXL355_Data_Scan(int_least32_t* SensorX,int_least32_t* SensorY,int_least32_t* SensorZ,u32* SensorT);
static u32 ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData);

#endif
