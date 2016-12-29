#ifndef __LCD_H
#define __LCD_H	 
#include "sys.h"

//#define LCD_TEST 1

#ifdef LCD_TEST	

	#define LCD_SCL PCout(6)       //SCLK  Ê±ÖÓ D0£¨SCLK£©
	#define LCD_SDA PCout(7)       //SDA   D1£¨MOSI£© Êý¾Ý
	#define LCD_RST PCout(8)       //_RES  hardware reset   ¸´Î» 
	#define LCD_DC  PCout(9)       //A0  H/L ÃüÁîÊý¾ÝÑ¡Í¨¶Ë£¬H£ºÊý¾Ý£¬L:ÃüÁ

#else

	#define LCD_SCL PDout(2)       //SCLK  Ê±ÖÓ D0£¨SCLK£©
	#define LCD_SDA PCout(12)       //SDA   D1£¨MOSI£© Êý¾Ý
	#define LCD_RST PCout(11)       //_RES  hardware reset   ¸´Î» 
	#define LCD_DC  PCout(10)       //A0  H/L ÃüÁîÊý¾ÝÑ¡Í¨¶Ë£¬H£ºÊý¾Ý£¬L:ÃüÁ

#endif

void LCD_Init(void) ;
void LCD_welcome(void);
void LCD_CLS(void);
void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);
	
#endif

