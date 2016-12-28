//³¬Éù²¨²â¾à´«¸ÐÆ÷
#ifndef __CAPTURE_H
#define __CAPTURE_H	 
#include "sys.h"


#define trig PBout(0)
#define echo  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

void capture_init(void);
void start_cal_distance(void) ;

#endif
