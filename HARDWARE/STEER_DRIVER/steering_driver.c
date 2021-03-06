#include "steering_driver.h"
#include "delay.h"
#include "usart.h"


/********** 驱动器 端口定义 **************
//DRIVER_DIR1   PD7 默认高，正转
//DRIVER_DIR2   PF11 默认高，正转
//DRIVER_OE    PD3 默认低，使能
//STEP_PULSE   PI5 
******************************************/

u8 rcr_remainder;   //重复计数余数部分
u8 is_rcr_finish=1; //重复计数器是否设置完成

long rcr_integer;	//重复计数整数部分
long target_pos=0;  //有符号方向
long current_pos=0; //有符号方向
long rcrval=0;
DIR_Type motor_dir=CW;//顺时针

/************** 驱动器控制信号线初始化 ****************/
void Driver_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_7; //DRIVER_DIR DRIVER_OE对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD3,7
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //DRIVER_DIR DRIVER_OE对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF11
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_7);//PD7,PF11输出高 顺时针方向  DIR+
	GPIO_SetBits(GPIOF,GPIO_Pin_11);
	GPIO_ResetBits(GPIOD,GPIO_Pin_3|GPIO_Pin_11);//输出低 使能输出(开启锁定)  ENA+ 
}
/***********************************************
//TIM8_CH1(PI5) 单脉冲输出+重复计数功能初始化
//TIM8 时钟频率 84*2=168MHz
//arr:自动重装值
//psc:时钟预分频数
************************************************/
void TIM8_OPM_RCR_Init(u16 arr,u16 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	//TIM8时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); 	//使能PORTI时钟	                                                                     	
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//使能PORTI时钟	
	
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource5,GPIO_AF_TIM8); //GPIOI5复用为定时器8
	//GPIO_PinAFConfig(GPIOI,GPIO_PinSource6,GPIO_AF_TIM8);//GPIOI6复用为定时器8
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6; //GPIOI5,GPIOI6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;      //下拉 
	GPIO_Init(GPIOI,&GPIO_InitStructure);  	//初始化PI
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值   
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_ClearITPendingBit(TIM8,TIM_IT_Update);

	TIM_UpdateRequestConfig(TIM8,TIM_UpdateSource_Regular); /********* 设置只有计数溢出作为更新中断 ********/
	TIM_SelectOnePulseMode(TIM8,TIM_OPMode_Single);/******* 单脉冲模式 **********/
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出2使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; /****** 比较输出2N失能 *******/
	TIM_OCInitStructure.TIM_Pulse = arr>>1; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIM8_ch1


  TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能	 	 
	TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIM8在ARR上的预装载寄存器
	
	TIM_ITConfig(TIM8, TIM_IT_Update ,ENABLE);  //TIM8   使能或者失能指定的TIM中断
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;  //TIM8中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
	TIM_Cmd(TIM8, ENABLE);  //使能TIM8											  
}
/******* TIM8更新中断服务程序 *********/
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8,TIM_FLAG_Update)!=RESET)//更新中断
	{
		TIM_ClearITPendingBit(TIM8,TIM_FLAG_Update);//清除更新中断标志位		
		if(is_rcr_finish==0)//重复计数器未设置完成
		{
			if(rcr_integer!=0) //整数部分脉冲还未发送完成
			{
				TIM8->RCR=RCR_VAL;//设置重复计数值
				rcr_integer--;//减少RCR_VAL+1个脉冲				
			}
			else if(rcr_remainder!=0)//余数部分脉冲 不位0
			{
				TIM8->RCR=rcr_remainder-1;//设置余数部分
				rcr_remainder=0;//清零
				is_rcr_finish=1;//重复计数器设置完成				
			}else goto out;   //rcr_remainder=0，直接退出			 
			TIM_GenerateEvent(TIM8,TIM_EventSource_Update);//产生一个更新事件 重新初始化计数器
			TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE 主输出使能	
			TIM_Cmd(TIM8, ENABLE);  //使能TIM8			
			if(motor_dir==CW) //如果方向为顺时针   
				current_pos+=(TIM8->RCR+1);//加上重复计数值
			else          //否则方向为逆时针
				current_pos-=(TIM8->RCR+1);//减去重复计数值			
		}
		else
		{
out:	is_rcr_finish=1;//重复计数器设置完成
			TIM_CtrlPWMOutputs(TIM8,DISABLE);	//MOE 主输出关闭
			TIM_Cmd(TIM8, DISABLE);  //关闭TIM8				
			printf("当前位置=%ld\r\n",current_pos);//打印输出
		}	
	}
}
/***************** 启动TIM8 *****************/
void TIM8_Startup(u32 frequency)   //启动定时器8
{
	u16 temp_arr=1000000/frequency-1; 
	TIM_SetAutoreload(TIM8,temp_arr);//设定自动重装值	
	TIM_SetCompare1(TIM8,temp_arr>>1); //匹配值2等于重装值一半，是以占空比为50%	
	TIM_SetCounter(TIM8,0);//计数器清零
	TIM_Cmd(TIM8, ENABLE);  //使能TIM8
}
/********************************************
//相对定位函数 
//num 0～2147483647
//frequency: 20Hz~100KHz
//dir: CW(顺时针方向)  CCW(逆时针方向)
*********************************************/
void Locate_Rle(long num,u32 frequency,DIR_Type dir) //相对定位函数
{
	if(num<=0) //数值小等于0 则直接返回
	{
		printf("\r\nThe num should be greater than zero!!\r\n");
		return;
	}
	if(TIM8->CR1&0x01)//上一次脉冲还未发送完成  直接返回
	{
		printf("\r\nThe last time pulses is not send finished,wait please!\r\n");
		return;
	}
	if((frequency<20)||(frequency>100000))//脉冲频率不在范围内 直接返回
	{
		printf("\r\nThe frequency is out of range! please reset it!!(range:20Hz~100KHz)\r\n");
		return;
	}
	motor_dir=dir;//得到方向	
	DRIVER_DIR2=motor_dir;//设置方向
	DRIVER_DIR1=~DRIVER_DIR2;
	
	if(motor_dir==CW)//顺时针
		target_pos=current_pos+num;//目标位置
	else if(motor_dir==CCW)//逆时针
		target_pos=current_pos-num;//目标位置
	
	rcr_integer=num/(RCR_VAL+1);//重复计数整数部分
	rcr_remainder=num%(RCR_VAL+1);//重复计数余数部分
	is_rcr_finish=0;//重复计数器未设置完成
	TIM8_Startup(frequency);//开启TIM8
	
}
/********************************************
//绝对定位函数 
//num   -2147483648～2147483647
//frequency: 20Hz~100KHz
*********************************************/
void Locate_Abs(long num,u32 frequency)//绝对定位函数
{
	if(TIM8->CR1&0x01)//上一次脉冲还未发送完成 终止定时器 计算更新当前位置
	{
		
		TIM_Cmd(TIM8, DISABLE);//定时器停止
		delay_ms(60);//如果转向步进电机存在颤抖，调大延时，但过高的延时会导致电机转向修正时有顿挫感
		rcrval=TIM8->RCR;
		if(motor_dir==CCW)current_pos=target_pos+(rcr_integer*(RCR_VAL+1)+rcr_remainder+rcrval);
		else current_pos=target_pos-(rcr_integer*(RCR_VAL+1)+rcr_remainder+rcrval);
		//printf("\r\ncurrent_pos updated!\r\n");
	}
	if((frequency<20)||(frequency>100000))//脉冲频率不在范围内 直接返回
	{
		//printf("\r\nThe frequency is out of range! please reset it!!(range:20Hz~100KHz)\r\n");
		return;
	}
	target_pos=num;//设置目标位置
	if(target_pos!=current_pos)//目标和当前位置不同
	{
		if(target_pos>current_pos)
			motor_dir=CW;//顺时针
		else
			motor_dir=CCW;//逆时针
		DRIVER_DIR2=motor_dir;//设置方向
		DRIVER_DIR1=~DRIVER_DIR2;
		
		rcr_integer=abs(target_pos-current_pos)/(RCR_VAL+1);//重复计数整数部分
		rcr_remainder=abs(target_pos-current_pos)%(RCR_VAL+1);//重复计数余数部分
		is_rcr_finish=0;//重复计数器未设置完成
		TIM8_Startup(frequency);//开启TIM8
	}
}





