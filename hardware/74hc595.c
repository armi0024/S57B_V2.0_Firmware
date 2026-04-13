#include "74hc595.h"
#include "gpio.h"

void spi595(u8 u1,u8 u)
{
	u8 i,num,num1;
	num=u;	
	num1=u1;

	for(i=0;i<8;i++)	//--发送第一个字节--//位选
	{
		
//		delay_ms(2);
		if((num1&0x80)==0x80)			//先发送高位
		{
			HC595_DATA=1;				//最高位为1时，就发送1，否则就发送0
		}
		else
		{
			HC595_DATA=0;
		}
		S_CLK=0;						//来一个上升沿送出一位数据
		S_CLK=1;
//		delay_ms(2);
		num1=num1<<1;		

	}
	
	for(i=0;i<8;i++)
	{
		
//		delay_ms(2);
		if((num&0x80)==0x80)
		{
			HC595_DATA=1;
		}
		else
		{
			HC595_DATA=0;
		}
		S_CLK=0;
//		MOSIO=u1>>7;
		S_CLK=1;
//		delay_ms(2);
		num=num<<1;			//段选
		
	}
	R_CLK=0;						//产生上升沿，并行输出
//	delay_ms(2);
	R_CLK=1;
	R_CLK=0;	
}









