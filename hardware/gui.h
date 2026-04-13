#ifndef __GUI_H
#define __GUI_H
#include "stm32f10x.h"

void TFT_ClearScreen(u16 color);		//清屏并涂色
void GUI_Drawpoint(u16 sx,u16 sy,u16 color);//画点GUI函数
void GUI_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);//指定区域，进行填充指定颜色
void GUI_Drawline(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//画线GUI函数
void GUI_DrawRectangle(u16 x1,u16 x2,u16 y1,u16 y2,u16 color);//画矩形
void GUI_DrawCircle(u16 x0,u16 y0,u8 r,u16 color);//画圆

void TFT_Showchar(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 wordcolor,u16 blackcolor);									//写一个字符GUI函数1608
void TFT_Shownum(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 wordcolor,u16 backcolor);									//写一个数字GUI函数, 与写一个字符函数算法相同，只是函数名变了一下
void TFT_Showchar1624(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 wordcolor,u16 backcolor);								//写一个字符GUI函数,字号1624

void TFT_Showword(u16 x,u16 y,u8 *num,u8 mode,u16 wordcolor,u16 blackcolor );										//写一个汉字GUI函数
void TFT_Writeword(u16 x,u16 y,u16 width,u16 height,u8 wordsize,u8 *p,u16 wordcolor,u16 blackcolor);				//写一串字

void TFT_ShowString(u16 x,u16 y,u16 width,u16 height,u8 wordsize,u8 *p,u16 wordcolor,u16 blackcolor,u8 select);		//显示字符串GUI函数

#endif
