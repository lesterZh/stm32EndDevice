#include "usart2.h"

void USART2_Init( u32 bound )
{
    GPIO_InitTypeDef GPIO_InitStructure;//I/O?¨²3?¨º??¡¥?¨¢11¨¬?
    NVIC_InitTypeDef NVIC_InitStructure;//?D??3?¨º??¡¥?¨¢11¨¬?
    USART_InitTypeDef USART_InitStructure;//¡ä??¨²3?¨º??¡¥?¨¢11¨¬?  
    
    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
    
    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = bound;                //¨°?¡ã?¨¦¨¨???a9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//¡Á?3¡è?a8??¨ºy?Y??¨º?
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     //¨°???¨ª¡ê?1??
    USART_InitStructure.USART_Parity = USART_Parity_No;        //?T????D¡ê?¨¦??
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Y¨¢¡Â????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//¨º?¡¤¡é?¡ê¨º?   
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
}

void USART2_send_chars( char * chars)
{
	 
    while (*chars)   
    {  
        USART_SendData(USART2, *chars);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	
		chars++;
    }  

}


void USART2_send_data(unsigned char * data, int len) 
{
	int i = 0;
	for (i=0; i<len; i++) 
	{
		USART_SendData(USART2, data[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	
	}
}

