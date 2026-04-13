#include "adc.h"
#include "delay.h"
//功能:ADC采集
//ADC初始化
void ADC1_init(void)
{
	//先初始化IO口
 	RCC->APB2ENR|=1<<2;    //使能PORTA口时钟 
//	GPIOA->CRL&=0XFFFF000;//PA1.0anolog输入
	//通道10/11设置			 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 
	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
//	ADC1->CR1|=1<<8;    //扫描模式	  
	
	ADC1->CR1&=~(1<<8);    //非扫描模式	  
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐	 
	
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1|=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1
	ADC1->SMPR1&=~(7<<18);
	ADC1->SMPR1|=7<<18;
	//设置通道1的采样时间
	ADC1->SMPR2&=~(7<<3);  //通道1采样时间清空	  
 	ADC1->SMPR2|=7<<3;     //通道1  239.5周期,提高采样时间可以提高精确度
	ADC1->CR2|=1<<23;
	ADC1->CR2|=1<<0;	   //开启AD转换器	 
	ADC1->CR2|=1<<3;       //使能复位校准  
	while(ADC1->CR2&1<<3); //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  

}

//得到采集到的数据，即原始值
u16 Get_adc(u8 ch)
{
	ADC1->SQR3&=0XFFFFFe0;//规则序列1  通道ch   1100 0000 0000
	ADC1->SQR3|=ch;			// 000010 0000
	ADC1->CR2|=1<<22;		//启动规则通道
	while(!(ADC1->SR&1<<1));//等待转换结束
	return ADC1->DR;		//返回adc的值
}


//温度采集函数
u16 Get_temperature(void)//得到采集到的数据，即原始值
{
	u16 temp=0;
	u8 t;
	for(t=0;t<10;t++)
	{
//		temp+=Get_adc(16);
		temp+=Get_adc(1);//通道1
		delay_ms(5);
	}
	return temp/10;			//返回温度的平均值
}
