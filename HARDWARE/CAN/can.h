#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    

	
//CAN1接收RX0中断使能
#define CAN1_RX0_INT_ENABLE	1		//0,不使能;1,使能.								    


//u8 CanBuf[8];//接收缓冲


u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
u8 CAN1_Send_Msg(u8* msg,u8 len);						//发送数据
u8 CAN1_Receive_Msg(u8 *buf);							//接收数据
void Can_AGV_Control(u8 *CanReceive);//命令解析
void Can_Motor_Control(u8 *CanReceive);
void Can_Steering_Control(u8 *CanReceive);

extern FunctionalState Controler_ENABLE;//遥控器
#endif

















