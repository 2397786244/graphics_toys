#ifndef _LCD_H
#define _LCD_H

#include "stm32f4xx.h"
#include "stm32f4xx_fsmc.h"
#include "math.h"

#define LCD_BASE_ADDR ((uint32_t)0x6C000000)
//HADDR[25:1] --  FSMC[24:0]  所以对FSMC地址进行左移一位 
#define WIDTH ((uint16_t)240)
#define HEIGHT ((uint16_t)200)

#define APUT1(addr)  (addr |  (1 << 7))
#define APUT0(addr)  (addr & ~(1 << 7))

#define LCD_DATA APUT1(LCD_BASE_ADDR)
#define LCD_CMD APUT0(LCD_BASE_ADDR)

#define LCD_CMD_ID1 ((uint16_t)0xDA)
#define LCD_CMD_ID4 ((uint16_t)0xd3)
#define LCD_CMD_COLSET ((uint16_t)0x2a)
#define LCD_CMD_ROWSET ((uint16_t)0x2b)
#define LCD_CMD_RAMWR ((uint16_t)0x2c)

// color RGB565
#define BLACK  ((uint16_t)0x0)
#define PINK   ((uint16_t)0xfe19) 
#define PURPLE ((uint16_t)0x8010) 
#define BLUE   ((uint16_t)0x001F) 
#define GREEN  ((uint16_t)0x07E0) 
#define WHITE  ((uint16_t)0xffff)
#define RED    ((uint16_t)0x8811)
#define GREAT_RED ((uint16_t)0xf800)

// status code.
#define OK ((uint16_t)0x0915)
#define WRONG (OK+1)

void LCD_Config();
void LCD_Reg_Config();
void LCD_Write_Data(uint16_t data);
void LCD_Write_CMD(uint16_t cmd);
uint16_t LCD_Read_DATA(void);

#define LCD_WR_REG(d) LCD_Write_CMD(d)
#define LCD_WR_DATA(d) LCD_Write_Data(d)
#define IN_HEART(xx,yy)    ( pow(xx * xx + yy * yy - 1.0,3.0) - pow(xx,2.0) * pow(yy,3.0) )
#define SetRGB24(R8,G8,B8) ((uint32_t)(R8<<16)|(G8<<8)|B8)


// LCD 功能函数
uint16_t LCD_Read_ID1(void);
uint16_t LCD_Read_ID4(void);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t point_color);
void LCD_DrawHeart(uint16_t x,uint16_t y,uint16_t c,uint16_t size);
void LCD_DrawCircle(uint16_t xc,uint16_t yc,uint16_t r,uint16_t color);
void LCD_DrawRect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t c);
uint16_t RGB24ToRGB16Convert(uint32_t color888);
uint16_t LCD_FullScreenWrite(uint16_t width,uint16_t height,uint16_t * image_ptr);

#define LCD_BackLight_ON  (GPIO_SetBits(GPIOB,GPIO_Pin_15))
#define LCD_BackLight_OFF (GPIO_ResetBits(GPIOB,GPIO_Pin_15))
#endif
