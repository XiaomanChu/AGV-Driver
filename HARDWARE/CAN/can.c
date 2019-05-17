#include "can.h"
#include "motor_driver.h"
#include  "controler_logic.h"
#include "delay.h"
#include "usart.h"
#include "steering_driver.h"
int i=0;
 FunctionalState Controler_ENABLE =DISABLE;//默认遥控器不可用
 FunctionalState CAN_COMMAND = ENABLE;//默认CAN接收
u8 Can_Motor_Buf[8];
u8 Can_AGV_Buf[8];
u8 Can_Steer_Buf[8];
Motor_StatusDef CAN_Motor_Setup;

//CAN总线初始化
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    //使能相关时钟
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟	                   											 
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);//使能PORTI时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB9
		GPIO_Init(GPIOI, &GPIO_InitStructure);//初始化PI9
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); //GPIOB9复用为CAN1
	  GPIO_PinAFConfig(GPIOI,GPIO_PinSource9,GPIO_AF_CAN1); //GPIOI9复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; //屏蔽模式
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000; 
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
#if CAN1_RX0_INT_ENABLE
	
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN1_RX0_INT_ENABLE	//使能RX0中断
//CAN接收中断服务函数			    
void CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0) != RESET)
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
		CAN_Receive(CAN1, 0, &RxMessage);

		if(RxMessage.IDE == CAN_ID_STD && RxMessage.RTR==CAN_RTR_DATA && RxMessage.DLC==8)
		{
			switch(RxMessage.StdId)
			{
				case 0x100:
					Can_AGV_Control(RxMessage.Data)	;break;//小车控制						
				case 0x102:
					if(CAN_COMMAND)Can_Motor_Control(RxMessage.Data);break;//电机控制
				case 0x104:
					if(CAN_COMMAND)Can_Steering_Control(RxMessage.Data);break;	//转向控制
			}
		}	
	}
}
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=0x12;	 // 标准标识符为0
  //TxMessage.ExtId=0x12;	 // 设置扩展标示符（29位）
  TxMessage.IDE= CAN_ID_STD;		  // 使用标准帧
  TxMessage.RTR=CAN_RTR_Data;		  // 消息类型为数据帧，一帧8位
  TxMessage.DLC=len;							 // 发送len帧，最高8帧
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // 第i帧信息          
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return 1;
  return 0;		

}



//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}


//小车功能配置
void Can_AGV_Control(u8 *CanReceive)
{
	for(i=0;i<7;i++)Can_AGV_Buf[i]=CanReceive[7-i];//读取缓冲
	
	if(Can_AGV_Buf[0]==0x01){  //ID0x100的第0位控制小车操控方式
		Controler_ENABLE=ENABLE;//遥控器控制
		CAN_COMMAND = DISABLE;
	}
	else{
		 CAN_COMMAND = ENABLE;//CAN命令控制
		Controler_ENABLE=DISABLE;
	}
	
}


//小车电机CAN命令控制
void Can_Motor_Control(u8 *CanReceive)
{
	for(i=0;i<7;i++)Can_Motor_Buf[i]=CanReceive[7-i];//读取缓冲
	
	if(Can_Motor_Buf[0]== ENABLE)CAN_Motor_Setup.enable=MotorEnable;
	else CAN_Motor_Setup.enable=MotorDisable;
	if(Can_Motor_Buf[1]== ENABLE)CAN_Motor_Setup.dir=FORWARD;
	else CAN_Motor_Setup.dir = BACKWARD;
	if(Can_Motor_Buf[2]== ENABLE)CAN_Motor_Setup.Break=BREAK;
	else CAN_Motor_Setup.Break = NoBREAK;
	Motor_Status_Setup(&CAN_Motor_Setup);//电机状态控制
	if(Can_Motor_Buf[3]<=100)
	{
		SetMotorPWM(dir_flag,Can_Motor_Buf[3]);
	}
	else printf("开启使能 \r\n");
	
}

//CAN转向控制  //每转1°大约1600个脉冲，转45°就是pluse_num=1600*45（待调整）
void Can_Steering_Control(u8 *CanReceive)
{
	for(i=0;i<7;i++)Can_Steer_Buf[i]=CanReceive[7-i];//读取缓冲
	long pulse_num=0;//脉冲数量
	dir_flag=Can_Steer_Buf[3];
	if(Can_Steer_Buf[4]<20)pulse_num=1600*Can_Steer_Buf[4];
	if(dir_flag==1)Locate_Abs(pulse_num,7000);
	else if(dir_flag==2)Locate_Abs(-pulse_num,7000);
	else if(dir_flag==0)Locate_Abs(0,7000);
	
}












