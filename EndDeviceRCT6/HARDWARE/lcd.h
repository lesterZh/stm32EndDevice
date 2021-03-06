#ifndef __LCD_H
#define __LCD_H	 
#include "sys.h"

//#define LCD_TEST 1

#ifdef LCD_TEST	

	#define LCD_SCL PCout(6)       //SCLK  时钟 D0（SCLK）
	#define LCD_SDA PCout(7)       //SDA   D1（MOSI） 数据
	#define LCD_RST PCout(8)       //_RES  hardware reset   复位 
	#define LCD_DC  PCout(9)       //A0  H/L 命令数据选通端，H：数据，L:命�

#else

	#define LCD_SCL PDout(2)       //SCLK  时钟 D0（SCLK）
	#define LCD_SDA PCout(12)       //SDA   D1（MOSI） 数据
	#define LCD_RST PCout(11)       //_RES  hardware reset   复位 
	#define LCD_DC  PCout(10)       //A0  H/L 命令数据选通端，H：数据，L:命�

#endif

void LCD_Init(void) ;
void LCD_welcome(void);
void LCD_CLS(void);
void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);
	
	void LCD_show1(u8 * data) ;
	void LCD_show2(u8 * data) ;
	void LCD_show3(u8 * data) ;
	void LCD_show4(u8 * data) ;
	
#endif

