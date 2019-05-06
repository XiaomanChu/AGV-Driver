#include "controler_logic.h"
#include "controler_capture.h"
#include "motor_driver.h"
#include "steering_driver.h"


u32 count1=0;//计数值
u32 count2=0;//计数值
u32 count3=0;//计数值
u32 count4=0;//计数值
u32 pst=0,pst_old=0;
u8 dir=0;
float	k1,k_right,k_left;//捕获转换系数
u32 delay=0;//用于串口延时发送；
u32 dir_delay;//用于转向延时

void RemoteControler() //遥控程序
{
		
		k1=(float)(500-0)/(float)(216-87);//捕获的测量值是87~215，标定到0~500
		k_right=(float)35000/(float)(21-15);//右转系数
		k_left=(float)35000/(float)(15-8);//左转系数
		count1=tim4up1/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
		count2=tim4up2/10;
		count3=tim4up3/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
		count4=tim4up4/10;
		
		
		//printf("HIGH:%lld us\r\n",count1); 
		//printf("DIR: %d \r\n",count2);
		//printf("DIR: %lld \n",count3);
	
	
		//将捕获计数转换成PWM电机控制
		//count1=count1/10;//
		if(count1<300)
		{
			//printf("HIGH:%d us\r\n",tim4up1); 
			count1=500-(int)(k1*(count1-87));//标定到（500，0）区间
			//printf("duty:%d \r\n",count1); 
			SetMotorPWM(dir,count1);
		}
		if(count2<109)
		{
			GPIO_SetBits(GPIOH,GPIO_Pin_9);//正转
			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
			GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使
	
		}
		else if(count2>190&&count2<300)
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//反转
			GPIO_SetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
			GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使
			
		}
		else if(count2>140&&count2<160) 
		{
			 GPIO_ResetBits(GPIOH,GPIO_Pin_15);//关闭使能
			
		}
		
		pst=count3/10;  //将遥控器输入标定到(8,21)
		if(pst<300&&pst!=pst_old)//遥控方向有变化
		{
			dir_delay ++;
			if(dir_delay==5000)//延时，防止频繁转动
			{
				if(pst!=pst_old)
				{
					dir_delay=0;
					if(pst>15&&pst<30&&pst!=pst_old)//右转
					{
						pst_old=pst;
						dir=Right;
						Locate_Abs(k_right*(pst-15),7000);
						
						
					}
					else if(pst>0&&pst<15&&pst!=pst_old)//左转
					{
						pst_old=pst;
						dir=Left;
						
						Locate_Abs((-35000+k_left*(pst-8)),7000);
						
						
					}
					else if(pst==15&&pst!=pst_old)
					{	
						if(current_pos!=0)
						{
							dir=Advance;
							Locate_Abs(0,7000);
							
							
						}
						pst_old =15;
					}
				}
			}
		}
	

}	
