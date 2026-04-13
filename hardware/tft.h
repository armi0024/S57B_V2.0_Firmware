#ifndef __TFT_H
#define __TFT_H
#include <stm32f10x.h>

//#define   TFT_dateout(x)  {GPIOD->BRR =0xFFFF;\
//GPIOD->BSRR =x & 0xFFFF;}        //这样宏定义不影响其他的P口,P1

#define   TFT_dateoutH(x)  {GPIOD->BRR =0x00FF;\
GPIOD->BSRR =x & 0x00FF;}        //这样宏定义不影响其他的P口,P0
#define   TFT_dateoutL(x)  {GPIOD->BRR = 0xFF00;\
GPIOD->BSRR =((u16)x << 8);}    //这样宏定义不影响其他的P口,P1


//#define   TFT_dateoutH(x)  {GPIOA->BRR =0x00FF;\
//GPIOA->BSRR =x & 0x00FF;}        //这样宏定义不影响其他的P口,P1
//#define   TFT_dateoutL(x)  {GPIOB->BRR = 0xFF00;\
//GPIOB->BSRR =((u16)x << 8);}    //这样宏定义不影响其他的P口,P0

//#define dateout(x) GPIOD->BSRR=x
//---定义屏的大小---//
#define TFT_XMAX ((u16)239)//		//设置TFT屏的大小，X最大0-239，Y最大0-399
#define TFT_YMAX ((u16)399)//

//---定义颜色的宏---//
#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0		 //定义颜色的宏

void write_com_TFTLCD(u16 comd);
void write_date_TFTLCD(u16 date);
void TFT_Write_comddate(u16 comd,u16 date);
void TFTLCD_init(void);
void TFT_SetWindow(u16 xStart,u16 yStart,u16 xEnd,u16 yEnd );

u16 RGB(u8 R,u8 G,u8 B);

#endif

