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

u8 RxProcess(void);//´®¿ÚÊı¾İ´¦Àí
void TxProcess(u8 sendData);//´®¿ÚÊı¾İ·¢ËÍ
int main(void)
{ 
	//u16 led0pwmval=0;    
	u8 temp=0;int i=0;
	u32 delay=0;//ÓÃÓÚ´®¿ÚÑÓÊ±·¢ËÍ£»
	u8 keyval;//°´¼ü¼ì²â
	long long count1=0;//¼ÆÊıÖµ
	long long count2=0;//¼ÆÊıÖµ
	long long count3=0;//¼ÆÊıÖµ
	long long count4=0;//¼ÆÊıÖµ
	int pst=0;
	float	k1,k2,k3;//²¶»ñ×ª»»ÏµÊı
	k1=(float)(500-0)/(float)(215-87);//²¶»ñµÄ²âÁ¿ÖµÊÇ87~215£¬±ê¶¨µ½0~500
	k2=(float)100000/(float)(21-15);
	k3=(float)100000/(float)(15-8);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÏµÍ³ÖĞ¶ÏÓÅÏÈ¼¶·Ö×é2
	delay_init(168);  //³õÊ¼»¯ÑÓÊ±º¯Êı
	uart_init(115200);//³õÊ¼»¯´®¿Ú²¨ÌØÂÊÎª115200
	IO_Init();
 	PWMInit(500-1,84-1);	//84M/84=1MhzµÄ¼ÆÊıÆµÂÊ,ÖØ×°ÔØÖµ500£¬ËùÒÔPWMÆµÂÊÎª 1M/500=2Khz.  
  TIM_SetCompare1(TIM5,500);	//ĞŞ¸Ä±È½ÏÖµ£¬ĞŞ¸ÄÕ¼¿Õ±È, 500¶ÔÓ¦Õ¼¿Õ±È0
	TIM_SetCompare2(TIM5,500);
	TIM_SetCompare3(TIM5,500);
	TIM_SetCompare4(TIM5,500);
	
	KEY_Init();					//°´¼ü³õÊ¼»¯
	//usmart_dev.init(84); 	//³õÊ¼»¯USMART			
	TIM4_Cap_Init(0XFFFFF,84-1); //ÒÔ1MhzµÄÆµÂÊ¼ÆÊı £¬Òç³öÊ±¼ä65535us,Ò£¿ØÆ÷½ÓÊÕÆ÷×î³¤Âö¿í2160us
  TIM3_Cap_Init(0xffff,84-1); //ÒÔ1MhzµÄÆµÂÊ¼Æ
	
	Driver_Init();			//²½½øµç»úÇı¶¯Æ÷³õÊ¼»¯
	TIM8_OPM_RCR_Init(999,168-1); //TIM8Îª168MHzËùÒÔÊÇ1MHz¼ÆÊıÆµÂÊ  µ¥Âö³å+ÖØ¸´¼ÆÊıÄ£Ê½  
#if 1	
   while(1) 
	{
 		delay_ms(10);
		
#if 1    //PC¶Ë´®¿Úµ÷ÊÔ³ÌĞò
		temp= RxProcess();
		if(USART_RX_BUF[0]!=0)printf("µç»úËÙÂÊ:%d\r\n",100-temp);
		//Çå³ı´®¿ÚÊı¾İ,×¼±¸ÏÂÒ»´Î½ÓÊÕ
		for(i=0;i<USART_RX_STA;i++)
		{
			USART_RX_BUF[i]=0;
		}
		USART_RX_STA=0;
#endif
		
#if 1 //Ò£¿Ø³ÌĞò
		delay_ms(10);		 

		count1=tim4up1/10;		   //È¥µô×îºóÒ»Î»²»ÎÈ¶¨µÄÊı£¨´¦ÀíÖ®ºó·¶Î§´ó¸ÅÔÚ87~215£©
		count2=tim4up2/10;
		count3=tim4up3/10;		   //È¥µô×îºóÒ»Î»²»ÎÈ¶¨µÄÊı£¨´¦ÀíÖ®ºó·¶Î§´ó¸ÅÔÚ87~215£©
		count4=tim4up4/10;
		
		
	//printf("HIGH:%lld us\r\n",count1); 
		//printf("DIR: %lld \n",count2);
		//printf("DIR: %lld \n",count3);
	
	
		//½«²¶»ñ¼ÆÊı×ª»»³ÉPWMµç»ú¿ØÖÆ
		//count1=count1/10;//
		if(count1<300)
		{
			count1=500-(int)(k1*(count1-87));//±ê¶¨µ½£¨500£¬0£©Çø¼ä
			TIM_SetCompare1(TIM5,count1);
			TIM_SetCompare2(TIM5,count1);
			TIM_SetCompare3(TIM5,count1);
			TIM_SetCompare4(TIM5,count1);
		}
		if(count2<109)
		{
			GPIO_SetBits(GPIOH,GPIO_Pin_9);//Õı×ª
			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
			GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹Ä
		}
		else if(count2>194&&count2<300)
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//·´×ª
			GPIO_SetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
			GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹Ä
		}
		else if(count2>145&&count2<160) 
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_15);//¹Ø±ÕÊ¹ÄÜ
			
		}
		pst=count3/10;
		//printf("PST:%lld us\r\n",(long long)pst);
		if(count3>160&&count3<300)
		{
			
			Locate_Abs((int)k2*(pst-15),58000);
			printf("PST:%lld us\r\n",(long long)(int)k3*(pst-15));
		}
		else if(count3<145)
		{
			Locate_Abs(-(int)k3*(pst-8),58000);
			printf("PST:%lld us\r\n",(long long)-(int)k3*(pst-8));
		}
		else if(count3>145&&count3<160)Locate_Abs(0,58000);
