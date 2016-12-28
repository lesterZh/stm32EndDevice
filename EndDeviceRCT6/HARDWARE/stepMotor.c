#include "stepMotor.h"
#include "delay.h"

void motor_init(void)
{ 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //Ê¹ÄÜPB¶Ë¿ÚÊ±ÖÓ
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //MA-->PB.5 ¶Ë¿ÚÅäÖÃ
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾İÉè¶¨²ÎÊı³õÊ¼»¯GPIOA.8
	 GPIO_ResetBits(GPIOB,GPIO_Pin_5);						 //PB.5 Êä³öµÍ

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    	//MBö
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				//ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
	 GPIO_ResetBits(GPIOB,GPIO_Pin_6);					

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    	//MC
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				//ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
	 GPIO_ResetBits(GPIOB,GPIO_Pin_7);						  

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    	//MD
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				//ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
	 GPIO_ResetBits(GPIOB,GPIO_Pin_8);						  	
}

int delay = 4000;


void motor_run_half(void)
{
	MD = 0;
	MA = 1;
	delay_us(delay);
	MA = 1;
	MB = 1;
	delay_us(delay);
	MA = 0;
	MB = 1;
	delay_us(delay);
	MB = 1;
	MC = 1;
	delay_us(delay);
	MB = 0;
	MC = 1;
	delay_us(delay);
	MC = 1;
	MD = 1;
	delay_us(delay);
	MC = 0;
	MD = 1;
	delay_us(delay);
	MD = 1;
	MA = 1;
	delay_us(delay);
}

//direction=1 Õı×ª£¬0 ·´×ª £¬64´ÎÔ¼45¶È 128´Î90¶È
//8*8=64ÅÄ£¬µç»ú(ÄÚÈ¦)×ª¶¯360¡ã£¬ÍâÈ¦(Öá)×ª¶¯5.625¶È
void motor_run(int direction, int angle)
{
	int ts = (128 / 90.0) * angle, i=0;
	for (; i<ts; i++) 
	{
		if (direction == 1) //Õı×ª,8ÅÄ£¬µç»ú(ÄÚÈ¦)×ª¶¯45¶È£¬ÍâÈ¦(Öá)×ª¶¯45/64¶È
		{
			MD = 0;
			MA = 1;
			delay_us(delay);
			MA = 1;
			MB = 1;
			delay_us(delay);
			MA = 0;
			MB = 1;
			delay_us(delay);
			MB = 1;
			MC = 1;
			delay_us(delay);
			MB = 0;
			MC = 1;
			delay_us(delay);
			MC = 1;
			MD = 1;
			delay_us(delay);
			MC = 0;
			MD = 1;
			delay_us(delay);
			MD = 1;
			MA = 1;
			delay_us(delay);
		} else { //·´×ª
			MD = 1;
			MA = 1;
			delay_us(delay);
			MA = 0;
			MD = 1;
			delay_us(delay);
			
			MC = 1;
			MD = 1;
			delay_us(delay);
			MD = 0;
			MC = 1;
			delay_us(delay);
			
			MC = 1;
			MB = 1;
			delay_us(delay);
			MC = 0;
			MB = 1;
			delay_us(delay);
			MA = 1;
			MB = 1;
			delay_us(delay);
			MB = 0;
			MA = 1;
			delay_us(delay);
		}
	}	
}

