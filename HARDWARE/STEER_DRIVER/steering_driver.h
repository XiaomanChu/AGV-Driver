#ifndef __STEERING_DRIVER_H
#define __STEERING_DRIVER_H
#include "sys.h"
#include "stdlib.h"	


/********** 驱动器 端口定义 **************
//DRIVER_DIR   PE5 (DCMI_D6)
//DRIVER_OE    PE6 (DCMI_D7)
//STEP_PULSE   PC7 (TIM8_CH2  DCMI_D1)
******************************************/
#define DRIVER_DIR1   PDout(7) // 旋转方向1
#define DRIVER_DIR2   PFout(11) // 旋转方向1
#define DRIVER_OE    PDout(3) // 使能脚 低电平有效 
#define RCR_VAL    255  //每计数（RCR_VAL+1）次，中断一次，这个值（0~255）设置大一些可以降低中断频率

typedef enum
{
	CW = 1,//高电平顺时针
	CCW = 0,//低电平逆时针
}DIR_Type;//运行方向

extern long target_pos;//有符号方向
extern long current_pos;//有符号方向

void Driver_Init(void);//驱动器初始化
void TIM8_OPM_RCR_Init(u16 arr,u16 psc);//TIM8_CH2 单脉冲输出+重复计数功能初始化
void TIM8_Startup(u32 frequency);   //启动定时器8
void Locate_Rle(long num,u32 frequency,DIR_Type dir); //相对定位函数
void Locate_Abs(long num,u32 frequency);//绝对定位函数

#endif


