#ifndef __74HC595_H
#define __74HC595_H
#include <stm32f10x.h>
#include "sys.h"


#define HC595_DATA PGout(15)  //=P3^4;
#define R_CLK PGout(13)  //=P3^5;
#define S_CLK PGout(12)  //=P3^6;

void spi595(u8 u0,u8 u);

#endif
