#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "motor_driver.h"
#include "controler_capture.h"
#include "controler_logic.h"
#include "steering_driver.h"
#include	"SpdCap.h"
#include "key.h"
#include "can.h"


//u8 RxProcess(void);//´®¿ÚÊı¾İ´¦Àí
//void TxProcess(u8 sendData);//´®¿ÚÊı¾İ·¢ËÍ


int main(void)
{  
	u8 keyval;//°´¼ü¼ì²â
	//u8 CanRxBuf[8];//can×ÜÏß½ÓÊÜÊı¾İ
	u8 CanTxBuf[7];//can×ÜÏß·¢ËÍÊı¾İ
	u8 CanT_Flag;
	u8 CanR_Flag;
	
	//¸÷ÖÖ³õÊ¼»¯³ÌĞò
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÏµÍ³ÖĞ¶ÏÓÅÏÈ¼¶·Ö×é2
	delay_init(168);  //³õÊ¼»¯ÑÓÊ±º¯Êı
	uart_init(115200);//³õÊ¼»¯´®¿Ú²¨ÌØÂÊÎª115200
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//CAN³õÊ¼»¯»·»ØÄ£Ê½£¨CAN_Mode_LoopBack£©,£¨ÆÕÍ¨Ä£Ê½£ºCAN_Mode_Normal£©²¨ÌØÂÊ500Kbps   
	Motor_Init();//ĞĞ×ßµç»úÇı¶¯³õÊ¼»¯
 	PWMInit(500-1,84-1);	//84M/84=1MhzµÄ¼ÆÊıÆµÂÊ,ÖØ×°ÔØÖµ500£¬ËùÒÔPWMÆµÂÊÎª 1M/500=2Khz.  
  SetMotorPWM(0,0);//ËÙ¶È³õÊ¼0
	KEY_Init();					//°´¼ü³õÊ¼»¯
	TIM4_Cap_Init(0XFFFF,84-1); //ÒÔ1MhzµÄÆµÂÊ¼ÆÊı £¬Òç³öÊ±¼ä65536us,Ò£¿ØÆ÷½ÓÊÕÆ÷×î³¤Âö¿í2160us
  TIM3_Cap_Init(0x63,84-1); //ÒÔ1MhzµÄÆµÂÊ¼Æ£¬Òç³öÊ±¼ä100us
	Driver_Init();			//²½½øµç»úÇı¶¯Æ÷³õÊ¼»¯
	TIM8_OPM_RCR_Init(999,168-1); //TIM8Îª168MHzËùÒÔÊÇ1MHz¼ÆÊıÆµÂÊ  µ¥Âö³å+ÖØ¸´¼ÆÊıÄ£Ê½  

	//Ö÷Ñ­»·
   while(1) 
	{
 		//delay_ms(10);
		



		//Ò£¿Ø³ÌĞò
		if(Controler_ENABLE)RemoteControler();
		//printf("motor 1: hz \r\n");
		//ËÙ¶È·´À¡³ÌĞò
		//delay = SpeedFeedback(delay);

		
		//×ªÏòÎ¢µ÷	
		keyval=KEY_Scan(0);
		if(keyval==KEY3_PRES)
		{
					//printf("key4 pressed\r\n");
			for(int i=0;i<8;i++)
			{
				CanTxBuf[i]=i;
			}
			CanT_Flag=CAN1_Send_Msg(CanTxBuf,8);
			if(!CanT_Flag)printf("Transction Success\r\n" );
			else printf("Transction Failed\r\n");
			//Locate_Abs(0,7000);//°´ÏÂKEY3£¬»ØÁãµã
		}else if(keyval==KEY1_PRES)
		{
			Locate_Rle(1000,7500,CW);//°´ÏÂKEY1£¬ÒÔ7000HzµÄÆµÂÊ Ë³Ê±Õë·¢1000Âö³å, 
		}else if(keyval==KEY2_PRES)
		{
			Locate_Rle(1000,7500,CCW);//°´ÏÂKEY2£¬ÒÔ7000HzµÄÆµÂÊ ÄæÊ±Õë·¢1000Âö³å
		}			

	

	#if 0    //PC¶Ë´®¿Úµ÷ÊÔ³ÌĞò
		temp= RxProcess();
		if(USART_RX_BUF[0]!=0)printf("µç»úËÙÂÊ:%d\r\n",100-temp);
		//Çå³ı´®¿ÚÊı¾İ,×¼±¸ÏÂÒ»´Î½ÓÊÕ
		for(i=0;i<USART_RX_STA;i++)
		{
			USART_RX_BUF[i]=0;
		}
		USART_RX_STA=0;
#endif
	}	
}

//u8 RxProcess(void)//´®¿ÚÊı¾İ´¦Àí
//{
//	u8 temp = 0;
//	float k;

//	switch(USART_RX_BUF[0])//Ñ¡ÔñÄ£Ê½
//	{
//		case 0x10://10Ôİ¶¨µç»úµ÷ËÙÄ£Ê½
//		{
//			//GPIO_ResetBits(GPIOH,GPIO_Pin_9);
//			GPIO_SetBits(GPIOH,GPIO_Pin_9);//Õı×ª
//			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
//			temp = USART_RX_BUF[1];
//			k=(float)(500-0)/(float)(100-0);
//			if(temp<=100)
//			{
//				SetMotorPWM(0,(u32)k*temp);
//				GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹Ä
//			}
//			else	printf("Êı¾İ´íÎó\r\n");
//		}break;
//		case 0x11:
//		{
//			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//·´×ª
//			GPIO_SetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
//			//GPIO_SetBits(GPIOH,GPIO_Pin_9);
//			temp = USART_RX_BUF[1];
//			k=(float)(500-0)/(float)(100-0);
//			if(temp<=100)
//			{
//					SetMotorPWM(0,(u32)k*temp);	
//					GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹Ä
//			}
//		}break;
//		case 0X12:
//			GPIO_ResetBits(GPIOH,GPIO_Pin_15);break;//¹Ø±ÕÊ¹ÄÜ
//			
//		case 0X13:
//			GPIO_ResetBits(GPIOH,GPIO_Pin_13);break;//É²³µ
//	}
//	return temp;//·µ»ØÕ¼¿Õ±È
//}
//void TxProcess(u8 sendData)//´®¿ÚÊı¾İ·¢ËÍ
//{
//	switch(USART_RX_BUF[0])
//	{	
//		case 0x10:
//			USART_SendData(USART1,sendData);
//	}
//}
