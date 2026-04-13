/************************************************************************************/
//注意: 读取时间间隔应大于15；时间间隔过短，可能导致温湿度测量不准
/***********************************************************************************/
#include "dht11.h"
#include "delay.h"
#include "oled.h"


u8 temperature;				//定义一个装温度的变量
u8 humidity; 				//定义一个装湿度的变量
u8 DHT11[4];

//复位DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//拉低DQ
    delay_ms(20);    	//拉低至少18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}
//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}
//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
//采样输出数据为一帧 40bit的数据，高位先出。
//数据格式:40bit: 16bit湿度数据 16bit温度数据 8bit校验和其中
//8bit校验和=8bit湿度整数数据+湿度小数数据+8bit温度整数数据+8bit温度小数数据（进位丢失）
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])//校验数据
		{
			*humi=buf[0];			//把采集到的湿度数据给指针。
			*temp=buf[2];			//把采集到的温度数据给指针。
		}
	}else return 1;
	return 0;	    
}
//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
u8 DHT11_Init(void)
{
	RCC->APB2ENR|=1<<3;    //使能PORTB口时钟 
	GPIOB->CRH&=0XFFFFFFF0;//PORTB.0 推挽输出
	GPIOB->CRH|=0X00000003;
	GPIOB->ODR|=1<<0;      //输出1				    
	DHT11_Rst();
	return DHT11_Check();
}
//功能:温湿度数据处理及显示
//temp:传递来的温度值
//humi:传递来的湿度值
void DHT11_data_deal(u8 temp,u8 humi)
{
	u8 i, len;
	DHT11[0]=temp/10;	//温度值处理
	DHT11[1]=temp%10;	
	
	DHT11[2]=humi/10;	//湿度值处理
	DHT11[3]=humi%10;
	for(i=0;i<4;i++)
	{
		len=i*8;
		if(i<2)
		{
			OLED_ShowChar(72+len,31,DHT11[i]+'0',16,1);//显示ASCII
		}
		else 
		{
			len-=16;
			OLED_ShowChar(72+len,16,DHT11[i]+'0',16,1);//显示ASCII
		}
	}
}

//温湿度显示
void temp_humi_display(void)
{
	DHT11_Read_Data(&temperature,&humidity);//读取温度值
	DHT11_data_deal(temperature,humidity);	//处理及显示温湿度值
}




















