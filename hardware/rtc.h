#ifndef __RTC_H
#define __RTC_H
#include <stm32f10x.h>
#include "sys.h"

//定义一个时间结构体
struct date_typedef 
{
	u8 hour;
	u8 min;
	u8 sec;
	//公历年月日
	u16 w_year;
	u8 w_month;
	u8 w_date;
	u8 week;
};
extern struct date_typedef  calendar;//日历结构体 date_typedef

u8 RTC_Init(void);														//实时时钟初始化
void RTC_IRQHandler(void);													//RTC秒中断
u8 Is_Leap_Year(u16 year);													//判断是否是闰年函数
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);				//设置RTC计数器的值,即时间初始设置
u8 RTC_Get(void);															//得到当前的时间
u8 RTC_Get_Week(u16 year,u8 month,u8 day);									//获得现在是星期几
void date_process(u16 year,u8 month,u8 day,u8 week,u8 hour,u8 min,u8 sec);	//日期显示处理函数
void Date_display(void);													//日期显示函数
	
#endif
