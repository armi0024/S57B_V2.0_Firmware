#include "delay.h"
unsigned int fac_us,fac_ms;

//晶振：内部默认8MHz 但实际需要二分频作为内部时钟源（4M），16倍频后主频是64MHz
//    如果用外部8MHz 实际不需要将外部晶振2分频，作为时钟源的，最高主频可以达到72MHz 

void stm32_SysClock72M(u8 PLL)//系统时钟函数，配置系统时钟
{
	unsigned char temp=0;
	RCC->CR=0X00000083;//复位时钟控制寄存器,默认内部8M时钟准备就绪
    #if 0
        RCC->CR|=0X00010000;//把时钟控制寄存器的第16位置1来开启外部振荡 
        while(!(RCC->CR>>17));//通过判断时钟控制寄存器的第17位，来查看外部振荡器是否就绪（即第17位是否被硬件置1）
	#endif
    #if 1
        while(!(RCC->CR&0x02));//通过判断时钟控制寄存器的第2位，来查看内部振荡器是否就绪（即第2位是否被硬件置1）
    #endif
    RCC->CFGR=0X00000400;//通过设置时钟配置寄存器（CFGR）的8~10位(PPRE1)来保证APB1时钟频率不超过36MHZ
    #if 0                //如果用外部晶振需要减2即 1001（9）（PLL 11倍频）-2= 7（0111，PLL 9倍频）
        PLL-=2;				 //倍频系数减二
    #endif
	RCC->CFGR|=PLL<<18;   //设置时钟配置寄存器的18~21位PLL
	RCC->CFGR|=1<<16;		//设置时钟配置寄存器的第16位，置1时选择外部高速振荡器作为PLL输入时钟
	FLASH->ACR|=0X32;		//FLASH延时2个周期
	RCC->CR|=0X01000000;	//设置时钟控制寄存器的第24位，打开PLL
	while(!(RCC->CR>>25));	//通过判断时钟控制寄存器的第25位，来确定PLL是否由硬键置1
	RCC->CFGR|=0X00000002;	//设置时钟配置寄存器的0~1位来选择系统时钟，00为内部高速，01为外部高速，10为PLL（锁相环）
	while(temp!=0x02)		//通过查询时钟配置寄存器的第2~3位，判断系统时钟是否准备就绪，00HSI作为系统时钟，01HSE作为系统时钟，10PLL作为系统时钟
	{
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}
}

void delay_init(u8 SYSCLK)//延时函数初始化
{
	
	SysTick->CTRL&=0xfffffffb;//选外部时钟
	fac_us=SYSCLK/8;
	fac_ms=fac_us*1000;
}

void delay_us(u32 nus)//延时微秒级别
{
	u32 temp;
//	delay_Init(72);//延时函数初始化
	SysTick->LOAD=nus*fac_us;
	SysTick->VAL=0x00;//val寄存器清零后自动重装
	SysTick->CTRL=0x01;//开始倒数（计时）
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));
	SysTick->CTRL=0x00;
	SysTick->VAL=0x00;
}

void delay_ms(u32 nms)//延时毫秒级别
{
	u32 temp;
//	delay_Init(72);//延时函数初始化
	SysTick->LOAD=nms*fac_ms;
	SysTick->VAL=0x00;//val寄存器清零后自动重装
	SysTick->CTRL=0x01;//开始倒数（计时）
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));
	SysTick->CTRL=0x00;
	SysTick->VAL=0x00;
}

void System_Clock_Init(void)
{
    SystemInit();
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	
 	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
}







