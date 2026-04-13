#include "ds18b20.h"
#include "oled.h"

u8 string[]={0,1,2,3,4,5,6,7,8,9};
u8 temp_sbuff[8];

///复位DS18B20
void ds18b20_rst(void)
{
	ds18b20_io_out();		//配置IO方向为输出
	ds18b20_dq_out=0;		//将数据总线拉为低电平
	delay_us(750);			//延时750us
	ds18b20_dq_out=1;		//再将数据总线拉为高电平
	delay_us(15);			//延时15us
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 ds18b20_check(void)
{
	u8 retry=0;
	ds18b20_io_in();		//配置IO方向为输入
	while(ds18b20_dq_in)	//等待数据总线被拉低，等待5ms,超过5ms,,则未检测到ds18b20的存在
	{
		delay_ms(1);
		retry++;
		if(retry>5)
			return 1;		//返回1:未检测到DS18B20的存在
	}
//	while(ds18b20_dq_in&&retry<200)
//	{
//		retry++;
//		delay_us(1);
//	};
//	if((retry>=200)) return 1;
//	else retry=0;
//	while(!ds18b20_dq_in&&retry<240)
//	{
//		retry++;
//		delay_us(1);
//	};
//	if((retry>=240)) return 1;
	return ds18b20_dq_in;	//返回0:存在

}
//从DS18B20读取一个位
//返回值：1/0
u8 ds18b20_read_bit(void)
{
	u8 data;
	ds18b20_io_out();
	ds18b20_dq_out=0;		//先将总线拉低2us
	delay_us(2);
	ds18b20_dq_out=1;		//然后释放总线
	ds18b20_io_in();		//配置IO为输入
	delay_us(12);			//延时12us等待数据稳定
	if(ds18b20_dq_in) data=1; //读取数据，从最低位开始读取
	else data=0;
//	delay_us(50);
	return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
u8 ds18b20_read_byte(void)
{
	u8 i,j,dat;
	dat=0;
	for(i=0;i<8;i++)
	{
		j=ds18b20_read_bit();	//读取一位
		dat=(j<<7)|(dat>>1);
		delay_us(50);			//读取完之后等待50us再接着读取下一个数
	}
	return dat;					//返回数据
}
//写一个字节到DS18B20
//dat：要写入的字节
void ds18b20_write_byte(u8 dat)
{
	u8 j;
	u8 testb;
	ds18b20_io_out();			//配置IO方向为输出
	for(j=0;j<8;j++)
	{
		testb=dat&0x01;			//从低位往高位依次发送数据
		if(testb)
		{
			ds18b20_dq_out=0; 	//每写入一位数据之前先把总线拉低2us
			delay_us(2);
			
			ds18b20_dq_out=1;	//然后写入一个数据，从最低位开始
			delay_us(60);		//延时68us，持续时间最少60us
		}
		else
		{
			ds18b20_dq_out=0;
			delay_us(60);
			
			ds18b20_dq_out=1;
			delay_us(2);
		}
		dat=dat>>1;
	}
}
//开始温度转换
void ds18b20_start(void)
{
	
//	ds18b20_rst();
//	ds18b20_check();
	ds18b20_init();					//复位DS18B20
	delay_ms(1);					//复位成功后等待1ms
	ds18b20_write_byte(0xcc);		//写指令给ds18b20，跳过ROM指令
	ds18b20_write_byte(0x44);		//写指令，启动DS18B20进行温度转换
}
//读温度指令函数
void ds18b20_readtemp_com(void)
{
//   ds18b20_rst();
//   ds18b20_check();	 
	ds18b20_init();					//复位DS18B20
	delay_ms(1);					//复位成功后等待1ms	
    ds18b20_write_byte(0xcc);		//写指令给ds18b20，跳过ROM指令    //skip rom
    ds18b20_write_byte(0xbe);		//写读温度指令，convert	  
	
}
//初始化DS18B20的IO口 DQ 同时检测DS18b20的存在
//返回1:不存在
//返回0:存在    
u8 ds18b20_init(void)
{
	RCC->APB2ENR|=1<<6;    //使能PORTE口时钟 
	GPIOE->CRL&=0XFFFFFFF0;//PORTE.0 推挽输出
	GPIOE->CRL|=0X00000003;
//	GPIOE->ODR|=1<<0;      //输出1
	ds18b20_dq_out=1;		//先将总线拉为高电平
	ds18b20_rst();			//调用复位函数
	return ds18b20_check();
}
//读温度函数
short ds18b20_get_temp(void)
{
	u8 temp;
	u8 	TL,TH;
	short tem;
	ds18b20_start ();                   // ds1820 开始转换温度
	ds18b20_readtemp_com();				//写入读温度指令
	
    TL=ds18b20_read_byte(); // LSB   
    TH=ds18b20_read_byte(); // MSB  
	if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(float)tem*0.0625*100;//转换    
	if(temp)return tem; //返回温度值
	else 
		return -tem;    	
}
//功能:温度处理及显示函数 
//
void temperature_deal(short temp)
{
	u8 i=0,len=0;
	temp_sbuff[0]=string[temp/1000];
	temp_sbuff[1]=string[temp%1000/100] ;//此处改精确度
	temp_sbuff[2]=string[temp%10000%100/10];
	temp_sbuff[3]=string[temp%10];
	
	for(i=0;i<4;i++)
	{
		len=i*8;
		if(i<2)
		{
			OLED_ShowChar(40+len,0,temp_sbuff[i]+'0',16,1);//显示ASCII
		}
		else 
		{
			len-=16;
			OLED_ShowChar(64+len,0,temp_sbuff[i]+'0',16,1);//显示ASCII
		}
		
	}
}
