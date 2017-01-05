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
#include "dht11.h"
#include "stmflash.h"


void keyFun(void);
int dir = 1;
int angle = 90;

char lcd_show[10] = {0};

char rec_buf[3] = {0};
char rec_index = 0;
char self_num[2] = {0x00, 0x01};//ͣ��λ��� ����Ǹ�λ���ұ��ǵ�λ ����ǰֻʹ���˵�λ

//��λ��Ĭ���ǿ����ģ���ʱ�����޷�����
volatile int is_lock_open = 1; //0 close,  1 open

u8 temp=0, humi=0;
u8 dht11_lcd[16] = {0};

u16 save_self_num = 0;//Ĭ�ϴ����λ
u8 clean_str[] = "               ";

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
	LED3=1;
	
	read_flash_self_num(&save_self_num);
	self_num[1] = save_self_num;
	sprintf(lcd_show, "device:%d", self_num[1]);
	LCD_P8x16Str(0,0,clean_str);
	LCD_P8x16Str(0,0,lcd_show);
	LCD_P8x16Str(0,4,"lock open  ");
	
	 //DHT11_Init();
	 
	if (DHT11_Init() == 0) 
	{
		LCD_show4("DHT11 ok");
	} else
	{
		LCD_show4("DHT11 error");
	}
	
	while(1)
	{
		
		keyFun();
		//motor_run();
		
	}	 
}

//�������ñ��ڵ���
void set_self_num()
{
	static u8 tmp = 0;
	tmp++;
	self_num[1] = tmp;
	write_flash_self_num(self_num[1]);
	sprintf(lcd_show, "device:%d", self_num[1]);
	LCD_P8x16Str(0,0,clean_str);
	LCD_P8x16Str(0,0,lcd_show);
}

void keyFun(void)
{
	int t=KEY_Scan(0);		//�õ���ֵ
	switch(t)
	{				 
		case KEY1_PRES:
			printf("key1 \r\n");
			set_self_num();
		
			break;
		
		case KEY2_PRES:
			
			motor_run(0,2);
			break;
		
		case KEY3_PRES:	
			motor_run(1,2);
			break;
		
		default:;
			delay_ms(10);	
	}
}

void LCD_show_temp(u8 * data) 
{
	LCD_P8x16Str(80, 4, "      ");
	LCD_P8x16Str(80, 4, data);
}

void LCD_show_humi(u8 * data) 
{
	LCD_P8x16Str(80, 6, "      ");
	LCD_P8x16Str(80, 6, data);
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
			LED2=!LED2;
			//LED3=!LED3;
			
			//USART2_send_chars("d2\r\n");

			DHT11_Read_Data(&temp, &humi);
			sprintf(dht11_lcd, "t:%dC ",temp);
			LCD_show_temp(dht11_lcd);
			sprintf(dht11_lcd, "h:%d%%", humi);
			LCD_show_humi(dht11_lcd);
			
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
	static u8 motor_flag = 0;
	static u8 self_num_flag = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if (res == 0x51) //���ñ��ڵ���
		{
			self_num_flag = 1;
			return;
		}
		
		if (res == 0x52) //�������ת������
		{
			motor_flag = 1;
			return;
		}

		if (res == 0x53) //�������ת������
		{
			motor_flag = 2;
			return;
		}
		
		if (motor_flag == 1) //�������ת��ִ��
		{
			motor_flag = 0;
			motor_run(1,res);
			return;
		}

		if (motor_flag == 2) //�������ת��ִ��
		{
			motor_flag = 0;
			motor_run(0,res);
			return;
		}
		
		if (self_num_flag == 1) //���ñ��ڵ���
		{
			self_num_flag = 0;
			self_num[1] = res;
			write_flash_self_num(res);
			sprintf(lcd_show, "device:%d", self_num[1]);
			LCD_P8x16Str(0,0,clean_str);
			LCD_P8x16Str(0,0,lcd_show);
			return;
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

