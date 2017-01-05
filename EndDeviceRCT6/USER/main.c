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
char self_num[2] = {0x00, 0x01};//停车位编号 左边是高位，右边是低位 ，当前只使用了低位

//车位锁默认是开启的，此时车辆无法进来
volatile int is_lock_open = 1; //0 close,  1 open

u8 temp=0, humi=0;
u8 dht11_lcd[16] = {0};

u16 save_self_num = 0;//默认代表低位
u8 clean_str[] = "               ";

 int main(void)
 {	
 
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(9600);	 //串口初始化为9600
	USART2_Init(9600);
	LED_Init();		  	 //初始化与LED连接的硬件接口 
	KEY_Init();
	TIM2_Int_Init(99,7199);//10Khz的计数频率，计数99为10ms 
	
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

//递增设置本节点编号
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
	int t=KEY_Scan(0);		//得到键值
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
			LED2=!LED2;
			//LED3=!LED3;
			
			//USART2_send_chars("d2\r\n");

			DHT11_Read_Data(&temp, &humi);
			sprintf(dht11_lcd, "t:%dC ",temp);
			LCD_show_temp(dht11_lcd);
			sprintf(dht11_lcd, "h:%d%%", humi);
			LCD_show_humi(dht11_lcd);
			
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
	static u8 motor_flag = 0;
	static u8 self_num_flag = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if (res == 0x51) //设置本节点编号
		{
			self_num_flag = 1;
			return;
		}
		
		if (res == 0x52) //电机正向转动命令
		{
			motor_flag = 1;
			return;
		}

		if (res == 0x53) //电机反向转动命令
		{
			motor_flag = 2;
			return;
		}
		
		if (motor_flag == 1) //电机正向转动执行
		{
			motor_flag = 0;
			motor_run(1,res);
			return;
		}

		if (motor_flag == 2) //电机反向转动执行
		{
			motor_flag = 0;
			motor_run(0,res);
			return;
		}
		
		if (self_num_flag == 1) //设置本节点编号
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
		res = USART_ReceiveData(USART2);	//读取接收到的数据
		//USART_SendData(USART2, res);
		//整个协议：协议头0x5A + 车位编号  (2byte)+ 命令  (1byte)
		if (res == 0x5A) //接受到协议头0x5A
		{
			recCommandFlag = 1;
			return;
		} 
		
		//命令模式 
		if (recCommandFlag == 1)
		{
			rec_buf[rec_index++] = res;
			if (rec_index == 3) 
			{
				if (rec_buf[1] == self_num[1]) 
				{
					if (rec_buf[2] == 0x51 && is_lock_open == 0) // open lock ,车辆无法进来
					{
						LCD_P8x16Str(0,4,"lock open ");
						is_lock_open = 1;
						motor_run(1, 90);
						
					} 
					else if (rec_buf[2] == 0x52 && is_lock_open == 1)// close lock，车辆可以进来
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

