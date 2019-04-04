#include "GPIO.h" 
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

//PB0,PB1为电机转向控制,高电平为正转
//PA0为电机使能信号,高电平使能
//LED IO初始化
void IO_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);//使能GPIO时钟
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	//GPIO_ResetBits(GPIOC,GPIO_Pin_7);//方向信号,置低,灯灭
	GPIO_ResetBits(GPIOH,GPIO_Pin_15);//使能信号EN默认高电平/不使能
  GPIO_SetBits(GPIOH,GPIO_Pin_9);				//方向正转
	GPIO_ResetBits(GPIOH,GPIO_Pin_14);					//方向正转
	GPIO_SetBits(GPIOH,GPIO_Pin_13);		//刹车信号
	//GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);//默认左右都是正转

	
}






