#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//LED端口定义
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	 
#define FORWARD 1
#define BACK 	0
#define MotorEnable 1
#define MotorDisable 0
#define BREAK 1
#define NoBREAK	0

#define Right 1
#define Left 2
#define Advance 0

void IO_Init(void);//初始化	
void PWMInit(u32 arr,u32 psc);
void SetMotorPWM(u8 flag,u32 arry);
void Motor_Status_Setup(u8 en,u8 dir,u8 bk,u32 speed);

extern double Diff_Calculation(double alpha);

#endif
