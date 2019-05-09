#include "SpdCap.h"
#include "motor_driver.h"
#include "usart.h"
#include "sys.h"

u32 TIM3_CNT=0;
bool RisingFlag_EXIT0 =False;//上升沿flag
bool RisingFlag_EXIT1 =False;
bool RisingFlag_EXIT2 =False;
bool RisingFlag_EXIT3 =False;
u32 EXIT0_CNT;//外部中断计数
u32 EXIT1_CNT;
u32 EXIT2_CNT;
u32 EXIT3_CNT;
u32 EXIT0_TIME;//周期计算时间
u32 EXIT1_TIME;
u32 EXIT2_TIME;
u32 EXIT3_TIME;
u32 EXIT0_SPEED;//速度计算
u32 EXIT1_SPEED;
u32 EXIT2_SPEED;
u32 EXIT3_SPEED;

u8 num1,num2,num3,num0;//用于计算平均值

u32 EXIT0_SPEEDS[15]={0};


TIM_ICInitTypeDef TIM3_ICInitStructure;

void TIM3_Cap_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);  //使能GPIOG时钟
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3;  //PG0,1,2,3 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉，IO默认低电平
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PG
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能 SYSCFG 时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource0);//将PG0连接EXTI0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource1);//将PG1连接EXTI1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource2);//将PG2连接EXTI2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource3);//将PG3连接EXTI3
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置

	
	//初始化定时器3 TIM11	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器 3 更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器 3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// 初始化 NVIC

	TIM_Cmd(TIM3,ENABLE); //使能定时器 3
	
	


}

//速度计算程序

u32 Speed_Cal(u32 time)
{
	u32 f,speed;
	f= 1000000/time;
	speed=f;
	return speed;
	
}


//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) 		//发生溢出中断
		{
				if(RisingFlag_EXIT0==1)
				TIM3_CNT++;
				if(TIM3_CNT>0xFFFF)TIM3_CNT=0;
		}
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
}

//外部中断0服务程序
void EXTI0_IRQHandler(void)
{	
		if(RisingFlag_EXIT0 == True)//第2个上升沿
		{
			if(TIM3_CNT>EXIT0_CNT)
			{
				EXIT0_CNT=TIM3_CNT-EXIT0_CNT;//更新在此期间的溢出值
				EXIT0_TIME=100*EXIT0_CNT;
				EXIT0_CNT=0;	
				EXIT0_SPEED+= Speed_Cal(EXIT0_TIME);
				num0++;
				if(num0==20){
					num0=0;
					EXIT0_SPEED=EXIT0_SPEED/20;
					printf("[ motor 0: %d  \r\n]  ", EXIT0_SPEED);
				}
			}			
			RisingFlag_EXIT0=False;			
		}
		else if( RisingFlag_EXIT0==0)//第一次检测到上升沿
		{
			TIM3->CNT=0;
			EXIT0_CNT=TIM3_CNT;//记录当前的溢出值
			RisingFlag_EXIT0 = True;
		}
		
	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}	
//外部中断1服务程序
void EXTI1_IRQHandler(void)
{
		if(RisingFlag_EXIT1 == True)//第2个上升沿
		{
			if(TIM3_CNT>EXIT1_CNT)
			{
				EXIT1_CNT=TIM3_CNT-EXIT1_CNT;//更新在此期间的溢出值
				EXIT1_TIME=100*EXIT1_CNT;
				EXIT1_CNT=0;	
				EXIT1_SPEED+= Speed_Cal(EXIT0_TIME);
				num1++;
				if(num1==20){
					num1=0;
					EXIT1_SPEED=EXIT1_SPEED/20;
					printf("[ motor 1: %d  \r\n]  ", EXIT1_SPEED);
				}
			}			
			RisingFlag_EXIT1=False;			
		}
		else if( RisingFlag_EXIT1==0)//第一次检测到上升沿
		{
			TIM3->CNT=0;
			EXIT1_CNT=TIM3_CNT;//记录当前的溢出值
			RisingFlag_EXIT1 = True;
		}
	 EXTI_ClearITPendingBit(EXTI_Line1);//清除LINE2上的中断标志位 
}
//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
		if(RisingFlag_EXIT2 == True)//第2个上升沿
		{
			if(TIM3_CNT>EXIT2_CNT)
			{
				EXIT2_CNT=TIM3_CNT-EXIT2_CNT;//更新在此期间的溢出值
				EXIT2_TIME=100*EXIT2_CNT;
				EXIT2_CNT=0;	
				EXIT2_SPEED+= Speed_Cal(EXIT2_TIME);
				num2++;
				if(num2==20){
					num2=0;
					EXIT2_SPEED=EXIT2_SPEED/20;
					printf("[ motor 2: %d  \r\n]  ", EXIT2_SPEED);
				}
			}			
			RisingFlag_EXIT2=False;			
		}
		else if( RisingFlag_EXIT2==0)//第一次检测到上升沿
		{
			TIM3->CNT=0;
			EXIT2_CNT=TIM3_CNT;//记录当前的溢出值
			RisingFlag_EXIT2 = True;
		}
	 EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE3上的中断标志位  
}
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
		if(RisingFlag_EXIT3 == True)//第2个上升沿
		{
			if(TIM3_CNT>EXIT3_CNT)
			{
				EXIT3_CNT=TIM3_CNT-EXIT3_CNT;//更新在此期间的溢出值
				EXIT3_TIME=100*EXIT3_CNT;
				EXIT3_CNT=0;	
				EXIT3_SPEED+= Speed_Cal(EXIT3_TIME);
				num3++;
				if(num3==20){
					num3=0;
					EXIT3_SPEED=EXIT3_SPEED/20;
					printf("[ motor 3: %d  \r\n]  ", EXIT3_SPEED);
				}
			}			
			RisingFlag_EXIT3=False;			
		}
		else if( RisingFlag_EXIT3==0)//第一次检测到上升沿
		{
			TIM3->CNT=0;
			EXIT3_CNT=TIM3_CNT;//记录当前的溢出值
			RisingFlag_EXIT3 = True;
		}
	 EXTI_ClearITPendingBit(EXTI_Line3);//清除LINE4上的中断标志位  
}
