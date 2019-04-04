#include "pwm.h"
#include "usart.h"
#include "capture.h"
TIM_ICInitTypeDef  TIM4_ICInitStructure;


u8 TIM4CH1_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM4CH1_CAPTURE_UPVAL;
u16 TIM4CH1_CAPTURE_DOWNVAL;

u8 TIM4CH2_CAPTURE_STA = 0;	//通道2输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM4CH2_CAPTURE_UPVAL;
u16 TIM4CH2_CAPTURE_DOWNVAL;

u8 TIM4CH3_CAPTURE_STA = 0;	//通道3输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM4CH3_CAPTURE_UPVAL;
u16 TIM4CH3_CAPTURE_DOWNVAL;

u8 TIM4CH4_CAPTURE_STA = 0;	//通道4输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM4CH4_CAPTURE_UPVAL;
u16 TIM4CH4_CAPTURE_DOWNVAL; 

u32 tim4up1 = 0;	//捕获总高电平的时间
u32 tim4up2 = 0;	//捕获总高电平的时间
u32 tim4up3 = 0;	//捕获总高电平的时间
u32 tim4up4 = 0;	//捕获总高电平的时间

u32 tim4_T1;//作为溢出情况下的补差值
u32 tim4_T2;
u32 tim4_T3;
u32 tim4_T4;

//定时器4通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIM4_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM4时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	//使能PORTD时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; //GPIOD12, 13, 14, 15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4); //PD复用位定时器5
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
  
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	

	//初始化TIM4输入捕获ch1参数
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; 
  TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM4_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM4, &TIM4_ICInitStructure);	
	
		//初始化TIM3输入捕获参数 通道2
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	//初始化TIM3输入捕获参数 通道3
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	//初始化TIM3输入捕获参数 通道4
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM4_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);//允许CC1,2,3,4IE捕获中断//由于没有考虑到多次溢出情况，所以也不考虑更新中断
	TIM_ITConfig(TIM4,TIM_IT_CC2,ENABLE);
  TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4

 
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	
}
//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)

