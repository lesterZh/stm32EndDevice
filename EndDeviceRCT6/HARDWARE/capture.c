//³¬Éù²¨²â¾à´«¸ÐÆ÷
#include "capture.h"

#include "led.h"
#include "timer.h"
#include "stepMotor.h"
#include "lcd.h"

void capture_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	//PB0 trig
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //Ê¹ÄÜPA,PD¶Ë¿ÚÊ±ÖÓ
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PA.8 ¶Ë¿ÚÅäÖÃ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOA.8
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
	
	
	//PB1 echo
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB1ÉèÖÃ³ÉÊäÈë£¬Ä¬ÈÏÏÂÀ­	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯GPIOA.0
	

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//Íâ²¿ÖÐ¶Ï£¬ÐèÒªÊ¹ÄÜAFIOÊ±ÖÓ

	  
    //GPIOB.1 ÖÐ¶ÏÏßÒÔ¼°ÖÐ¶Ï³õÊ¼»¯ÅäÖÃ
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//ÉÏÉýÑØ£¬ÏÂ½µÑØ´¥·¢
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//¸ù¾ÝEXTI_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//Ê¹ÄÜ°´¼üËùÔÚµÄÍâ²¿ÖÐ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//ÇÀÕ¼ÓÅÏÈ¼¶2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//×ÓÓÅÏÈ¼¶1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖÐ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure); //¸ù¾ÝNVIC_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ
	
	TIM3_Int_Init(0xFFFF,71);//³õÊ¼»¯¶¨Ê±Æ÷ ÆµÂÊÎª1M£¬1us¼ÆÊýÒ»´Î
}

int state_dis = 0;//Ã»ÓÐ´¥·¢
u16 time_cnt = 0;
extern int open_lock;
int have_car = 0; //ÅÐ¶Ï³µÊÇ·ñµ½À´
int cal_ts = 0; //È¡¼¸´Î¼ÆËã¾àÀëµÄ½á¹û£¬ÇóÆ½¾ùÖµ
float cal_sum = 0, cal_average = 0;

int car_leave_confirm = 0, car_come_confirm = 0;
u16 get_dis(u16 ts)
{
	u16 dis = ts / 1000000.0 * 340 * 1000 / 2;
	return dis;
}

//¿ªÊ¼½øÐÐÒ»´Î²â¾à£¬²âÁ¿½á¹ûÔÚÍâ²¿ÖÐ¶ÏÖÐ»ñÈ¡
void start_cal_distance(void) 
{
	trig = 1;
	delay_us(45);
	trig = 0;

	state_dis = 1;//´¥·¢
}

char show[30] = {0};

//Íâ²¿ÖÐ¶Ï£¬¼ÆËãechoÒý½Å¸ßµçÆ½µÄÊ±¼ä
void EXTI1_IRQHandler(void)
{
	if(echo == 1 && state_dis == 1) //²¶»ñµ½ÉÏÉýÑØ
	{	  
		TIM3->CNT = 0;
		TIM_Cmd(TIM3, ENABLE);//¿ªÆô¶¨Ê±Æ÷
		state_dis = 2;//×¼±¸²¶»ñÏÂ½µÑØ
	}
	if(echo == 0 && state_dis == 2)//²¶»ñµ½ÏÂ½µÑØ
	{	  
		time_cnt = TIM3->CNT;//¸ßµçÆ½µÄ³ÖÐøÊ±¼ä
		
		state_dis = 0;//»Ö¸´µ½³õÊ¼»¯×´Ì¬
		TIM_Cmd(TIM3, DISABLE);//¹Ø±Õ¶¨Ê±Æ÷
		
		//printf("dis£º%.1f cm\r\n", get_dis(time_cnt) / 10.0);
		
		cal_ts++;
		cal_sum += get_dis(time_cnt);
		
		if (cal_ts == 10) //ÀÛ¼Æ10´ÎÇóÆ½¾ùÖµ		 
		{
			cal_ts = 0;
			cal_average = cal_sum / 10.0;//10´Î
			cal_sum = 0;
			
			//printf("\r\ndis: %.1f cm\r\n", cal_average / 10.0);
			sprintf(show, "dis: %.1f cm", cal_average / 10.0);
			LCD_P8x16Str(0,2,show);
			
			if (open_lock == 1 && have_car == 0 && cal_average < 1000) //Ð¡ÓÚ1m
			{
				car_come_confirm++;
				if (car_come_confirm == 3) //3´ÎÈ·ÈÏ
				{
					car_come_confirm = 0;
					have_car = 1;//ÈÏÎªÓÐ³µÀ´
					//printf("\r\na car come in\r\n\r\n");
					sprintf(show, "a car come in");
					LCD_P8x16Str(0,2,show);
				}
				
			} 
			else if (open_lock == 1 && have_car == 1 && cal_average > 1500)
			{
				car_leave_confirm++;
				if (car_leave_confirm == 3) //3´ÎÈ·ÈÏ
				{
					car_leave_confirm = 0;
					have_car = 0;//ÈÏÎª³µÀë¿ªÁË
					open_lock = 0;//³µÎ»Ëø¹Ø±Õ
					//printf("\r\na car leave\r\n\r\n");
					sprintf(show, "a car leave");
					LCD_P8x16Str(0,2,show);
					motor_run(0, 90);//³µÎ»Ëø¹Ø±Õ
				}
				
			}
			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line1);  //Çå³ýEXTI0ÏßÂ·¹ÒÆðÎ»
}

void TIM3_IRQHandler(void)   //TIM3ÖÐ¶Ï
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //¼ì²éÖ¸¶¨µÄTIMÖÐ¶Ï·¢ÉúÓë·ñ:TIM ÖÐ¶ÏÔ´ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //Çå³ýTIMxµÄÖÐ¶Ï´ý´¦ÀíÎ»:TIM ÖÐ¶ÏÔ´ 
		state_dis = 0;//»Ö¸´µ½³õÊ¼»¯×´Ì¬
		//printf("unkown\r\n");//³¤Ê±¼äÎ´ÊÕµ½·´Éä²¨£¬³¤¶ÈÎÞ·¨²âÁ¿
		LCD_P8x16Str(0,2,"dis:unkown");
		TIM_Cmd(TIM3, DISABLE);//¹Ø±Õ¶¨Ê±Æ÷
	}
}
