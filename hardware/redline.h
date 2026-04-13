#ifndef __REDLINE_H
#define __REDLINE_H

#include <stm32f10x.h>
#include "sys.h"

#define RDATA PBin(9)	 //红外数据输入脚

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
//#define REMOTE_ID 0      		   

extern u8 RmtCnt;	//按键按下的次数

void Remote_Init(void);    //红外传感器接收头引脚初始化
void Remote_Scan(void);
void Ird_date_process(u8 dat);

void Ird_display(void);

#endif


