#include "SpdCap.h"

u8 TIM3CH1_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH1_CAPTURE_UPVAL;
u16 TIM3CH1_CAPTURE_DOWNVAL;

u8 TIM3CH2_CAPTURE_STA = 0;	//通道2输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH2_CAPTURE_UPVAL;
u16 TIM3CH2_CAPTURE_DOWNVAL;

u8 TIM3CH3_CAPTURE_STA = 0;	//通道3输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH3_CAPTURE_UPVAL;
u16 TIM3CH3_CAPTURE_DOWNVAL;

u8 TIM3CH4_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH4_CAPTURE_UPVAL;
u16 TIM3CH4_CAPTURE_DOWNVAL;

u32 tempup1 = 0;	//捕获总高电平的时间
u32 tempup2 = 0;	//捕获总高电平的时间
u32 tempup3 = 0;	//捕获总高电平的时间
u32 tempup4 = 0;	//捕获总高电平的时间
u32 f1=0;//捕获频率
u32 f2=0;//捕获频率
u32 f3=0;//捕获频率
u32 f4=0;//捕获频率



u32 tim3_T1;
u32 tim3_T2;
u32 tim3_T3;
u32 tim3_T4;

int pwmout1, pwmout2, pwmout3, pwmout4; 				//输出占空比

//定时器4通道1输入捕获配置

TIM_ICInitTypeDef TIM3_ICInitStructure;

void TIM3_Cap_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //使能GPIOC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //使能GPIOB时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //PB6,7,8,9 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  //PB6,7,8,9 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3); //PC6复用位定时器3
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3); //PC7复用位定时器3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //PB0复用位定时器3
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3); //PB1复用位定时器3


	//初始化定时器4 TIM3	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM3输入捕获参数 通道1
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//初始化TIM3输入捕获参数 通道2
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//初始化TIM3输入捕获参数 通道3
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//初始化TIM3输入捕获参数 通道4
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级1级
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);   //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,ENABLE);   //不允许更新中断，允许CC1IE,CC2IE,CC3IE,CC4IE捕获中断	

	TIM_Cmd(TIM3, ENABLE); 		//使能定时器3

}

//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
	if ((TIM3CH1_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) 		//捕获1发生捕获事件
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); 		//清除中断标志位
			if (TIM3CH1_CAPTURE_STA & 0X40)		//捕获到另一个上升沿
			{
				TIM3CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM3);//记录下此时的定时器计数值
				if (TIM3CH1_CAPTURE_DOWNVAL <= TIM3CH1_CAPTURE_UPVAL)
				{
					tim3_T1 = (65535-TIM3CH1_CAPTURE_UPVAL)+TIM3CH1_CAPTURE_DOWNVAL ;
				}
				else
					tim3_T1 = 0;
				tempup1 = TIM3CH1_CAPTURE_DOWNVAL - TIM3CH1_CAPTURE_UPVAL + tim3_T1;		//得到两个上升沿的时间差
				TIM3CH1_CAPTURE_STA = 0;		//捕获标志位清零
				f1=1000000/tempup1;
			}
			else //第一次捕获到数据
			{
				TIM3CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM3);		//获取计数值
				TIM3CH1_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
			}
		}
	}

	if ((TIM3CH2_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET) 		//捕获1发生捕获事件
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC2); 		//清除中断标志位
			if (TIM3CH2_CAPTURE_STA & 0X40)		//捕获到另一个上升沿
			{
				TIM3CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM3);//记录下此时的定时器计数值
				if (TIM3CH2_CAPTURE_DOWNVAL < TIM3CH2_CAPTURE_UPVAL)
				{
					tim3_T2 = (65535-TIM3CH2_CAPTURE_UPVAL)+TIM3CH2_CAPTURE_DOWNVAL ;
					
				}
				else
					tim3_T2 = 0;
				tempup2 = TIM3CH2_CAPTURE_DOWNVAL - TIM3CH2_CAPTURE_UPVAL + tim3_T2;		//得到两个上升沿的时间差
				TIM3CH2_CAPTURE_STA = 0;		//捕获标志位清零
			  f2=1000000/tempup2;

			}
			else //第一次捕获到数据
			{
				TIM3CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM3);		//获取计数值
				TIM3CH2_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
			}
		}
	}
	
	if ((TIM3CH3_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET) 		//捕获3发生捕获事件
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC3); 		//清除中断标志位
			if (TIM3CH3_CAPTURE_STA & 0X40)		//捕获到另一个上升沿
			{
				TIM3CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM3);//记录下此时的定时器计数值
				if (TIM3CH3_CAPTURE_DOWNVAL < TIM3CH3_CAPTURE_UPVAL)
				{
					//tim3_T3 = (65535-TIM3CH3_CAPTURE_UPVAL)+TIM3CH3_CAPTURE_DOWNVAL ;
					tim3_T3 = 65535;
				}
				else
					tim3_T3 = 0;
				tempup3 = TIM3CH3_CAPTURE_DOWNVAL - TIM3CH3_CAPTURE_UPVAL + tim3_T3;		//得到两个上升沿的时间差
				TIM3CH3_CAPTURE_STA = 0;		//捕获标志位清零
				f3=1000000/tempup3;
			}
			else //第一次捕获到数据
			{
				TIM3CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM3);		//获取计数值
				TIM3CH3_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
			}
		}
	}

		if ((TIM3CH4_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET) 		//捕获4发生捕获事件
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC4); 		//清除中断标志位
			if (TIM3CH4_CAPTURE_STA & 0X40)		//捕获到另一个上升沿
			{
				TIM3CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM3);//记录下此时的定时器计数值
				if (TIM3CH4_CAPTURE_DOWNVAL < TIM3CH4_CAPTURE_UPVAL)
				{
					tim3_T4 = (65535-TIM3CH4_CAPTURE_UPVAL)+TIM3CH4_CAPTURE_DOWNVAL ;
				}
				else
					tim3_T4 = 0;
				tempup4 = TIM3CH4_CAPTURE_DOWNVAL - TIM3CH4_CAPTURE_UPVAL + tim3_T4;		//得到两个上升沿的时间差
				TIM3CH4_CAPTURE_STA = 0;		//捕获标志位清零
				f4=1000000/tempup4;
			}
			else //第一次捕获到数据
			{
				TIM3CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM3);		//获取计数值
				TIM3CH4_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
			}
		}
	}
}