#endif
		
#if 0 //ËÙ¶È·´À¡³ÌĞò
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
			Locate_Abs(0,500);//°´ÏÂWKUP£¬»ØÁãµã
		}else if(keyval==KEY0_PRES)
		{
			Locate_Rle(200,500,CW);//°´ÏÂKEY0£¬ÒÔ500HzµÄÆµÂÊ Ë³Ê±Õë·¢200Âö³å
		}else if(keyval==KEY1_PRES)
		{
			Locate_Rle(400,500,CCW);//°´ÏÂKEY1£¬ÒÔ500HzµÄÆµÂÊ ÄæÊ±Õë·¢400Âö³å
		}			
		delay_ms(10);
	}
#endif
}


u8 RxProcess(void)//´®¿ÚÊı¾İ´¦Àí
{
	u8 temp = 0;
	float k;

	switch(USART_RX_BUF[0])//Ñ¡ÔñÄ£Ê½
	{
		case 0x10://10Ôİ¶¨µç»úµ÷ËÙÄ£Ê½
		{
			//GPIO_ResetBits(GPIOH,GPIO_Pin_9);
			GPIO_SetBits(GPIOH,GPIO_Pin_9);//Õı×ª
			GPIO_ResetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
			temp = USART_RX_BUF[1];
			k=(float)(500-0)/(float)(100-0);
			if(temp<=100)
			{
				TIM_SetCompare1(TIM5,(int)k*temp);
				TIM_SetCompare2(TIM5,(int)k*temp);
				TIM_SetCompare3(TIM5,(int)k*temp);
				TIM_SetCompare4(TIM5,(int)k*temp);
				GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹Ä
			}
			else	printf("Êı¾İ´íÎó\r\n");
		}break;
		case 0x11:
		{
			GPIO_ResetBits(GPIOH,GPIO_Pin_9);//·´×ª
			GPIO_SetBits(GPIOH,GPIO_Pin_14);
			GPIO_SetBits(GPIOH,GPIO_Pin_13);//²»É²³µ
			//GPIO_SetBits(GPIOH,GPIO_Pin_9);
			temp = USART_RX_BUF[1];
			k=(float)(500-0)/(float)(100-0);
			if(temp<=100)
			{
					TIM_SetCompare1(TIM5,(int)k*temp);
					TIM_SetCompare2(TIM5,(int)k*temp);
					TIM_SetCompare3(TIM5,(int)k*temp);
					TIM_SetCompare4(TIM5,(int)k*temp);
					GPIO_SetBits(GPIOH,GPIO_Pin_15);//µç»úÊ¹Ä
			}
		}break;
		case 0X12:
			GPIO_ResetBits(GPIOH,GPIO_Pin_15);break;//¹Ø±ÕÊ¹ÄÜ
			
		case 0X13:
			GPIO_ResetBits(GPIOH,GPIO_Pin_13);break;//É²³µ
	}
	return temp;//·µ»ØÕ¼¿Õ±È
}
void TxProcess(u8 sendData)//´®¿ÚÊı¾İ·¢ËÍ
{
	switch(USART_RX_BUF[0])
	{	
		case 0x10:
			USART_SendData(USART1,sendData);
	}
}
