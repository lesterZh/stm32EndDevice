#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"

#define MA PBout(5)	// PB5
#define MB PBout(6)	// PB6	
#define MC PBout(7)	// PB7
#define MD PBout(8)	// PB8

void motor_init(void);
void motor_run(int direction, int angle);
void motor_run_half(void);
extern int delay;

#endif
