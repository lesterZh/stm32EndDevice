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
int open_lock = 0; //车位锁是否开启

 int main(void)
 {	
	u8 t;
	u8 len;	 
 
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(9600);	 //串口初始化为9600
	USART2_Init(9600);
	LED_Init();		  	 //初始化与LED连接的硬件接口 
	KEY_Init();
	TIM2_Int_Init(99,7199);//10Khz的计数频率，计数99为10ms 
	
	motor_init();
	capture_init();
	 
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART_RX_BUF[t];
				while((USART1->SR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}
		keyFun();
		//motor_run();
		
	}	 
}

void keyFun(void)
{
	int t=KEY_Scan(0);		//得到键值
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
void TIM2_IRQHandler(void)   //TIM3中断
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		timer10ms++;
		if (timer10ms % 50 == 0) 
		{
			LED1=!LED1;
		}
		
		if (timer10ms % 100 == 0) 
		{
			//printf("angle %d\r\n",angle);  
		}
		
		if (timer10ms % 10 == 0) //1s测10次
		{
			//printf("angle %d\r\n",angle);
			start_cal_distance();			
		}
	}
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		res =USART_ReceiveData(USART1);	//读取接收到的数据
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
		res = USART_ReceiveData(USART2);	//读取接收到的数据
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

