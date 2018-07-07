#include "ADXL355.h" 
#include "delay.h" 				 
#include "usart.h"

static u8 DeviceAddr;
/************************* Local scope functions *****************************/

/**
   @brief 在ADXL355指定地址读入一个数据
   @param ReadAddr  :读取数据的目的地址  
   @return 读到的数据
**/
u8 ADXL355_ReadOneByte(u8 ReadAddr){				  
	u8 temp=0;
    IIC_Start();
	IIC_Send_Byte( (DeviceAddr<<1) | 0x00);   			//发送地址
	IIC_Wait_Ack();
	
	IIC_Send_Byte(ReadAddr);
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte( (DeviceAddr<<1) | 0x01);   			//发送地址
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件	    
	return temp;
}

/**
   @brief 在ADXL355指定地址写入一个数据
   @param WriteAddr  :写入数据的目的地址  
   @param DataToWrite:要写入的数据
   @return none
**/
static void ADXL355_WriteOneByte(u8 WriteAddr,u8 DataToWrite){
    IIC_Start();  
	IIC_Send_Byte( (DeviceAddr<<1) | 0x00);		//写模式
	printf("w_ack = %d\r\n",IIC_Wait_Ack());
    IIC_Send_Byte(WriteAddr);   		//发送地址
	printf("w_ack = %d\r\n",IIC_Wait_Ack());
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	printf("w_ack = %d\r\n",IIC_Wait_Ack());
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/**
   @brief 在ADXL355里面的指定地址开始写入长度为Len的数据,该函数用于写入16bit或者32bit的数据.
   @param WriteAddr  :开始写入的地址 
   @param DataToWrite:数据数组首地址
   @param Len        :要写入数据的长度2,4
   @return none
**/
static void ADXL355_WriteLenByte(u8 WriteAddr,u32 DataToWrite,u8 Len){
	u8 t;
	for(t=0;t<Len;t++)
	{
		ADXL355_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}	
}


/**
   @brief 在ADXL355里面的指定地址开始读出长度为Len的数据
   @param ReadAddr  :开始读出的地址 
   @param Len       :要读出数据的长度2,3,4
   @return 返回值   :数据
**/
static u32 ADXL355_ReadLenByte(u8 ReadAddr,u8 Len){  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=ADXL355_ReadOneByte(ReadAddr+t); 
	}
	return temp;												    
}

/**
   @brief Convert the two's complement data in X,Y,Z registers to signed integers
   @param ui32SensorData - raw data from register
   @return int32_t - signed integer data
**/
static u32 ADXL355_Acceleration_Data_Conversion (uint32_t ui32SensorData){
   u32 volatile i32Conversion = 0;
   ui32SensorData = (ui32SensorData  >> 4);
   ui32SensorData = (ui32SensorData & 0x000FFFFF);
	
   if((ui32SensorData & 0x00080000)  == 0x00080000){
         i32Conversion = (ui32SensorData | 0xFFF00000);
   }
   else{
         i32Conversion = ui32SensorData;
   }
   return i32Conversion;
}


/************************* Global scope functions *****************************/
/**
	@brief ADXL355 init
	@para  Deviceaddr	device addr
	@reture none
**/
void ADXL355_Init(u8 Deviceaddr){
	IIC_Init();//IIC初始化
	DeviceAddr = Deviceaddr;
//   /* Quick verification test for boards */
//   uint32_t volatile ui32test = SPI_Read(DEVID_AD, SPI_READ_ONE_REG);                  /* Read the ID register */
//   uint32_t volatile ui32test2 = SPI_Read(DEVID_MST, SPI_READ_ONE_REG);                  /* Read the ID register */
//   uint32_t volatile ui32test3 = SPI_Read(PARTID, SPI_READ_ONE_REG);                  /* Read the ID register */
//   uint32_t volatile ui32test4 = SPI_Read(REVID, SPI_READ_ONE_REG);                 /* Read the ID register */
}

