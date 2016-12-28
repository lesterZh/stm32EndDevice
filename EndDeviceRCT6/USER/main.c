#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "timer.h"
#include "stepMotor.h"
#include "capture.h"
#include "usart2.h"

void keyFun(void);
int dir = 1;
int angle = 90;
int open_lock = 0; //��λ���Ƿ���

 int main(void)
 {	
	u8 t;
	u8 len;	 
 
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	USART2_Init(9600);
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ� 
	KEY_Init();
	TIM2_Int_Init(99,7199);//10Khz�ļ���Ƶ�ʣ�����99Ϊ10ms 
	
	motor_init();
	capture_init();
	 
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
		}
		keyFun();
		//motor_run();
		
	}	 
}

void keyFun(void)
{
	int t=KEY_Scan(0);		//�õ���ֵ
	switch(t)
	{				 
		case KEY0_PRES:
			printf("%d\r\n",TIM3->CNT);
			break;
		case KEY1_PRES:
			
			break;
		case WKUP_PRES:	
			//motor_run(dir,angle);
			
			//trig = !trig;
			break;
		default:;
			//delay_ms(10);	
	}
}

unsigned int timer10ms = 0;
void TIM2_IRQHandler(void)   //TIM3�ж�
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		timer10ms++;
		if (timer10ms % 50 == 0) 
		{
			LED1=!LED1;
		}
		
		if (timer10ms % 100 == 0) 
		{
			//printf("angle %d\r\n",angle);  
		}
		
		if (timer10ms % 10 == 0) //1s��10��
		{
			//printf("angle %d\r\n",angle);
			start_cal_distance();			
		}
	}
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		if (res == 0x55)
		{
			if (open_lock == 0) 
			{
				printf("open lock\r\n");
				motor_run(1, 90);
				open_lock = 1;
			}
			
		}			
    } 
} 

void USART2_IRQHandler(void)    
{  
    u8 res;
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)  
    {       
		res = USART_ReceiveData(USART2);	//��ȡ���յ�������
		//USART_SendData(USART2, res);
		if (res == 0x55)
		{
			if (open_lock == 0) 
			{
				printf("open lock\r\n");
				motor_run(1, 90);
				open_lock = 1;
			}
			
		}		
    }
      
}

