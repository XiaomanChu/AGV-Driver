#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "motor_driver.h"
#include "controler_capture.h"
#include "controler_logic.h"
#include "steering_driver.h"
#include	"SpdCap.h"
#include "key.h"

//#include "usmart.h"

u8 RxProcess(void);//串口数据处理
void TxProcess(u8 sendData);//串口数据发送
//void SetMotorPWM(u32 arry);
//void RemoteControler(void);

int main(void)
{  
	//u8 temp=0;int i=0;

	u8 keyval;//按键检测

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);//初始化串口波特率为115200
	Motor_Init();
 	PWMInit(500-1,84-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.  
  SetMotorPWM(0,500);//500对应0占空比
	KEY_Init();					//按键初始化
	TIM4_Cap_Init(0XFFFF,84-1); //以1Mhz的频率计数 ，溢出时间65536us,遥控器接收器最长脉宽2160us
  TIM3_Cap_Init(0x63,84-1); //以1Mhz的频率计，溢出时间100us
	
	Driver_Init();			//步进电机驱动器初始化
	TIM8_OPM_RCR_Init(999,168-1); //TIM8为168MHz所以是1MHz计数频率  单脉冲+重复计数模式  

   while(1) 
	{
 		//delay_ms(10);
		
#if 0    //PC端串口调试程序
		temp= RxProcess();
		if(USART_RX_BUF[0]!=0)printf("电机速率:%d\r\n",100-temp);
		//清除串口数据,准备下一次接收
		for(i=0;i<USART_RX_STA;i++)
		{
			USART_RX_BUF[i]=0;
		}
		USART_RX_STA=0;
#endif
		
		//遥控程序
		RemoteControler();
		//printf("motor 1: hz \r\n");
		//速度反馈程序
		//delay = SpeedFeedback(delay);

		
		//转向微调	
		keyval=KEY_Scan(0);
		if(keyval==KEY3_PRES)
		{
			printf("hello \r\n");
			Locate_Abs(0,7000);//按下KEY3，回零点
		}else if(keyval==KEY1_PRES)
		{
			Locate_Rle(1000,7500,CW);//按下KEY1，以7000Hz的频率 顺时针发1000脉冲, 
		}else if(keyval==KEY2_PRES)
		{
			Locate_Rle(1000,7500,CCW);//按下KEY2，以7000Hz的频率 逆时针发1000脉冲
		}			
		
	

	
	}	
}

//void SetMotorPWM(u32 arry)
//{   
//			double	alpha;//当前转向角
//			u32 duty= arry;
//			
//			if(flag==1){//右转
//				alpha = abs(current_pos)*0.00063;//每个脉冲转角约为0.00063度
//				duty=1.0/Diff_Calculation(alpha)*arry; //正常占空系数*内外圈速度比例
//				TIM_SetCompare1(TIM5,duty);
//				TIM_SetCompare2(TIM5,duty);
//			}
//			else if(flag==2){//左转
//				alpha = abs(current_pos)*0.00063;//每个脉冲转角约为0.00063度
//				duty=1.0/Diff_Calculation(alpha)*arry; //正常占空系数*内外圈速度比例
//				TIM_SetCompare3(TIM5,duty);
//				TIM_SetCompare4(TIM5,duty);
//				//printf("diff: %d \r\n",duty);
//			}
//			else
//			{
//				TIM_SetCompare1(TIM5,duty);
//				TIM_SetCompare2(TIM5,duty);
//				TIM_SetCompare3(TIM5,duty);
//				TIM_SetCompare4(TIM5,duty);
//			}
//}

