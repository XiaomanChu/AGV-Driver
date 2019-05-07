#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "motor_driver.h"
#include "controler_capture.h"
#include "controler_logic.h"
#include "steering_driver.h"
//#include	"SpdCap.h"
#include "key.h"

//#include "usmart.h"

u8 RxProcess(void);//´®¿ÚÊı¾İ´¦Àí
void TxProcess(u8 sendData);//´®¿ÚÊı¾İ·¢ËÍ
//void SetMotorPWM(u32 arry);
//void RemoteControler(void);

int main(void)
{  
	//u8 temp=0;int i=0;

	u8 keyval;//°´¼ü¼ì²â

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÏµÍ³ÖĞ¶ÏÓÅÏÈ¼¶·Ö×é2
	delay_init(168);  //³õÊ¼»¯ÑÓÊ±º¯Êı
	uart_init(115200);//³õÊ¼»¯´®¿Ú²¨ÌØÂÊÎª115200
	Motor_Init();
 	PWMInit(500-1,84-1);	//84M/84=1MhzµÄ¼ÆÊıÆµÂÊ,ÖØ×°ÔØÖµ500£¬ËùÒÔPWMÆµÂÊÎª 1M/500=2Khz.  
  SetMotorPWM(0,500);//500¶ÔÓ¦0Õ¼¿Õ±È
	KEY_Init();					//°´¼ü³õÊ¼»¯
	TIM4_Cap_Init(0XFFFF,84-1); //ÒÔ1MhzµÄÆµÂÊ¼ÆÊı £¬Òç³öÊ±¼ä65535us,Ò£¿ØÆ÷½ÓÊÕÆ÷×î³¤Âö¿í2160us
//  TIM3_Cap_Init(0xffff,84-1); //ÒÔ1MhzµÄÆµÂÊ¼Æ
	
	Driver_Init();			//²½½øµç»úÇı¶¯Æ÷³õÊ¼»¯
	TIM8_OPM_RCR_Init(999,168-1); //TIM8Îª168MHzËùÒÔÊÇ1MHz¼ÆÊıÆµÂÊ  µ¥Âö³å+ÖØ¸´¼ÆÊıÄ£Ê½  

   while(1) 
	{
 		//delay_ms(10);
		
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
		
		//Ò£¿Ø³ÌĞò
		RemoteControler();
	
		//ËÙ¶È·´À¡³ÌĞò
		//delay = SpeedFeedback(delay);

		
		//×ªÏòÎ¢µ÷	
		keyval=KEY_Scan(0);
		if(keyval==KEY3_PRES)
		{
			Locate_Abs(0,7000);//°´ÏÂKEY3£¬»ØÁãµã
		}else if(keyval==KEY1_PRES)
		{
			Locate_Rle(1000,7000,CW);//°´ÏÂKEY1£¬ÒÔ7000HzµÄÆµÂÊ Ë³Ê±Õë·¢1000Âö³å, 
		}else if(keyval==KEY2_PRES)
		{
			Locate_Rle(1000,7000,CCW);//°´ÏÂKEY2£¬ÒÔ7000HzµÄÆµÂÊ ÄæÊ±Õë·¢1000Âö³å
		}			
		
	

	
	}	
}

//void SetMotorPWM(u32 arry)
//{
//			double	alpha;//µ±Ç°×ªÏò½Ç
//			u32 duty= arry;
//			
//			if(flag==1){//ÓÒ×ª
//				alpha = abs(current_pos)*0.00063;//Ã¿¸öÂö³å×ª½ÇÔ¼Îª0.00063¶È
//				duty=1.0/Diff_Calculation(alpha)*arry; //Õı³£Õ¼¿ÕÏµÊı*ÄÚÍâÈ¦ËÙ¶È±ÈÀı
//				TIM_SetCompare1(TIM5,duty);
//				TIM_SetCompare2(TIM5,duty);
//			}
//			else if(flag==2){//×ó×ª
//				alpha = abs(current_pos)*0.00063;//Ã¿¸öÂö³å×ª½ÇÔ¼Îª0.00063¶È
//				duty=1.0/Diff_Calculation(alpha)*arry; //Õı³£Õ¼¿ÕÏµÊı*ÄÚÍâÈ¦ËÙ¶È±ÈÀı
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

//void RemoteControler() //Ò£¿Ø³ÌĞò
//{
//		
//		k1=(float)(500-0)/(float)(216-87);//²¶»ñµÄ²âÁ¿ÖµÊÇ87~215£¬±ê¶¨µ½0~500
//		k_right=(float)35000/(float)(21-15);//ÓÒ×ªÏµÊı
//		k_left=(float)35000/(float)(15-8);//×ó×ªÏµÊı
//		count1=tim4up1/10;		   //È¥µô×îºóÒ»Î»²»ÎÈ¶¨µÄÊı£¨´¦ÀíÖ®ºó·¶Î§´ó¸ÅÔÚ87~215£©
//		count2=tim4up2/10;
//		count3=tim4up3/10;		   //È¥µô×îºóÒ»Î»²»ÎÈ¶¨µÄÊı£¨´¦ÀíÖ®ºó·¶Î§´ó¸ÅÔÚ87~215£©
//		count4=tim4up4/10;
//		
//		
//		//printf("HIGH:%lld us\r\n",count1); 
//		//printf("DIR: %d \r\n",count2);
//		//printf("DIR: %lld \n",count3);
//	
//	
//		//½«²¶»ñ¼ÆÊı×ª»»³ÉPWMµç»ú¿ØÖÆ
//		//count1=count1/10;//
//		if(count1<300)
//		{
//			//printf("HIGH:%d us\r\n",tim4up1); 
//			count1=500-(int)(k1*(count1-87));//±ê¶¨µ½£¨500£¬0£©Çø¼ä
//			//printf("duty:%d \r\n",count1); 
//			SetMotorPWM(flag,count1);
//		}
//		if(count2<109)
//		{
//			GPIO_SetBits(GPIOH,GPIO_Pin_9);//Õı×ª
//			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
//			GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹
//	
//		}
//		else if(count2>190&&count2<300)
//		{
//			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//·´×ª
//			GPIO_SetBits(GPIOH,GPIO_Pin_14);
//			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
//			GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹
//			
//		}
//		else if(count2>140&&count2<160) 
//		{
//			 GPIO_ResetBits(GPIOH,GPIO_Pin_15);//¹Ø±ÕÊ¹ÄÜ
//			
//		}
//		
//		pst=count3/10;  //½«Ò£¿ØÆ÷ÊäÈë±ê¶¨µ½(8,21)
//		if(pst<300&&pst!=pst_old)//Ò£¿Ø·½ÏòÓĞ±ä»¯
//		{
//			dir_delay ++;
//			if(dir_delay==5000)//ÑÓÊ±£¬·ÀÖ¹Æµ·±×ª¶¯
//			{
//				if(pst!=pst_old)
//				{
//					dir_delay=0;
//					if(pst>15&&pst<30&&pst!=pst_old)//ÓÒ×ª
//					{
//						pst_old=pst;
//						flag=1;
//						Locate_Abs(((int)k_right*(pst-15)),7000);
//						
//						
//					}
//					else if(pst>0&&pst<15&&pst!=pst_old)//×ó×ª
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
