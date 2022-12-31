#include "stm32f4xx.h"
#include "stm32f4xx_fsmc.h"

void FSMC_Init()
{
	FSMC_NORSRAMTimingInitTypeDef TimingStructure;
	FSMC_NORSRAMInitTypeDef InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//FSMC GPIO Config.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG |RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 ???????,???????
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//????????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOB, &GPIO_InitStructure);//????? //PB15 ???????,???????

	GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(0XFF<<8);//PD0,1,4,5,8~15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOD, &GPIO_InitStructure);//?????  
	
  GPIO_InitStructure.GPIO_Pin = (3<<0)|(0X1FF<<7);//PE0,1,7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOE, &GPIO_InitStructure);//?????  
	
 	GPIO_InitStructure.GPIO_Pin = (0X3F<<0)|(0XF<<12); 	//PF0~5,12~15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOF, &GPIO_InitStructure);//?????  

	GPIO_InitStructure.GPIO_Pin =(0X3F<<0)| GPIO_Pin_10;//PG0~5,10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOG, &GPIO_InitStructure);//????? 
	
	
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12
 
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12
 
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FSMC);//PF0,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FSMC);//PF1,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FSMC);//PF2,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FSMC);//PF3,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FSMC);//PF4,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FSMC);//PF5,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FSMC);//PF13,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FSMC);//PF14,AF12
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FSMC);//PF15,AF12
	
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource3,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource10,GPIO_AF_FSMC);
	
	//FSMC 时钟初始化
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);
	
	//TimingInitStructure 初始化
	TimingStructure.FSMC_AccessMode = FSMC_AccessMode_A;   //ModeA for SRAM
	TimingStructure.FSMC_AddressHoldTime = 0x00; //不需要配置
	TimingStructure.FSMC_AddressSetupTime = 0x00;
	TimingStructure.FSMC_BusTurnAroundDuration = 0x00;  //不需要配置
	TimingStructure.FSMC_CLKDivision = 0x00;  //用于同步存储器
	TimingStructure.FSMC_DataLatency = 0x00; //异步存储器不需要配置
	TimingStructure.FSMC_DataSetupTime = 0xA;
	
	
	InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	InitStructure.FSMC_ReadWriteTimingStruct = &TimingStructure;
	InitStructure.FSMC_WriteTimingStruct = &TimingStructure;
	FSMC_NORSRAMInit(&InitStructure);
	//FSMC 工作
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3,ENABLE);
	
}

