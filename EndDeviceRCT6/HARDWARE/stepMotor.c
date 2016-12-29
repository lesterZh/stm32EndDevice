#include "stepMotor.h"
#include "delay.h"

void motor_init(void)
{ 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;				 //MA-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	 GPIO_ResetBits(GPIOC, GPIO_Pin_9);						 //PB.5 �����

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

//direction=1 ��ת��0 ��ת ��64��Լ45�� 128��90��
//8*8=64�ģ����(��Ȧ)ת��360�㣬��Ȧ(��)ת��5.625��
void motor_run(int direction, int angle)
{
	int ts = (128 / 90.0) * angle, i=0;
	for (; i<ts; i++) 
	{
		if (direction == 1) //��ת,8�ģ����(��Ȧ)ת��45�ȣ���Ȧ(��)ת��45/64��
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
		} else { //��ת
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