//定时器5中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    

 	if((TIM4CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		
		if(TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1); 		//清除中断标志位
			if (TIM4CH1_CAPTURE_STA & 0X40)		//捕获到下降沿
			{
				TIM4CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
				TIM4CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM4);//记录下此时的定时器计数值
				if (TIM4CH1_CAPTURE_DOWNVAL <= TIM4CH1_CAPTURE_UPVAL)
				{
					tim4_T1 = (0xFFFF-TIM4CH1_CAPTURE_UPVAL)+TIM4CH1_CAPTURE_DOWNVAL ;
				}
				else
					tim4_T1 = 0;
				tim4up1 = TIM4CH1_CAPTURE_DOWNVAL - TIM4CH1_CAPTURE_UPVAL + tim4_T1;		//得到上下沿时间差
				TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Rising); //设置为上升沿捕获
				TIM4CH1_CAPTURE_STA=0;
			}
			else //第一次捕获到数据
			{
				TIM4CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM4);		//获取计数值
				TIM4CH1_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC1PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//设置为下降沿捕获
			}

		}			     	    					   
 	}
	
	if((TIM4CH2_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		
		if(TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)//捕获2发生捕获事件
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC2); 		//清除中断标志位
			if (TIM4CH2_CAPTURE_STA & 0X40)		//捕获到下降沿
			{
				TIM4CH2_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
				TIM4CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM4);//记录下此时的定时器计数值
				if (TIM4CH2_CAPTURE_DOWNVAL <= TIM4CH2_CAPTURE_UPVAL)
				{
					tim4_T2 = (0xFFFF-TIM4CH2_CAPTURE_UPVAL)+TIM4CH2_CAPTURE_DOWNVAL ;
				}
				else
					tim4_T2 = 0;
				tim4up2 = TIM4CH2_CAPTURE_DOWNVAL - TIM4CH2_CAPTURE_UPVAL + tim4_T2;		//得到上下沿时间差
				TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); //设置为上升沿捕获
				TIM4CH2_CAPTURE_STA=0;
			}
			else //第一次捕获到数据
			{
				TIM4CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM4);		//获取计数值
				TIM4CH2_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//设置为下降沿捕获
			}

		}			     	    					   
 	}
	
	if((TIM4CH3_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		
		if(TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)//捕获3发生捕获事件
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC3); 		//清除中断标志位
			if (TIM4CH3_CAPTURE_STA & 0X40)		//捕获到下降沿
			{
				TIM4CH3_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
				TIM4CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM4);//记录下此时的定时器计数值
				if (TIM4CH3_CAPTURE_DOWNVAL <= TIM4CH3_CAPTURE_UPVAL)
				{
					tim4_T3 = (0xFFFF-TIM4CH3_CAPTURE_UPVAL)+TIM4CH3_CAPTURE_DOWNVAL ;
				}
				else
					tim4_T3 = 0;
				tim4up3 = TIM4CH3_CAPTURE_DOWNVAL - TIM4CH3_CAPTURE_UPVAL + tim4_T3;		//得到上下沿时间差
				TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //设置为上升沿捕获
				TIM4CH3_CAPTURE_STA=0;
			}
			else //第一次捕获到数据
			{
				TIM4CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM4);		//获取计数值
				TIM4CH3_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//设置为下降沿捕获
			}

		}			     	    					   
 	}
	if((TIM4CH4_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		
		if(TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//捕获4发生捕获事件
		{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC4); 		//清除中断标志位
			if (TIM4CH4_CAPTURE_STA & 0X40)		//捕获到下降沿
			{
				TIM4CH4_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
				TIM4CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM4);//记录下此时的定时器计数值
				if (TIM4CH4_CAPTURE_DOWNVAL <= TIM4CH4_CAPTURE_UPVAL)
				{
					tim4_T4 = (0xFFFF-TIM4CH4_CAPTURE_UPVAL)+TIM4CH4_CAPTURE_DOWNVAL ;
				}
				else
					tim4_T4 = 0;
				tim4up4 = TIM4CH4_CAPTURE_DOWNVAL - TIM4CH4_CAPTURE_UPVAL + tim4_T4;		//得到上下沿时间差
				TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //设置为上升沿捕获
				TIM4CH4_CAPTURE_STA=0;
			}
			else //第一次捕获到数据
			{
				TIM4CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM4);		//获取计数值
				TIM4CH4_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//设置为下降沿捕获
			}

		}			     	    					   
 	}
	/*
	//ch2
	if((TIM4CH2_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//溢出
		{	     
			if(TIM4CH2_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM4CH2_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM4CH2_CAPTURE_STA|=0X80;		//标记成功捕获了一次
					TIM4CH2_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM4CH2_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)//捕获2发生捕获事件
		{	
			if(TIM4CH2_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM4CH2_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
			  TIM4CH2_CAPTURE_VAL=TIM_GetCapture2(TIM4);//获取当前的捕获值.
	 			TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC2P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				TIM4CH2_CAPTURE_STA=0;			//清空
				TIM4CH2_CAPTURE_VAL=0;
				TIM4CH2_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				TIM_Cmd(TIM4,DISABLE ); 	//关闭定时器5
	 			//TIM_SetCounter(TIM4,0);
	 			TIM_OC2PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC2P=1 设置为下降沿捕获
				TIM_Cmd(TIM4,ENABLE ); 	//使能定时器5
			}		    
		}			     	    					   
 	}
		//ch3
	if((TIM4CH3_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//溢出
		{	     
			if(TIM4CH3_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM4CH3_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM4CH3_CAPTURE_STA|=0X80;		//标记成功捕获了一次
					TIM4CH3_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM4CH3_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)//捕获3发生捕获事件
		{	
			if(TIM4CH3_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM4CH3_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
			  TIM4CH3_CAPTURE_VAL=TIM_GetCapture3(TIM4);//获取当前的捕获值.
	 			TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC3P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				TIM4CH3_CAPTURE_STA=0;			//清空
				TIM4CH3_CAPTURE_VAL=0;
				TIM4CH3_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				TIM_Cmd(TIM4,DISABLE ); 	//关闭定时器5
	 			TIM_SetCounter(TIM4,0);
	 			TIM_OC3PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC3P=1 设置为下降沿捕获
				TIM_Cmd(TIM4,ENABLE ); 	//使能定时器5
			}		    
		}			     	    					   
 	}
	//ch4
	if((TIM4CH4_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//溢出
		{	     
			if(TIM4CH4_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM4CH4_CAPTURE_STA&0X4F)==0X3F)//高电平太长了
				{
					TIM4CH4_CAPTURE_STA|=0X80;		//标记成功捕获了一次
					TIM4CH4_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM4CH4_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)//捕获4发生捕获事件
		{	
			if(TIM4CH4_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM4CH4_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
			  TIM4CH4_CAPTURE_VAL=TIM_GetCapture4(TIM4);//获取当前的捕获值.
	 			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC4P=0 设置为上升沿捕获
			}
			else  								//还未开始,第一次捕获上升沿
			{
				TIM4CH4_CAPTURE_STA=0;			//清空
				TIM4CH4_CAPTURE_VAL=0;
				TIM4CH4_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				TIM_Cmd(TIM4,DISABLE ); 	//关闭定时器5
	 			TIM_SetCounter(TIM4,0);
	 			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);		//CC3P=1 设置为下降沿捕获
				TIM_Cmd(TIM4,ENABLE ); 	//使能定时器5
			}		    
		}			     	    					   
 	}
	*/
}



/*
u32 period = 0;
u32 duty  = 0;
u8 CollectFlag = 1;

void TIM4_IRQHandler(void)
{

	if(CollectFlag){
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
		{	
			duty = TIM_GetCapture1(TIM4); 				//采集占空比		
		  period	=	TIM_GetCapture2(TIM4);     //采集周期
		}
		
		CollectFlag = 0;
	}	
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
}*/


