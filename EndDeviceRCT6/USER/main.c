#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "timer.h"
#include "stepMotor.h"
#include "capture.h"
#include "usart2.h"
#include "lcd.h"


void keyFun(void);
int dir = 1;
int angle = 90;

char lcd_show[10] = {0};

char rec_buf[3] = {0};
char rec_index = 0;
char self_num[2] = {0x00, 0x03};//ͣ��λ���

//��λ��Ĭ���ǿ����ģ���ʱ�����޷�����
volatile int is_lock_open = 1; //0 close,  1 open


 int main(void)
 {	
 
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	USART2_Init(9600);
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ� 
	KEY_Init();
	TIM2_Int_Init(99,7199);//10Khz�ļ���Ƶ�ʣ�����99Ϊ10ms 
	
	motor_init();
	capture_init();
	
	LCD_Init() ;
	sprintf(lcd_show, "device:%d", self_num[1]);
	LCD_P8x16Str(0,0,lcd_show);
	LCD_P8x16Str(0,4,"lock open  ");
	 
	while(1)
	{
		
		keyFun();
		//motor_run();
		
	}	 
}

void keyFun(void)
{
	int t=KEY_Scan(0);		//�õ���ֵ
	switch(t)
	{				 
		case KEY1_PRES:
			printf("key1\r\n");
			break;
		case KEY2_PRES:
			printf("key2\r\n");
			dir = 0;
			motor_run(dir,angle);
			break;
		case KEY3_PRES:	
			printf("key3\r\n");
			dir = 1;
			motor_run(dir,angle);
			
			//trig = !trig;
			break;
		default:;
			delay_ms(10);	
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
		
		if (timer10ms % 300 == 0) 
		{
			//printf("angle %d\r\n",angle);  
			LED2=!LED2;
			LED3=!LED3;
			
			//USART2_send_chars("d2\r\n");
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
			
		}			
    } 
} 


void USART2_IRQHandler(void)    
{  
    static u8 recCommandFlag = 0;
	u8 res;
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)  
    {       
		res = USART_ReceiveData(USART2);	//��ȡ���յ�������
		//USART_SendData(USART2, res);
		//����Э�飺Э��ͷ0x5A + ��λ���  (2byte)+ ����  (1byte)
		if (res == 0x5A) //���ܵ�Э��ͷ0x5A
		{
			recCommandFlag = 1;
			return;
		} 
		
		//����ģʽ 
		if (recCommandFlag == 1)
		{
			rec_buf[rec_index++] = res;
			if (rec_index == 3) 
			{
				if (rec_buf[1] == self_num[1]) 
				{
					if (rec_buf[2] == 0x51 && is_lock_open == 0) // open lock ,�����޷�����
					{
						LCD_P8x16Str(0,4,"lock open ");
						is_lock_open = 1;
						motor_run(1, 90);
						
					} 
					else if (rec_buf[2] == 0x52 && is_lock_open == 1)// close lock���������Խ���
					{
						LCD_P8x16Str(0,4,"lock close");
						LCD_P8x16Str(0,6,"            ");
						is_lock_open = 0;
						motor_run(0, 90);
					}
				}
				
				recCommandFlag = 0;
				rec_index = 0;
			}
		}
		
				
    }
      
}

