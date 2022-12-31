#include "lcd.h"
#include "delay.h"

void LCD_Config(){
	FSMC_NORSRAMTimingInitTypeDef readTimingStructure;
	FSMC_NORSRAMTimingInitTypeDef writeTimingStructure;
	FSMC_NORSRAMInitTypeDef InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	//FSMC GPIO Config.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG |RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB,ENABLE);

	//LCD使用的FSMC 相关引脚:
	//FSMC A6    PF12
	//FSMC D0-D15  PD14 PD15 PD0 PD1 PE7-PE15 PD8-PD10
	//FSMC NOE     
	//FSMC NWE    PD4 PD5
	//FSMC NE4  PG12
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 ???????,???????
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//????????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOB, &GPIO_InitStructure);//????? //PB15 ???????,???????
	
	// 1100 0111 0011 0011
	GPIO_InitStructure.GPIO_Pin = 0xC733;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOD, &GPIO_InitStructure);//?????  
	
	//1111 1111 1000 0000
  GPIO_InitStructure.GPIO_Pin = 0xFF80;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOE, &GPIO_InitStructure);//?????  
	
	//PF12
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//???????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//???????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//????
  GPIO_Init(GPIOF, &GPIO_InitStructure);//?????  
	
	//PG12
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
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
 
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12

  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);
	
	//FSMC 时钟初始化
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);
	
	//TimingInitStructure 初始化
	writeTimingStructure.FSMC_AccessMode = FSMC_AccessMode_B;   //ModeB for NOR FLASH
	writeTimingStructure.FSMC_AddressHoldTime = 0x00; //不需要配置
	writeTimingStructure.FSMC_AddressSetupTime = 0xA;
	writeTimingStructure.FSMC_BusTurnAroundDuration = 0x00;  //不需要配置
	writeTimingStructure.FSMC_CLKDivision = 0x00;  //用于同步存储器
	writeTimingStructure.FSMC_DataLatency = 0x00; //异步存储器不需要配置
	writeTimingStructure.FSMC_DataSetupTime = 0xA;
	
	readTimingStructure.FSMC_AccessMode = FSMC_AccessMode_B;
	readTimingStructure.FSMC_AddressHoldTime = 0x00; //不需要配置
	readTimingStructure.FSMC_AddressSetupTime = 0xF;
	readTimingStructure.FSMC_BusTurnAroundDuration = 0x00;  //不需要配置
	readTimingStructure.FSMC_CLKDivision = 0x00;  //用于同步存储器
	readTimingStructure.FSMC_DataLatency = 0x00; //异步存储器不需要配置
	readTimingStructure.FSMC_DataSetupTime = 60;
	
	InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	InitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	InitStructure.FSMC_ReadWriteTimingStruct = &readTimingStructure;
	InitStructure.FSMC_WriteTimingStruct = &writeTimingStructure;
	FSMC_NORSRAMInit(&InitStructure);
	//FSMC 工作
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4,ENABLE);
}

//液晶屏写入配置
void LCD_Reg_Config(){
		LCD_WR_REG(0xCF);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0xC1); 
		LCD_WR_DATA(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATA(0x1B);   //VRH[5:0] 
		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
		LCD_WR_REG(0xC5);    //VCM control 
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2 
		LCD_WR_DATA(0XB7); 
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATA(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1A); 
		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2); 
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATA(0x01); 
		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x2A); 
		LCD_WR_DATA(0x28); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x0E); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x54); 
		LCD_WR_DATA(0XA9); 
		LCD_WR_DATA(0x43); 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 		 
		LCD_WR_REG(0XE1);    //Set Gamma 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x15); 
		LCD_WR_DATA(0x17); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x06); 
		LCD_WR_DATA(0x2B); 
		LCD_WR_DATA(0x56); 
		LCD_WR_DATA(0x3C); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);	 
		LCD_WR_REG(0x11); //Exit Sleep
		delay_ms(120);
		LCD_WR_REG(0x29); //display on	
}

//往LCD写入数据
void LCD_Write_Data(uint16_t data){
		__IO uint16_t * p = (__IO uint16_t*)LCD_DATA;
		*p = data;
}

void LCD_Write_CMD(uint16_t cmd){
		__IO uint16_t * p = (__IO uint16_t*)LCD_CMD;
		*p = cmd;
}

uint16_t LCD_Read_DATA(void){
	return *(uint16_t *)LCD_DATA;
}

uint16_t LCD_Read_ID1(void)
{
		LCD_Write_CMD(LCD_CMD_ID1);
		LCD_Read_DATA();
		return LCD_Read_DATA();
}

