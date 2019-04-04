#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "GPIO.h"
#include "pwm.h"
#include "capture.h"
#include	"driver.h"
#include	"SpdCap.h"
#include "key.h"
//#include "usmart.h"

u8 RxProcess(void);//串口数据处理
void TxProcess(u8 sendData);//串口数据发送
int main(void)
{ 
	//u16 led0pwmval=0;    
	u8 temp=0;int i=0;
	u32 delay=0;//用于串口延时发送；
	u8 keyval;//按键检测
	long long count1=0;//计数值
	long long count2=0;//计数值
	long long count3=0;//计数值
	long long count4=0;//计数值
	int pst=0,pst_old=0;
	float	k1,k2,k3;//捕获转换系数
	k1=(float)(500-0)/(float)(215-87);//捕获的测量值是87~215，标定到0~500
	k2=(float)50000/(float)(21-15);
	k3=(float)50000/(float)(15-8);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);//初始化串口波特率为115200
	IO_Init();
 	PWMInit(500-1,84-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.  
  TIM_SetCompare1(TIM5,500);	//修改比较值，修改占空比, 500对应占空比0
	TIM_SetCompare2(TIM5,500);
	TIM_SetCompare3(TIM5,500);
	TIM_SetCompare4(TIM5,500);
	
	KEY_Init();					//按键初始化
	//usmart_dev.init(84); 	//初始化USMART			
	TIM4_Cap_Init(0XFFFFF,84-1); //以1Mhz的频率计数 ，溢出时间65535us,遥控器接收器最长脉宽2160us
  TIM3_Cap_Init(0xffff,84-1); //以1Mhz的频率计
	
	Driver_Init();			//步进电机驱动器初始化
	TIM8_OPM_RCR_Init(999,168-1); //TIM8为168MHz所以是1MHz计数频率  单脉冲+重复计数模式  
#if 1	
   while(1) 
	{
 		delay_ms(10);
		
#if 1    //PC端串口调试程序
		temp= RxProcess();
		if(USART_RX_BUF[0]!=0)printf("电机速率:%d\r\n",100-temp);
		//清除串口数据,准备下一次接收
		for(i=0;i<USART_RX_STA;i++)
		{
			USART_RX_BUF[i]=0;
		}
		USART_RX_STA=0;
#endif
		
#if 1 //遥控程序
		delay_ms(10);		 

		count1=tim4up1/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
		count2=tim4up2/10;
		count3=tim4up3/10;		   //去掉最后一位不稳定的数（处理之后范围大概在87~215）
		count4=tim4up4/10;
		
		
	//printf("HIGH:%lld us\r\n",count1); 
		//printf("DIR: %lld \n",count2);
		//printf("DIR: %lld \n",count3);
	
	
		//将捕获计数转换成PWM电机控制
		//count1=count1/10;//
		if(count1<300)
		{
			count1=500-(int)(k1*(count1-87));//标定到（500，0）区间
			TIM_SetCompare1(TIM5,count1);
			TIM_SetCompare2(TIM5,count1);
			TIM_SetCompare3(TIM5,count1);
			TIM_SetCompare4(TIM5,count1);
		}
		if(count2<109)
		{
			GPIO_SetBits(GPIOH,GPIO_Pin_9);//正转
			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
			GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使�
		}
		else if(count2>194&&count2<300)
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//反转
			GPIO_SetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
			GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使�
		}
		else if(count2>145&&count2<160) 
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_15);//关闭使能
			
		}
		
		pst=count3/10;  //将遥控器输入标定到(8,21)
		int flag=0;			//实际位置与设定位置一致
		if(current_pos==target_pos)//判断当前位置与设定位置
		{
			delay_ms(100);
			flag=1;
		}
		else flag=0;
		if(pst>15&&pst<30&&flag&&pst!=pst_old)
		{
			//printf("PST:%lld us\r\n",(long long)pst);
			Locate_Abs((int)k2*(pst-15),7000);
			pst_old=pst;
			//printf("PST_OLD:%lld us\r\n",(long long)pst_old);
		
		}
		else if(pst>0&&pst<15&&flag&&pst!=pst_old)
		{
			//printf("PST:%lld us\r\n",(long long)pst);
			Locate_Abs(-50000+(int)k3*(pst-8),7000);
			pst_old=pst;
			//printf("PST_OLD:%lld us\r\n",(long long)pst_old);
		}
		else if(pst==15&&flag&&pst!=pst_old)
		{
			//printf("PST:%lld us\r\n",(long long)pst);
			if(current_pos!=0)
			{
				Locate_Abs(0,7000);
				pst_old =0;
			}
			//printf("PST_OLD:%lld us\r\n",(long long)pst_old);
		}
	
#endif
		
#if 0 //速度反馈程序
		delay ++;
		if(delay == 100)
		{
			delay=0;
			printf("[ motor 1: %d rpm ]  ",f1);
			printf("[ motor 2: %d rpm ] \n",f2);
			printf("[ motor 3: %d hz ]  ",f3);
			printf("[ motor 4: %d hz ]\r\n",f4);
		}
#endif
		keyval=KEY_Scan(0);
		if(keyval==KEY3_PRES)
		{
			Locate_Abs(0,500);//按下WKUP，回零点
		}else if(keyval==KEY0_PRES)
		{
			Locate_Rle(200,500,CW);//按下KEY0，以500Hz的频率 顺时针发200脉冲
		}else if(keyval==KEY1_PRES)
		{
			Locate_Rle(400,500,CCW);//按下KEY1，以500Hz的频率 逆时针发400脉冲
		}			
		delay_ms(10);
	}
#endif
}


u8 RxProcess(void)//串口数据处理
{
	u8 temp = 0;
	float k;

	switch(USART_RX_BUF[0])//选择模式
	{
		case 0x10://10暂定电机调速模式
		{
			//GPIO_ResetBits(GPIOH,GPIO_Pin_9);
			GPIO_SetBits(GPIOH,GPIO_Pin_9);//正转
			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
			temp = USART_RX_BUF[1];
			k=(float)(500-0)/(float)(100-0);
			if(temp<=100)
			{
				TIM_SetCompare1(TIM5,(int)k*temp);
				TIM_SetCompare2(TIM5,(int)k*temp);
				TIM_SetCompare3(TIM5,(int)k*temp);
				TIM_SetCompare4(TIM5,(int)k*temp);
				GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使�
			}
			else	printf("数据错误\r\n");
		}break;
		case 0x11:
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//反转
			GPIO_SetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//不刹车
			//GPIO_SetBits(GPIOH,GPIO_Pin_9);
			temp = USART_RX_BUF[1];
			k=(float)(500-0)/(float)(100-0);
			if(temp<=100)
			{
					TIM_SetCompare1(TIM5,(int)k*temp);
					TIM_SetCompare2(TIM5,(int)k*temp);
					TIM_SetCompare3(TIM5,(int)k*temp);
					TIM_SetCompare4(TIM5,(int)k*temp);
					GPIO_SetBits(GPIOH,GPIO_Pin_15);//电机使�
			}
		}break;
		case 0X12:
			GPIO_ResetBits(GPIOH,GPIO_Pin_15);break;//关闭使能
			
		case 0X13:
			GPIO_ResetBits(GPIOH,GPIO_Pin_13);break;//刹车
	}
	return temp;//返回占空比
}
void TxProcess(u8 sendData)//串口数据发送
{
	switch(USART_RX_BUF[0])
	{	
		case 0x10:
			USART_SendData(USART1,sendData);
	}
}
