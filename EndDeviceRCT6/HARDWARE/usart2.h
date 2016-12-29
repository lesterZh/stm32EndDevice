#ifndef __USART2_H
#define __USART2_H

#include "sys.h"

void USART2_Init( u32 bound );
void USART2_send_data(unsigned char * data, int len);
void USART2_send_chars(char * chars);

#endif