uint16_t LCD_Read_ID4(void){
	uint16_t d;
	LCD_Write_CMD(LCD_CMD_ID4);
	LCD_Read_DATA();
	LCD_Read_DATA();
	d = LCD_Read_DATA();
	d <<= 8;
	return (d | LCD_Read_DATA());
}

void LCD_DrawRect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t c){
	int i,j;
	for(i = x0; i <= x1;i++)
	{
		for(j = y0; j <=y1 ;j++)
			LCD_DrawPoint(i,j,c);
	}
}

void LCD_DrawCircle(uint16_t xc,uint16_t yc,uint16_t r,uint16_t color){
	uint16_t col_left = xc - r;
	col_left = (col_left >= 0) ? col_left : 0;
	uint16_t col_right = xc + r;
	col_right = (col_right >=0 ) ? ((col_right >= 240) ? 240 : col_right) : 0;
	uint16_t row_top = yc - r;
	row_top = (row_top >=0) ? row_top : 0;
	uint16_t row_bottom = yc + r;
	row_bottom = (row_bottom >= 0 ) ? ((row_bottom >= 320) ? 320 : row_bottom) : 0;
	
	//fisrt,set column address.
	LCD_Write_CMD(LCD_CMD_COLSET);
	LCD_Write_Data((col_left >> 8) & 0xFF);  // start column.MSB
	LCD_Write_Data(col_left & 0xFF);
	LCD_Write_Data((col_right >> 8) & 0xFF);  //end of column. MSB
	LCD_Write_Data(col_right&0xFF);
	
	//second,set page address
	LCD_Write_CMD(LCD_CMD_ROWSET);
	LCD_Write_Data((row_top >> 8) & 0xFF );  // start row.MSB
	LCD_Write_Data(row_top & 0xff);
	LCD_Write_Data((row_bottom >> 8) & 0xFF);  //end of row. MSB
	LCD_Write_Data(row_bottom & 0xFF);
	
	//third, GRAM write.
	LCD_Write_CMD(LCD_CMD_RAMWR);
	int x,y;
	for(x = col_left;x <= col_left ; x ++)
	{
		for(y = row_top ; y < row_bottom ; y ++)
		{
			if((x - xc) * (x - xc) + (y - yc) * (y - yc) < r * r)
				LCD_Write_Data(color);  // in circle
			else
				LCD_Write_Data(WHITE);
		}
	}
}

void LCD_DrawHeart(uint16_t x,uint16_t y,uint16_t c,uint16_t size){
	// 以 x,y为 坐标原点。
	uint16_t offset = size;
	uint16_t x0 = x - offset;
	uint16_t x1 = x + offset;
	uint16_t y0 = y - offset;
	uint16_t y1 = y + offset;
	float xx,yy;
	uint16_t i , j;
	for(i = x0;i<=x1;i++)
	{
		for(j = y0;j<=y1;j++)
		{
			xx = (float)(i - x) / (size / 2.0);
			yy = (float)(j - y) / (size / 2.0);
			if( IN_HEART(xx,yy) < 0.0f)
			{
				LCD_DrawPoint(i,j,c);
			}
		}
	}
}

void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t point_color){
	LCD_Write_CMD(LCD_CMD_COLSET);
	LCD_Write_Data(x >> 8);
	LCD_Write_Data(x & 0xFF);
	LCD_Write_CMD(LCD_CMD_ROWSET);
	LCD_Write_Data(y>>8);
	LCD_Write_Data(y & 0xFF);
	LCD_Write_CMD(LCD_CMD_RAMWR);
	LCD_Write_Data(point_color);
}

//注意，image数组是行优先储存
uint16_t LCD_FullScreenWrite(uint16_t width,uint16_t height,uint16_t * image_ptr){
	if(!image_ptr)
		return WRONG;
	
	uint16_t i,j;
	for(i = 0;i<height;i++)
	{
		for(j = 0;j<width;j++)
		{
				// fragCoord [0,0] - [width,height] --> []
				LCD_DrawPoint(j,i,*(image_ptr + i * width + j));
		}
	}
	return OK;
}

// rgb888 到 rgb565的转换.
uint16_t RGB24ToRGB16Convert(uint32_t color888)
{
	uint16_t r565 = 0x0;
	r565 |= ((color888 >> 19) & 0x1f);
	r565 <<= 11;
	r565 |= (((color888 >> 10) & 0x3f) << 5);
	r565 |= ((color888 >> 3) & 0x1f);
	return r565;
}