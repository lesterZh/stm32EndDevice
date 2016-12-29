#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"
#define MA PCout(9)	// 
#define MB PCout(8)	// 
#define MC PCout(7)	// 
#define MD PCout(6)	//

void motor_init(void);
void motor_run(int direction, int angle);
void motor_run_half(void);
extern int delay;

#endif
