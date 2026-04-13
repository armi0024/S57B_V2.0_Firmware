#include "smg.h"
#include "delay.h"

u8 table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
			0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
u8 temp[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};

u8 data_sbuff[6];
//功能:温度处理
void temp_deal(short temp)
{
	data_sbuff[0]=table[temp/1000];
	data_sbuff[1]=table[temp%1000/100]|0x80;//此处改精确度
	data_sbuff[2]=table[temp%10000%100/10];
	data_sbuff[3]=table[temp%10];
}
//功能:数码管显示
void SMG_display(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		spi595(temp[i+1],data_sbuff[i]);	
		delay_ms(1);	
		
	}
	spi595(0xff,0x00);	//消影
}













