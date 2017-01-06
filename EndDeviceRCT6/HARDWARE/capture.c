//��������ഫ����
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
	
	
	//PB1 echo
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB1���ó����룬Ĭ������	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.0
	

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��

	  
    //GPIOB.1 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����أ��½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��
	
	TIM3_Int_Init(0xFFFF,71);//��ʼ����ʱ�� Ƶ��Ϊ1M��1us����һ��
}

int state_dis = 0;//û�д���
u16 time_cnt = 0;
extern int is_lock_open;
extern char self_num[2];

int have_car = 0; //�жϳ��Ƿ���
int cal_ts = 0; //ȡ���μ������Ľ������ƽ��ֵ
float cal_sum = 0, cal_average = 0;

int car_leave_confirm = 0, car_come_confirm = 0;
u16 get_dis(u16 ts)
{
	u16 dis = ts / 1000000.0 * 340 * 1000 / 2;
	return dis;
}

//��ʼ����һ�β�࣬����������ⲿ�ж��л�ȡ
void start_cal_distance(void) 
{
	trig = 1;
	delay_us(45);
	trig = 0;

	state_dis = 1;//����
}

char show[30] = {0};
unsigned char state_af[4] = {0x5a,0,1,0x53}; //��Э�������ͷ���״̬

int dis_come = 500, dis_leave = 800;//�ж��������������뿪��ļ�����

//�ⲿ�жϣ�����echo���Ÿߵ�ƽ��ʱ��
void EXTI1_IRQHandler(void)
{
	if(echo == 1 && state_dis == 1) //����������
	{	  
		TIM3->CNT = 0;
		TIM_Cmd(TIM3, ENABLE);//������ʱ��
		state_dis = 2;//׼�������½���
	}
	if(echo == 0 && state_dis == 2)//�����½���
	{	  
		time_cnt = TIM3->CNT;//�ߵ�ƽ�ĳ���ʱ��
		
		state_dis = 0;//�ָ�����ʼ��״̬
		TIM_Cmd(TIM3, DISABLE);//�رն�ʱ��
		
		//printf("dis��%.1f cm\r\n", get_dis(time_cnt) / 10.0);
		
		cal_ts++;
		cal_sum += get_dis(time_cnt);
		
		if (cal_ts == 10) //�ۼ�10����ƽ��ֵ		 
		{
			cal_ts = 0;
			cal_average = cal_sum / 10.0;//10��
			cal_sum = 0;
			
			//printf("\r\ndis: %.1f cm\r\n", cal_average / 10.0);
			sprintf(show, "dis: %.1f cm  ", cal_average / 10.0);
			LCD_P8x16Str(0,2,show);
			
			//is_lock_open == 0 && have_car == 0 && 
			if (is_lock_open == 0 && have_car == 0 && cal_average < dis_come) // < 1m
			{
				car_come_confirm++;
				if (car_come_confirm == 3) //3��ȷ��
				{
					car_come_confirm = 0;
					have_car = 1;//��Ϊ�г���
					//printf("\r\na car come in\r\n\r\n");
					
					LCD_P8x16Str(0,6,"car come  ");
				}
				
			} //is_lock_open == 0 && have_car == 1 && 
			else if (is_lock_open == 0 && have_car == 1 && cal_average > dis_leave) // > 1.2m
			{
				car_leave_confirm++;
				if (car_leave_confirm == 3) //3��ȷ��
				{
					car_leave_confirm = 0;
					have_car = 0;//��Ϊ���뿪��
					is_lock_open = 1;//��λ�Զ���
					//printf("\r\na car leave\r\n\r\n");
					
					LCD_P8x16Str(0,6,"car leave  ");
					LCD_P8x16Str(0,4,"lock open ");
					motor_run(0, 90);//��λ����
					
					//��Э���� �㱨�����뿪 ��λ������
					state_af[1] = self_num[0]; //���³�λ���
					state_af[2] = self_num[1];
					USART2_send_data(state_af, 4);
				}
				
			}
			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line1);  //���EXTI0��·����λ
}

void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		state_dis = 0;//�ָ�����ʼ��״̬
		//printf("unkown\r\n");//��ʱ��δ�յ����䲨�������޷�����
		LCD_P8x16Str(0,2,"dis:unkown");
		TIM_Cmd(TIM3, DISABLE);//�رն�ʱ��
	}
}
