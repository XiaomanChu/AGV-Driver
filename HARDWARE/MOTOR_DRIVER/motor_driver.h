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


//端口定义
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	

//状态定义
#define FORWARD ENABLE
#define BACKWARD 	DISABLE
#define MotorEnable ENABLE
#define MotorDisable DISABLE
#define BREAK ENABLE
#define NoBREAK	DISABLE

#define Right 1
#define Left 2
#define Advance 0

typedef enum{
	True = 1,
	False = 0
}bool;

typedef	struct{
	FunctionalState enable;
	FunctionalState dir;
	FunctionalState Break;
	u32 Acceleration;
}Motor_StatusDef;//电机状态定义

void Motor_Init(void);//初始化	
void PWMInit(u32 arr,u32 psc);
void SetMotorPWM(u8 flag,u32 arry);
void Motor_Status_Setup(Motor_StatusDef* StatusSetup);

extern double Diff_Calculation(double alpha);
extern Motor_StatusDef Motor_StatusInit;
#endif
