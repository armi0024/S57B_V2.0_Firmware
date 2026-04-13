#ifndef __OLED_H
#define __OLED_H

#include "mydef.h"
#include "stdlib.h"

//OLED模式设置
//0:4线串行模式
//1:并行8080模式
//#define OLED_MODE 0

//----------OLED端口定义--------------------			   
#define OLED_CS  PBout(12)		//片选
#define OLED_RST PAout(8) 		//复位
#define OLED_RS  PBout(13)		//数据/命令选择对应实物上DC引脚

//#define OLED_SCLK  PAout(11)		////时钟线对应实物上的(D0)引脚
//#define OLED_SDIN  PAout(12)	    ////数据线对应实物上的(D1)引脚
////PC0~7,作为数据线
//#define DATAOUT(x) GPIOC->ODR=(GPIOC->ODR&0xff00)|(x&0x00FF); //输出
  
//使用4线串行接口时使用 
#define OLED_SCLK PBout(15)		//时钟线对应实物上的(D0)引脚
#define OLED_SDIN PBout(14)		//数据线对应实物上的(D1)引脚
		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	//写数据、命令    

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);							//更新整屏		   
							   		    
void OLED_Init(void);									//OLED初始化
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);					//OLED画一个点

void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);			//OLED填充区域
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);	//显示ASCII字符
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);	//显示数字
void OLED_ShowString(u8 x,u8 y,const u8 *p);	 		//显示字符串
  
u32 oled_pow(u8 m,u8 n);//m^n次方
void OLED_Showword(u8 x,u8 y,u8 *num,u8 mode);




#endif
