#ifndef __DS18B20_H
#define __DS18B20_H

#include "mydef.h"

//IO方向设置
#define ds18b20_io_in()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=0X00000080;}		//配置GPIOE为上/下输入模式
#define ds18b20_io_out() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=0X00000030;}		//配置GPIOE为推挽输出
//IO位绑定											   
#define ds18b20_dq_in PBin(1)			//位输入
#define ds18b20_dq_out PBout(1)			//位输出

void ds18b20_rst(void);				//复位DS18B20
u8 ds18b20_check(void);				//检测DS18B20的存在
u8 ds18b20_read_bit(void);			//读一位数据函数
u8 ds18b20_read_byte(void);			//读一个字节函数
void ds18b20_write_byte(u8 dat);	//写一个字节函数
void ds18b20_start(void);			//开始转换温度函数
void ds18b20_readtemp_com(void);	//读温度指令函数
u8 ds18b20_init(void);				//ds18b20初始化
short ds18b20_get_temp(void);		//得到温度值函数
void temperature_deal(short temp);	//温度处理及显示函数 

#endif