/**
   @brief Turns on accelerometer measurement mode.
   @return none
**/
void ADXL355_Start_Sensor(void){
	uint8_t ui8temp = 0;
	ui8temp = ADXL355_ReadOneByte(POWER_CTL);       /* Read POWER_CTL register, before modifying it */
	printf("ui8temp  = 0x%2x\r\n", ui8temp);                 /* Print the ID register */
	ui8temp = ui8temp & 0xFE;                       /* Set measurement bit in POWER_CTL register */
	ADXL355_WriteOneByte(POWER_CTL, ui8temp);       /* Write the new value to POWER_CTL register */
}


/**
   @brief Puts the accelerometer into standby mode.
   @return none
**/
void ADXL355_Stop_Sensor(void){
   uint8_t ui8temp;
   ui8temp = (uint8_t)ADXL355_ReadOneByte(POWER_CTL);        /*Read POWER_CTL register, before modifying it */
   ui8temp = ui8temp | 0x01;                                 /* Clear measurement bit in POWER_CTL register */
   ADXL355_WriteOneByte(POWER_CTL, ui8temp);                 /* Write the new value to POWER_CTL register */
}

/**
   @brief Set the range of measurement
   @param Range :2 , 4 , 8
   @return  Precision
**/
float ADXL_RANGE_SET(u8 Range){
	float adxl355Scale;
	switch (Range){
		case 2:
			ADXL355_WriteOneByte(RANGE, 0x81);          /* Set sensor range within RANGE register */
			adxl355Scale = 256000.0f;
			break;
		case 4:
			ADXL355_WriteOneByte(RANGE, 0x82);          /* Set sensor range within RANGE register */
			adxl355Scale = 128000.0f;
			break;
		case 8:
			ADXL355_WriteOneByte(RANGE, 0x83);          /* Set sensor range within RANGE register */
			adxl355Scale = 64000.0f;
			break;
		default :
			ADXL355_WriteOneByte(RANGE, 0x81);          /* Set sensor range within RANGE register */
			adxl355Scale = 256000.0f;
			break;
	}
	return adxl355Scale;
}

/**
   @brief Get Device ID
   @return  ID
**/
u8 ADXL355_DEVICE_ID(void){
	return ADXL355_ReadOneByte(DEVID_AD);                  /* Read the ID register */
}

/**
   @brief Get Manufacturer Info
   @return  Manufacturer Info
**/
u8 ADXL355_MI(void){
	return ADXL355_ReadOneByte(DEVID_MST);               	/* Read the Manufacturer register */
}

/**
   @brief Get Device ID
   @return  ID
**/
u8 ADXL355_PART_ID(void){
	return ADXL355_ReadOneByte(PARTID);                  	/* Read the Part ID register */
}

/**
   @brief Reads the accelerometer data.
   @para SensorX : X acclelerate data pointer
   @para SensorY : Y acclelerate data pointer
   @para SensorZ : Z acclelerate data pointer
   @para SensorZ : Temperature data pointer
   @return none
**/
void ADXL355_Data_Scan(int_least32_t* SensorX,int_least32_t* SensorY,int_least32_t* SensorZ,u32* SensorT){
	volatile uint32_t ui32SensorX;
	volatile uint32_t ui32SensorY;
	volatile uint32_t ui32SensorZ;
	volatile uint32_t ui32SensorT;
	ui32SensorX = ADXL355_ReadLenByte(XDATA3,SPI_READ_THREE_REG);
	ui32SensorY = ADXL355_ReadLenByte(YDATA3,SPI_READ_THREE_REG);
	ui32SensorZ = ADXL355_ReadLenByte(ZDATA3, SPI_READ_THREE_REG);
	ui32SensorT = ADXL355_ReadLenByte(TEMP2, SPI_READ_TWO_REG);
	*SensorT = ui32SensorT & 0x00000FFF;
	*SensorX = ADXL355_Acceleration_Data_Conversion(ui32SensorX);
	*SensorY = ADXL355_Acceleration_Data_Conversion(ui32SensorY);
	*SensorZ = ADXL355_Acceleration_Data_Conversion(ui32SensorZ);
}