//void RemoteControler() //遥控程序
//{
//		
//		k1=(float)(500-0)/(float)(216-87);//捕获的测量值是87~215，标定到0~500
//		k_right=(float)35000/(float)(21-15);//右转系数
//		k_left=(float)35000/(float)(15-8);//左转系数
//		count1=tim4up1/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
//		count2=tim4up2/10;
//		count3=tim4up3/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
//		count4=tim4up4/10;
//		
//		
//		//printf("HIGH:%lld us\r\n",count1); 
//		//printf("DIR: %d \r\n",count2);
//		//printf("DIR: %lld \n",count3);
//	
//	
//		//将捕获计数转换成PWM电机控制
//		//count1=count1/10;//
//		if(count1<300)
//		{
//			//printf("HIGH:%d us\r\n",tim4up1); 
//			count1=500-(int)(k1*(count1-87));//标定到（500，0）区间
//			//printf("duty:%d \r\n",count1); 
//			SetMotorPWM(flag,count1);
//		}
//		if(count2<109)
//		{
//			GPIO_SetBits(GPIOH,GPIO_Pin_9);//正转
//			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
//			GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使
//	
//		}
//		else if(count2>190&&count2<300)
//		{
//			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//反转
//			GPIO_SetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
//			GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使
//			
//		}
//		else if(count2>140&&count2<160) 
//		{
//			 GPIO_ResetBits(GPIOH,GPIO_Pin_15);//关闭使能
//			
//		}
//		
//		pst=count3/10;  //将遥控器输入标定到(8,21)
//		if(pst<300&&pst!=pst_old)//遥控方向有变化
//		{
//			dir_delay ++;
//			if(dir_delay==5000)//延时，防止频繁转动
//			{
//				if(pst!=pst_old)
//				{
//					dir_delay=0;
//					if(pst>15&&pst<30&&pst!=pst_old)//右转
//					{
//						pst_old=pst;
//						flag=1;
//						Locate_Abs(((int)k_right*(pst-15)),7000);
//						
//						
//					}
//					else if(pst>0&&pst<15&&pst!=pst_old)//左转
//					{
//						pst_old=pst;
//						flag=2;
//						Locate_Abs(-35000+(int)k_left*(pst-8),7000);
//						
//						
//					}
//					else if(pst==15&&pst!=pst_old)
//					{	
//						if(current_pos!=0)
//						{
//							flag=0;
//							Locate_Abs(0,7000);
//							
//							
//						}
//						pst_old =15;
//					}
//				}
//			}
//		}
//	

//}	
//u32 SpeedFeedback(u32 delay)
//{
//		 
//		delay ++;
//		if(delay == 5)
//		{
//			delay=0;
//			printf("[ motor 1: %d hz ]  ",f1);
//			printf("[ motor 2: %d hz ] \n",f2);
//			printf("[ motor 3: %d us ]  ",f3);
//			printf("[ motor 4: %d us ]\r\n",f4);
//		}
//	return delay;
//}
//u8 RxProcess(void)//串口数据处理
//{
//	u8 temp = 0;
//	float k;

//	switch(USART_RX_BUF[0])//选择模式
//	{
//		case 0x10://10暂定电机调速模式
//		{
//			//GPIO_ResetBits(GPIOH,GPIO_Pin_9);
//			GPIO_SetBits(GPIOH,GPIO_Pin_9);//正转
//			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
//			temp = USART_RX_BUF[1];
//			k=(float)(500-0)/(float)(100-0);
//			if(temp<=100)
//			{
//				SetMotorPWM(0,(u32)k*temp);
//				GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使�
//			}
//			else	printf("数据错误\r\n");
//		}break;
//		case 0x11:
//		{
//			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//反转
//			GPIO_SetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
//			//GPIO_SetBits(GPIOH,GPIO_Pin_9);
//			temp = USART_RX_BUF[1];
//			k=(float)(500-0)/(float)(100-0);
//			if(temp<=100)
//			{
//					SetMotorPWM(0,(u32)k*temp);	
//					GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使�
//			}
//		}break;
//		case 0X12:
//			GPIO_ResetBits(GPIOH,GPIO_Pin_15);break;//关闭使能
//			
//		case 0X13:
//			GPIO_ResetBits(GPIOH,GPIO_Pin_13);break;//刹车
//	}
//	return temp;//返回占空比
//}
//void TxProcess(u8 sendData)//串口数据发送
//{
//	switch(USART_RX_BUF[0])
//	{	
//		case 0x10:
//			USART_SendData(USART1,sendData);
//	}
//}
