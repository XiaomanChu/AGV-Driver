#include "sys.h"

extern u32 tempup1;	//捕获总高电平时间
extern u32 tempup2;	//捕获总高电平时间
extern u32 tempup3;	//捕获总高电平时间
extern u32 tempup4;	//捕获总高电平时间
extern u32 f1;
extern u32 f2;
extern u32 f3;
extern u32 f4;
void TIM3_Cap_Init(u16 arr, u16 psc);
