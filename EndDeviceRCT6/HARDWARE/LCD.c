#include "codetab.h"
#include "lcd.h"


#define XLevelL        0x00
#define XLevelH        0x10
#define XLevel         ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column     128
#define Max_Row        64
#define Brightness     0xCF 
#define X_WIDTH        128
#define Y_WIDTH        64


void DelayMS(unsigned int msec)
{ 
//    unsigned int i,j;
//    
//    for (i=0; i<msec; i++)
//        for (j=0; j<530; j++);
	
	delay_ms(msec);
}

/*********************LCD ��ʱ1ms************************************/
void LCD_DLY_ms(unsigned int ms)
{                         
//    unsigned int a;
//    while(ms)
//    {
//        a=1800;
//        while(a--);
//        ms--;
//    }
//    return;
	delay_ms(ms);
}
/*********************LCDд����************************************/ 
void LCD_WrDat(unsigned char dat)     
{
    unsigned char i=8, temp=0;
    LCD_DC=1;  
    for(i=0;i<8;i++) //����һ����λ���� 
    {
        LCD_SCL=0;  
        
        temp = dat&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        LCD_SCL=1;             
        dat<<=1;    
    }
}
/*********************LCDд����************************************/                                        
void LCD_WrCmd(unsigned char cmd)
{
    unsigned char i=8, temp=0;
    LCD_DC=0;
    for(i=0;i<8;i++) //����һ����λ���� 
    { 
        LCD_SCL=0; 
       
        temp = cmd&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        LCD_SCL=1;
        cmd<<=1;;        
    }     
}
/*********************LCD ��������************************************/
void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
    LCD_WrCmd(0xb0+y);
    LCD_WrCmd(((x&0xf0)>>4)|0x10);
    LCD_WrCmd((x&0x0f)|0x01); 
} 
/*********************LCDȫ��************************************/
void LCD_Fill(unsigned char bmp_dat) 
{
    unsigned char y,x;
    for(y=0;y<8;y++)
    {
        LCD_WrCmd(0xb0+y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10);
        for(x=0;x<X_WIDTH;x++)
            LCD_WrDat(bmp_dat);
    }
}
/*********************LCD��λ************************************/
void LCD_CLS(void)
{
    unsigned char y,x;    
    for(y=0;y<8;y++)
    {
        LCD_WrCmd(0xb0+y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10); 
        for(x=0;x<X_WIDTH;x++)
            LCD_WrDat(0);
    }
}
/*********************LCD��ʼ��************************************/
void LCD_Init(void)     
{  
    GPIO_InitTypeDef  GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC, ENABLE);	 //PB15 LED1

#ifdef LCD_TEST	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8|GPIO_Pin_9;				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	
#else
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
#endif	

    LCD_SCL=1;
    LCD_RST=0;
    LCD_DLY_ms(100);
    LCD_RST=1;      //���ϵ絽���濪ʼ��ʼ��Ҫ���㹻��ʱ�䣬���ȴ�RC��λ���   
    LCD_WrCmd(0xae);//--turn off oled panel
    LCD_WrCmd(0x00);//---set low column address
    LCD_WrCmd(0x10);//---set high column address
    LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    LCD_WrCmd(0x81);//--set contrast control register
    LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
    LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    LCD_WrCmd(0xa6);//--set normal display
    LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
    LCD_WrCmd(0x3f);//--1/64 duty
    LCD_WrCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    LCD_WrCmd(0x00);//-not offset
    LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    LCD_WrCmd(0xd9);//--set pre-charge period
    LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    LCD_WrCmd(0xda);//--set com pins hardware configuration
    LCD_WrCmd(0x12);
    LCD_WrCmd(0xdb);//--set vcomh
    LCD_WrCmd(0x40);//Set VCOM Deselect Level
    LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    LCD_WrCmd(0x02);//
    LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
    LCD_WrCmd(0x14);//--set(0x10) disable
    LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    LCD_WrCmd(0xaf);//--turn on oled panel
    LCD_Fill(0xff);  //��ʼ����
    LCD_Set_Pos(0,0);  
		
	LCD_CLS();
} 
/***************������������ʾ6*8һ���׼ASCII�ַ���    ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void LCD_P6x8Str(unsigned char x, unsigned char y,unsigned char ch[])
{
    unsigned char c=0,i=0,j=0;      
    while (ch[j]!='\0')
    {    
        c =ch[j]-32;
        if(x>126){x=0;y++;}
        LCD_Set_Pos(x,y);    
        for(i=0;i<6;i++)     
            LCD_WrDat(F6x8[c][i]);  
        x+=6;
        j++;
    }
}
/*******************������������ʾ8*16һ���׼ASCII�ַ���     ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[])
{
    unsigned char c=0,i=0,j=0;
    while (ch[j]!='\0')
    {    
        c =ch[j]-32;
        if(x>120){x=0;y++;}
        LCD_Set_Pos(x,y);    
        for(i=0;i<8;i++)     
            LCD_WrDat(F8X16[c*16+i]);
        LCD_Set_Pos(x,y+1);    
        for(i=0;i<8;i++)     
            LCD_WrDat(F8X16[c*16+i+8]);  
        x+=8;
        j++;
    }
}
/*****************������������ʾ16*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void LCD_P16x16Ch(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned char wm=0;
    unsigned int adder=32*N;  //      
    LCD_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)  //             
    {
        LCD_WrDat(F16x16[adder]);    
        adder += 1;
    }      
    LCD_Set_Pos(x,y + 1); 
    for(wm = 0;wm < 16;wm++) //         
    {
        LCD_WrDat(F16x16[adder]);
        adder += 1;
    }           
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{     
    unsigned int j=0;
    unsigned char x,y;
    
    if(y1%8==0) y=y1/8;      
    else y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        LCD_Set_Pos(x0,y);                
        for(x=x0;x<x1;x++)
        {      
            LCD_WrDat(BMP[j++]);            
        }
    }
} 

void LCD_welcome(void)
{
    int i=0;
	LCD_P8x16Str(0, 0, "i am lcd");   
    LCD_P8x16Str(0, 2, "i am lcd");
    LCD_P8x16Str(0, 4, "2016-10-01");
	LCD_P8x16Str(0, 6, "2016-10-01");
}

