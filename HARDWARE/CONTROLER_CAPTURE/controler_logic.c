/***************************************************
*此部分代码为遥控器控制逻辑
*根据捕获值进行小车行走和方向控制
*程序输出：
*						dir_flag
***************************************************/
#include "controler_logic.h"
#include "controler_capture.h"
#include "motor_driver.h"
#include "steering_driver.h"


u32 count1=0;//遥控器脉宽计数值
u32 count2=0;
u32 count3=0;
u32 count4=0;
u32 pst=0,pst_old=0;
u8 dir_flag=0;
float	k1,k_right,k_left;//捕获转换系数
u32 delay=0;//用于串口延时发送；
u32 dir_delay;//用于转向延时


void RemoteControler() //遥控程序
{
		
		k1=(float)(100-0)/(float)(216-87);//捕获的测量值是87~215，标定到0~100
		k_right=(float)35000/(float)(21-15);//右转系数
		k_left=(float)35000/(float)(15-8);//左转系数
		count1=tim4up1/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
		count2=tim4up2/10;
		count3=tim4up3/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
		count4=tim4up4/10;
		
	
		//将捕获计数转换成PWM电机控制
		
		if(count1<300&count1>85)
		{
			count1=(int)(k1*(count1-87));//标定到（0，100）区间
			SetMotorPWM(dir_flag,count1);
		}
		if(count2<109&&count2>85)
		{
			Motor_StatusInit.enable=MotorEnable;
			Motor_StatusInit.dir=FORWARD;  //前进
			Motor_StatusInit.Break=NoBREAK;
			Motor_Status_Setup(&Motor_StatusInit);
		}
		else if(count2>190&&count2<300)
		{
			Motor_StatusInit.enable=MotorEnable;
			Motor_StatusInit.dir=BACKWARD;   //后退
			Motor_StatusInit.Break=NoBREAK;
			Motor_Status_Setup(&Motor_StatusInit);	
		}
		else if(count2>140&&count2<160) 
		{
			Motor_StatusInit.enable=MotorDisable;
			Motor_Status_Setup(&Motor_StatusInit);//使能关闭
		}
		
		pst=count4/10;  //将遥控器输入标定到(8,21)
		if(pst<300&&pst!=pst_old)//遥控方向有变化
		{
			dir_delay ++;
			if(dir_delay==5000)//延时，防止频繁转动
			{
				dir_delay=0;
				if(pst!=pst_old)
				{
					if(pst>15&&pst<30&&pst!=pst_old)//右转
					{
						pst_old=pst;
						dir_flag=Right;
						Locate_Abs(k_right*(pst-15),7000);//绝对定位函数
						
						
					}
					else if(pst>0&&pst<15&&pst!=pst_old)//左转
					{
						pst_old=pst;
						dir_flag=Left;
						Locate_Abs((-35000+k_left*(pst-8)),7000);	
						
					}
					else if(pst==15&&pst!=pst_old)
					{	
						if(current_pos!=0)
						{
							dir_flag=Advance;
							Locate_Abs(0,7000);
							
							
						}
						pst_old =15;
					}
				}
			}
		}
	

}	
