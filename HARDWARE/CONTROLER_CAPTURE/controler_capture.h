#ifndef __CAPTURE_H
#define __CAPTURE_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140504
//新增TIM14_PWM_Init函数,用于PWM输出
//V1.2 20140504
//新增TIM5_CH1_Cap_Init函数,用于输入捕获
////////////////////////////////////////////////////////////////////////////////// 	

extern u8  TIM4CH1_CAPTURE_STA;		//输入捕获状
extern u8  TIM4CH2_CAPTURE_STA;		//输入捕获状态		    				
extern u8  TIM4CH3_CAPTURE_STA;		//输入捕获状态		    				
extern u8  TIM4CH4_CAPTURE_STA;		//输入捕获状态		    				

extern u32 tim4up1;	//捕获总高电平时间
extern u32 tim4up2;	//捕获总高电平时间
extern u32 tim4up3;	//捕获总高电平时间
extern u32 tim4up4;	//捕获总高电平时间


//void TIM14_PWM_Init(u32 arr,u32 psc);
void TIM4_Cap_Init(u32 arr,u16 psc);

#endif























