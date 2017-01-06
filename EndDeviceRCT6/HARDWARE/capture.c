//³¬Éù²¨²â¾à´«¸ĞÆ÷
#include "capture.h"

#include "led.h"
#include "timer.h"
#include "stepMotor.h"
#include "lcd.h"
#include "usart2.h"

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
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾İÉè¶¨²ÎÊı³õÊ¼»¯GPIOA.8
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
	
	
	//PB1 echo
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB1ÉèÖÃ³ÉÊäÈë£¬Ä¬ÈÏÏÂÀ­	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯GPIOA.0
	

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//Íâ²¿ÖĞ¶Ï£¬ĞèÒªÊ¹ÄÜAFIOÊ±ÖÓ

	  
    //GPIOB.1 ÖĞ¶ÏÏßÒÔ¼°ÖĞ¶Ï³õÊ¼»¯ÅäÖÃ
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//ÉÏÉıÑØ£¬ÏÂ½µÑØ´¥·¢
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//¸ù¾İEXTI_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//Ê¹ÄÜ°´¼üËùÔÚµÄÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//ÇÀÕ¼ÓÅÏÈ¼¶2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//×ÓÓÅÏÈ¼¶1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure); //¸ù¾İNVIC_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ
	
	TIM3_Int_Init(0xFFFF,71);//³õÊ¼»¯¶¨Ê±Æ÷ ÆµÂÊÎª1M£¬1us¼ÆÊıÒ»´Î
}

int state_dis = 0;//Ã»ÓĞ´¥·¢
u16 time_cnt = 0;
extern int is_lock_open;
extern char self_num[2];

int have_car = 0; //ÅĞ¶Ï³µÊÇ·ñµ½À´
int cal_ts = 0; //È¡¼¸´Î¼ÆËã¾àÀëµÄ½á¹û£¬ÇóÆ½¾ùÖµ
float cal_sum = 0, cal_average = 0;

int car_leave_confirm = 0, car_come_confirm = 0;
u16 get_dis(u16 ts)
{
	u16 dis = ts / 1000000.0 * 340 * 1000 / 2;
	return dis;
}

//¿ªÊ¼½øĞĞÒ»´Î²â¾à£¬²âÁ¿½á¹ûÔÚÍâ²¿ÖĞ¶ÏÖĞ»ñÈ¡
void start_cal_distance(void) 
{
	trig = 1;
	delay_us(45);
	trig = 0;

	state_dis = 1;//´¥·¢
}

char show[30] = {0};
unsigned char state_af[4] = {0x5a,0,1,0x53}; //ÏòĞ­µ÷Æ÷·¢ËÍ·´À¡×´Ì¬

int dis_come = 500, dis_leave = 800;//ÅĞ¶¨³µÁ¾µ½À´»òÕßÀë¿ªºóµÄ¼ì²â¾àÀë

//Íâ²¿ÖĞ¶Ï£¬¼ÆËãechoÒı½Å¸ßµçÆ½µÄÊ±¼ä
void EXTI1_IRQHandler(void)
{
	if(echo == 1 && state_dis == 1) //²¶»ñµ½ÉÏÉıÑØ
	{	  
		TIM3->CNT = 0;
		TIM_Cmd(TIM3, ENABLE);//¿ªÆô¶¨Ê±Æ÷
		state_dis = 2;//×¼±¸²¶»ñÏÂ½µÑØ
	}
	if(echo == 0 && state_dis == 2)//²¶»ñµ½ÏÂ½µÑØ
	{	  
		time_cnt = TIM3->CNT;//¸ßµçÆ½µÄ³ÖĞøÊ±¼ä
		
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
			sprintf(show, "dis: %.1f cm  ", cal_average / 10.0);
			LCD_P8x16Str(0,2,show);
			
			//is_lock_open == 0 && have_car == 0 && 
			if (is_lock_open == 0 && have_car == 0 && cal_average < dis_come) // < 1m
			{
				car_come_confirm++;
				if (car_come_confirm == 3) //3´ÎÈ·ÈÏ
				{
					car_come_confirm = 0;
					have_car = 1;//ÈÏÎªÓĞ³µÀ´
					//printf("\r\na car come in\r\n\r\n");
					
					LCD_P8x16Str(0,6,"car come  ");
				}
				
			} //is_lock_open == 0 && have_car == 1 && 
			else if (is_lock_open == 0 && have_car == 1 && cal_average > dis_leave) // > 1.2m
			{
				car_leave_confirm++;
				if (car_leave_confirm == 3) //3´ÎÈ·ÈÏ
				{
					car_leave_confirm = 0;
					have_car = 0;//ÈÏÎª³µÀë¿ªÁË
					is_lock_open = 1;//³µÎ»×Ô¶¯´ò¿ª
					//printf("\r\na car leave\r\n\r\n");
					
					LCD_P8x16Str(0,6,"car leave  ");
					LCD_P8x16Str(0,4,"lock open ");
					motor_run(0, 90);//³µÎ»Ëø´ò¿ª
					
					//ÏòĞ­µ÷Æ÷ »ã±¨³µÁ¾Àë¿ª ³µÎ»Ëø¿ªÆô
					state_af[1] = self_num[0]; //¸üĞÂ³µÎ»±àºÅ
					state_af[2] = self_num[1];
					USART2_send_data(state_af, 4);
				}
				
			}
			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line1);  //Çå³ıEXTI0ÏßÂ·¹ÒÆğÎ»
}

void TIM3_IRQHandler(void)   //TIM3ÖĞ¶Ï
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //¼ì²éÖ¸¶¨µÄTIMÖĞ¶Ï·¢ÉúÓë·ñ:TIM ÖĞ¶ÏÔ´ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //Çå³ıTIMxµÄÖĞ¶Ï´ı´¦ÀíÎ»:TIM ÖĞ¶ÏÔ´ 
		state_dis = 0;//»Ö¸´µ½³õÊ¼»¯×´Ì¬
		//printf("unkown\r\n");//³¤Ê±¼äÎ´ÊÕµ½·´Éä²¨£¬³¤¶ÈÎŞ·¨²âÁ¿
		LCD_P8x16Str(0,2,"dis:unkown");
		TIM_Cmd(TIM3, DISABLE);//¹Ø±Õ¶¨Ê±Æ÷
	}
}
