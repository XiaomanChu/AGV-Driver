/************************************************************************
*此部分程序为AGV行走直流电机驱动程序，配合相关驱动器
*采用TIM5 PWM输出
*程序对外输出变量：
*				extern double Diff_Calculation(double alpha);//左右差速计算
*				extern Motor_StatusDef Motor_StatusInit;//电机状态结构体
*************************************************************************/



#include "motor_driver.h" 
#include "steering_driver.h"
#include "math.h"
#include "usart.h"
#define PI 3.1416

//PH9,PH14为电机转向控制,低电平为正转
//PH15 为电机使能信号,低电平使能
//PH13 为刹车信号 低电平刹车

Motor_StatusDef Motor_StatusInit;//电机状态设定结构体

void Motor_Init(void)
{   
	//IO初始化
  GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);//使能GPIO时钟
  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	//电机状态初始化
	Motor_StatusInit.enable= MotorDisable;
	Motor_StatusInit.dir = FORWARD;
	Motor_StatusInit.Break = NoBREAK;
	Motor_StatusInit.Acceleration=0;
	
	Motor_Status_Setup(&Motor_StatusInit);
	
//	GPIO_ResetBits(GPIOH,GPIO_Pin_15);//使能信号EN默认低电平/使能
//  GPIO_SetBits(GPIOH,GPIO_Pin_9);				//方向正转
//	GPIO_ResetBits(GPIOH,GPIO_Pin_14);			//方向正转
//	GPIO_SetBits(GPIOH,GPIO_Pin_13);		//刹车信号，不刹车


	
}

//TIM5 PWM部分初始化 
//PH10~12,PA3对应TIM5CH1~4通道输出
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void PWMInit(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); 	//使能PORTH时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource10,GPIO_AF_TIM5); //GPIO复用为定时器5
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource11,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource12,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM5);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOH,&GPIO_InitStructure);              //初始化PE5

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉           //初始化PE5
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);//初始化定时器9
	
	//初始化TIM9 Channel PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5OC1~OC4
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	TIM_OC3Init(TIM5, &TIM_OCInitStructure);
	TIM_OC4Init(TIM5, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR1~4上的预装载寄存器
  TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
  TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM5, ENABLE);  //使能TIM5
 
										  
}  

//电机占空比设定
//falg：方向，percent：速度百分比0~100
void SetMotorPWM(u8 flag,u32 percent)
{
			
			double	alpha;//当前转向角
	if(percent<=100)
	{
			u32 duty= 500-5*percent;//改成（500~0）区间
			
			if(flag== Right){//右转
				alpha = abs(current_pos)*0.00063;//每个脉冲转角约为0.00063度
				duty=Diff_Calculation(alpha)*percent; //正常占空系数*内外圈速度比例
				TIM_SetCompare3(TIM5,duty);//左边加速
				TIM_SetCompare4(TIM5,duty);
			}
			else if(flag== Left){//左转
				alpha = abs(current_pos)*0.00063;//每个脉冲转角约为0.00063度
				duty=Diff_Calculation(alpha)*percent; //正常占空系数*内外圈速度比例
				TIM_SetCompare1(TIM5,duty);//右边加速
				TIM_SetCompare2(TIM5,duty);
				//printf("diff: %d \r\n",duty);
			}
			else//正常前进，无需差速
			{
				TIM_SetCompare1(TIM5,duty);
				TIM_SetCompare2(TIM5,duty);
				TIM_SetCompare3(TIM5,duty);
				TIM_SetCompare4(TIM5,duty);
			}
		}
}

//电机状态控制
void Motor_Status_Setup(Motor_StatusDef* StatusSetup)
{
	if(StatusSetup->enable==MotorDisable)GPIO_ResetBits(GPIOH,GPIO_Pin_15);//使能信号EN默认低电平/不使能
	else if (StatusSetup->enable==MotorEnable)GPIO_SetBits(GPIOH,GPIO_Pin_15);//使能
	
	if(StatusSetup->Break==NoBREAK)GPIO_SetBits(GPIOH,GPIO_Pin_13);	//不刹车
	else GPIO_ResetBits(GPIOH,GPIO_Pin_13);//刹车
	
	if(StatusSetup->dir==FORWARD){
		GPIO_SetBits(GPIOH,GPIO_Pin_9);				//方向正转
		GPIO_ResetBits(GPIOH,GPIO_Pin_14);					//方向正转
	}
	else{
		GPIO_ResetBits(GPIOH,GPIO_Pin_9);				//方向反转
		GPIO_SetBits(GPIOH,GPIO_Pin_14);					//方向反转
	}
	
	//加速度设定待写

	
}


//差速比例计算 
double Diff_Calculation(double alpha)
{
	double la,lb; 
	double diff;
	double si =sin(alpha*3.1416/180);
	la = 7.0/18.0 * 3.1416* alpha / si;//内圈,路程小
	//diff = 20.0;//固定差速
	diff = 0.0018*pow(alpha,3) - 0.0395*pow(alpha,2)+0.3532*alpha-0.0238;
	alpha+=diff;
	si =sin(alpha*3.1416/180);
	lb = 7.0/18.0 * PI*alpha / si;//外圈，路程大
	return lb/la;
	
	
	
}




