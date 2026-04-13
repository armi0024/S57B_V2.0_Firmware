#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
//#include "gpio.h"
//////////////////////////////////////////////////////////////////////////////////	

//OLED的显存
//存放格式如下.
//好处:刷整屏时，可以保持某些元素不变。
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];

//更新显存到LCD		 
void OLED_Refresh_Gram(void)//每次更新都是以整屏更新。
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
		OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
#if OLED_MODE==1
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATAOUT(dat);	    
 	OLED_RS=cmd;
	OLED_CS=0;	   
	OLED_WR=0;	 
	OLED_WR=1;
	OLED_CS=1;	  
	OLED_RS=1;	 
} 	    	    
#else
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	OLED_RS=cmd; //写命令 
	OLED_CS=0;		  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK=0;
		if(dat&0x80)OLED_SDIN=1;
		else OLED_SDIN=0;
		OLED_SCLK=1;
		dat<<=1;   
	}				 
	OLED_CS=1;		  
	OLED_RS=1;   	  
} 
#endif	     
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
	for(n=0;n<128;n++)
	OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空
//基地址：将屏幕引脚朝上，正对着自己，左下角为基地址，且向上/向右依次地址递增。Y（0~63）；X(0~127)
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;		//计算列偏移量
	bx=y%8;			//计算该点在对应列地址中的偏移位。
	temp=1<<(7-bx);	//将数据移至对应位。
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //调用1206字体
		else temp=oled_asc2_1608[chr][t];		 //调用1608字体 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    } 
	OLED_Refresh_Gram();//更新显示	
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1;  
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	   
//定义一个结构体
//所建字库大小16x16
struct wordtypedef		 // 汉字字模数据结构 
{
	u8 Index[2];		  // 汉字内码索引,一个汉字占两个字节	
	u8 Msk[32];				// 点阵码数据(16*16/8) 
};
struct wordtypedef word[]={

{{"创"},{0x02,0x00,0x04,0x00,0x0B,0xFC,0x32,0x02,0xC2,0x22,0x22,0x12,0x13,0xE2,0x08,0x02,0x04,0x0E,0x00,0x00,0x1F,0xF0,0x00,0x02,0x00,0x01,0xFF,0xFE,0x00,0x00,0x00,0x00}},/*"?",0*/
//{{"欢"},{0x20,0x08,0x24,0x10,0x22,0x60,0x21,0x80,0x26,0x41,0x39,0x32,0x02,0x04,0x0C,0x18,0xF0,0x60,0x13,0x80,0x10,0x60,0x10,0x18,0x14,0x04,0x18,0x02,0x00,0x01,0x00,0x00}},/*"?",7*/
//{{"迎"},{0x02,0x00,0x02,0x02,0x42,0x04,0x33,0xF8,0x00,0x04,0x00,0x02,0x3F,0xF2,0x20,0x22,0x40,0x42,0x00,0x02,0x3F,0xFE,0x20,0x42,0x20,0x22,0x3F,0xC2,0x00,0x02,0x00,0x00}},/*"?",0*/
//{{"光"},{0x02,0x01,0x02,0x01,0x42,0x02,0x22,0x04,0x1A,0x18,0x03,0xE0,0x02,0x00,0xFE,0x00,0x02,0x00,0x03,0xFC,0x0A,0x02,0x12,0x02,0x62,0x02,0x02,0x02,0x02,0x1E,0x00,0x00}},/*"?",1*/
//{{"临"},{0x00,0x00,0x1F,0xF8,0x00,0x00,0x00,0x00,0xFF,0xFF,0x02,0x00,0x04,0x00,0x18,0xFE,0xF0,0x84,0x18,0x84,0x16,0xFC,0x10,0x84,0x10,0x84,0x10,0xFE,0x10,0x00,0x00,0x00}},/*"?",2*/
};

//写一个汉字的GUI函数
//检索自己在内部Flash所建的字库
//
void OLED_Showword(u8 x,u8 y,u8 *num,u8 mode)
{
	u8 t,t1,t2;
	u8 temp;
	u8 y0=y;
//	t=*num;
	for(t=0;t<50;t++)	//搜索创建的的字库，t为字库的大小
	{
		if((*num==word[t].Index[0])&&(*(num+1)==word[t].Index[1]))//通过指针*num与所要的字进行对比！
		{
			for(t1=0;t1<32;t1++)		//汉字的取模编码t1
			{
				temp=word[t].Msk[t1];
				for(t2=0;t2<8;t2++)	//扫描8次
				{
					if(temp&0x80)			//字和字母符号都是由一个个像素点构成扫描次数与字模有关
						OLED_DrawPoint(x,y,1);
					else
						OLED_DrawPoint(x,y,0);
					temp<<=1;
					y++	;				//字或字符的高度增加
//					if(y>=127) {return ;}		//超过x轴的范围就跳出
					if((y-y0)==16)		//对于16*16字体,扫描两次将高度扫完
					{
						y=y0;
						x++;			//每扫描完两个字节后x加1,32个字节，x加16
						if(x>=127) return;	
						break;				//超出区域就退出GUI函数
					}
				}
			}//写完一个字
		
		}
	}	
		OLED_Refresh_Gram();//更新显示	
}

//初始化SSD1306					    
void OLED_Init(void)
{ 	 

    #if 1
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟 
	RCC->APB2ENR |=1<<2;		//使能GPIOA时钟

    GPIOA->CRH &=0XFFFffFF0;	//PA11、12 推挽输出
    GPIOA->CRH |=0X00000003;	
    GPIOA->ODR |=1<<8;		    //PA11、12 初始化输出高电平	

  	GPIOB->CRH&=0X0000FFFF;//PB8 推挽输出 
  	GPIOB->CRH|=0X33330000;	
	GPIOB->ODR|=0xF<<12;
	
	#endif
    #if 0
    	RCC->APB2ENR|=1<<3;    //使能PORTB时钟 
        RCC->APB2ENR |=1<<2;		//使能GPIOA时钟

        GPIOB->CRL&=0X00FFFFFF;//PB6,7 推挽输出 
        GPIOB->CRL|=0X33000000;	 

        GPIOB->CRH&=0XFFFFFFF0;//PB8 推挽输出 
        GPIOB->CRH|=0X00000003;	
        GPIOB->ODR|=1<<8;
        
        GPIOA->CRH &=0XFFF00FFF;	//PA11、12 推挽输出
        GPIOA->CRH |=0X00033000;	
        GPIOA->ODR |=1<<11;		    //PA11、12 初始化输出高电平	
        
    #endif
									  
	OLED_RST=0;			  		//复位
	delay_ms(100);
	OLED_RST=1; 
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD);//设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);  //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD);//设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD);//默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD);//设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD);//默认为0

	OLED_WR_Byte(0x40,OLED_CMD);//设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD);//bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD);//设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD);//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD);//段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD);//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD);//设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD);//[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD);//对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD);//1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD);//设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD);//[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD);//设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	OLED_WR_Byte(0xA4,OLED_CMD);//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD);//设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD);//开启显示
	delay_ms(100);
	OLED_Clear();
}  





























