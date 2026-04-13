#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f10x.h"

void stm32_SysClock72M(u8 PLL);//系统时钟函数，配置系统时钟
void delay_init(u8 SYSCLK);//延时函数初始化
void delay_us(u32 nus);//延时微秒级别
void delay_ms(u32 nms);//延时毫秒级别
void System_Clock_Init(void);



#endif















