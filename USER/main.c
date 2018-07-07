#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "ADXL355.h" 
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "fattester.h"
#include "string.h"
#include "ANO_DT.h"

int main(void)
{ 
	u32 total,free,size,SensorT;
	int_least32_t SensorX,SensorY,SensorZ;
	u8 res=0;	
	u32 j = 100000;
	char tem[125];
	UINT flag;
	float adxl355Scale,f32temp;
	FIL file;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    							//初始化延时函数
	uart_init(115200);								//初始化串口波特率为115200
	ADXL355_Init(ASEL_LOW);
	LED_Init();										//初始化LED 
	usmart_dev.init(84);							//初始化USMART
	KEY_Init(); 									//按键初始化  
	my_mem_init(SRAMIN);							//初始化内部内存池
	my_mem_init(SRAMCCM);							//初始化CCM内存池
	
	while(SD_Init())								//检测内存卡
	{
		printf("SD Card Error!\r\n");
		delay_ms(500);					
		LED0=!LED0;									//DS0闪烁
	}
	
	adxl355Scale = ADXL_RANGE_SET(8);
	ADXL355_Start_Sensor();
	u8 ID = ADXL355_DEVICE_ID();				/* Read the ID register */
	while(ID != 0xad){
		delay_ms(100);
		LED0=!LED0;//DS0闪烁
	}
	
	exfuns_init();									//为fatfs相关变量申请内存				 
  	f_mount(fs[0],"0:",1); 							//挂载SD卡 
	
	while(exf_getfree("0",&total,&free))			//得到SD卡的总容量和剩余容量
	{
		printf("SD Card Fats Error!\r\n");
		delay_ms(200);
		LED0=!LED0;//DS0闪烁
	}
	printf("FATFS OK!\r\n");	 
	printf("SD Total Size:  %d   MB\r\n",total>>10);	 
	printf("SD  Free Size:  %d   MB\r\n",free>>10); 
	
//	while((res = f_open(&file,(const TCHAR*)"/2.txt",FA_WRITE|FA_CREATE_NEW)))printf("file open %d \r\n",res);

	while(j--)
	{
		ADXL355_Data_Scan( &SensorX, &SensorY, &SensorZ, &SensorT);
		f32temp = ((((float)SensorT - ADXL355_TEMP_BIAS)) / ADXL355_TEMP_SLOPE) + 25.0;
		size=sprintf(tem,"%f;%f;%f\r\n",(float)SensorX / adxl355Scale,(float)SensorX / adxl355Scale,(float)SensorZ / adxl355Scale);
		//f_write(&file,tem,size,&flag);
//		printf("file write size = %d %s\r\n%d %f	%d %f	%d %f	%d	%f\r\n",flag,tem,SensorX,(float)SensorX,SensorY,(float)SensorY,SensorZ,(float)SensorZ,SensorT,f32temp);
		LED0=!LED0;
		ANO_DT_Send_Senser(SensorX,SensorY,SensorZ,0,0,0,0,0,0,0);
	}
	f_close(&file);
}
