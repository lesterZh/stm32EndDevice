#include "stepMotor.h"
#include "delay.h"

void motor_init(void)
{ 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB端口时钟
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;				 //MA-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	 GPIO_ResetBits(GPIOC, GPIO_Pin_9);						 //PB.5 输出低

	 GPIO_ResetBits(GPIOC,GPIO_Pin_6);					

	 GPIO_ResetBits(GPIOC,GPIO_Pin_7);						  

	 GPIO_ResetBits(GPIOC,GPIO_Pin_8);						  	
}

int delay = 4000;//


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

//direction=1 正转，0 反转 ，64次约45度 128次90度
//8*8=64拍，电机(内圈)转动360°，外圈(轴)转动5.625度
void motor_run(int direction, int angle)
{
	int ts = (128 / 90.0) * angle, i=0;
	for (; i<ts; i++) 
	{
		if (direction == 1) //正转,8拍，电机(内圈)转动45度，外圈(轴)转动45/64度
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
		} else { //反转
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

