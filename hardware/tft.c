#include "tft.h"
#include "gpio.h"
#include "delay.h"
//写命令给TFF
//CS:片选信号端，置0有效
//RS:置1为选择数据，置0为选择命令
//WR:写信号控制端，//来一个低脉冲将数据写入
//RD:读写选择，置1选择写，置0选择读
void write_com_TFTLCD(u16 comd)
{
	u8 comdH,comdL;
	
	comdH = comd>> 8;
	comdL = comd&0x00FF;  //先进行数据换算，转为8位宽
	TFT_WR=1;//将WR拉高
	TFT_CS=0;	//打开TFT片选
	TFT_RS=0;	//选择命令
	TFT_RD=1;	//选择写
	
	TFT_dateoutH(comdH);//放置数据
	TFT_dateoutL(comdL);
	
	TFT_WR=0;	//来一个低脉冲将数据写入
	TFT_WR=1;	
	
	TFT_CS=1;	//关闭片选
}
//写数据给TFT
void write_date_TFTLCD(u16 date)
{
	u8 dateH,dateL;
	
	dateH = date >> 8;
	dateL = date & 0x00FF;  //先进行数据换算，转为8位宽
	TFT_WR=1;	//将WR拉高
	TFT_CS=0;	//打开TFT片选
		
	TFT_RS=1;	//选择数据
	TFT_RD=1;	//选择写
	
	TFT_dateoutH(dateH);//放置数据
	TFT_dateoutL(dateL);
//	dateout(date);
	
	TFT_WR=0;	//来一个低脉冲将数据写入
	TFT_WR=1;	
	
	TFT_CS=1;	//关闭片选
}
//写命令和数据
void TFT_Write_comddate(u16 comd,u16 date)
{
	write_com_TFTLCD(comd);
	write_date_TFTLCD(date);
}
//TFTLCD初始化，初始化时必须要进行复位控制
//CS:片选信号端，置0有效
//RS:置1为选择数据，置0为选择命令
//WR:写信号端
//RD:读写选择，置1选择写，置0选择读
void TFTLCD_init(void)
{
//	Gpio_init();//初始化I/O端口
//	TFT_CS=1;	//关闭片选
//	TFT_RD=1;	//I/O初始化，将RD置1 选择写
//	TFT_WR=1;	//写数据的信号端
//复位信号，一个低脉冲
	TFT_RESET=1;
	delay_us(420);
	TFT_RESET=0;
	delay_us(420);
	TFT_RESET=1;
	delay_us(420);
	
	TFT_CS=0;
	delay_us(420);
//Base Image Display Control
//Base Image Number of Line (R400h)
	TFT_Write_comddate(0x0400,0x3102);
    TFT_Write_comddate(0x0007,0x0021);
	delay_us(420);
    TFT_Write_comddate(0x0110,0x0001);
	delay_us(420); 
    TFT_Write_comddate(0x0100,0x17B0);//Power Control 1 (R100h) 电源控制
    TFT_Write_comddate(0x0112,0x0060);
    TFT_Write_comddate(0x0010,0x001F);//面板界面设置 Panel Interface Control 1
    TFT_Write_comddate(0x0101,0x0014);//Power Control 2 (R101h)
    TFT_Write_comddate(0x0102,0x019E);//Power Control3 (R102h)
    TFT_Write_comddate(0x0103,0x3200);//Power Control 4 (R103h)
    TFT_Write_comddate(0x0107,0x0000);//Power Control 5 (R107h)
	delay_us(420);
    TFT_Write_comddate(0x0102,0x01BE);
	delay_us(420); 
/****************寄存器地址，数据***********/
    TFT_Write_comddate(0x0001,0x0100);//TFT输出控制Driver Output Control，设置RGB点从0-720
    TFT_Write_comddate(0x0002,0x0000);//LCD Driver Wave control
    TFT_Write_comddate(0x0003,0x1230);//Entry Mode 入口模式设置AM=0为水平方向
    TFT_Write_comddate(0x0008,0x0808);//display control 2	屏幕前沿和后沿设置BP和FP都要大于2line BP+FP<=16
    TFT_Write_comddate(0x0009,0x0000);//display control 3	屏幕扫描设置置0为默认设置
    TFT_Write_comddate(0x000B,0x0010);//Low power control 低功耗控制，COL颜色控制26万色素，VEM低功耗设置
    TFT_Write_comddate(0x000C,0x0000);//外部显示界面寄存器，26万色RGB界面接口
    TFT_Write_comddate(0x0090,0x0000);
//gama Control 1 ~ 14 (R300h to R30Dh)
    TFT_Write_comddate(0x0300,0x0706);//
    TFT_Write_comddate(0x0301,0x0206);
    TFT_Write_comddate(0x0302,0x0002);
    TFT_Write_comddate(0x0303,0x0201);
    TFT_Write_comddate(0x0304,0x0300);
    TFT_Write_comddate(0x0305,0x0207);
    TFT_Write_comddate(0x0306,0x1416);
    TFT_Write_comddate(0x0307,0x0706);

    TFT_Write_comddate(0x0308,0x0206);
    TFT_Write_comddate(0x0309,0x0002);
    TFT_Write_comddate(0x030A,0x0201);
    TFT_Write_comddate(0x030B,0x0300);   
    TFT_Write_comddate(0x030C,0x0207);
    TFT_Write_comddate(0x030D,0x090B);
//Window Address Control
    TFT_Write_comddate(0x0210,0x0000);
    TFT_Write_comddate(0x0211,0x00EF);//Window Horizontal RAM Address Start (R210h), Window Horizontal RAM Address End(R211h)
	TFT_Write_comddate(0x0212,0x0000);//Window Vertical RAM Address Start (R212h), Window Vertical RAM Address End (R213h)
    TFT_Write_comddate(0x0213,0x018F);
//Base Image Display Control
//Base Image Display Control (R401h)
//Base Image Vertical Scroll Control (R404h)
    TFT_Write_comddate(0x0401,0x0000);
    TFT_Write_comddate(0x0404,0x0000);
	
    TFT_Write_comddate(0x0010,0x0010);//面板界面设置 Panel Interface Control 1
    TFT_Write_comddate(0x0011,0x0002);//面板界面设置 Panel Interface Control 2选内部时钟
    TFT_Write_comddate(0x0012,0x0100);//面板界面设置 Panel Interface Control 3
    TFT_Write_comddate(0x0101,0x0017);//Power Control 2 (R101h)
    TFT_Write_comddate(0x0200,0x0000);//RAM Address Set (Horizontal Address) (R200h)
    TFT_Write_comddate(0x0201,0x0000);//RAM Address Set (Vertical Address) (R201h)
    write_com_TFTLCD(0x0202);   	  //Write Data to GRAM (R202h) R/W控制位为 W，RS位为1 Read Data from GRAM (R202h) R/W控制位为R，RS位为1
    //display on
//    TFT_Write_comddate(0x0007,0x0021);//显示控制寄存器0x0007
//	delay_us(420);
//    TFT_Write_comddate(0x0007,0x0061);
//	delay_us(420);
    TFT_Write_comddate(0x0007,0x0173);
	delay_us(420); 
    write_com_TFTLCD(0x0202); 
}
//设置窗口的范围，起始地址，终止地址。
//xStart:X起始坐标
//yStart:y起始坐标
//xEnd	:x结束坐标
//yEnd	:y结束坐标
void TFT_SetWindow(u16 xStart,u16 yStart,u16 xEnd,u16 yEnd )
{
	TFT_Write_comddate(0x0200, xStart); //行地址寄存器设置0x0200
    TFT_Write_comddate(0x0201, yStart); //列地址寄存器设置0x0201

    TFT_Write_comddate(0x0210, xStart);
    TFT_Write_comddate(0x0212,yStart);
    
    TFT_Write_comddate(0x0211,xEnd);
    TFT_Write_comddate(0x0213,yEnd);

	write_com_TFTLCD(0x0202);
}

//按R5-G6-B5格式混合后的16位颜色码
//将电脑上常见的R8-G8-B8格式转换成16位单片机常用的R5-G6-B5格式
u16 RGB(u8 R,u8 G,u8 B)
{
	return ((u16)(R&0xf8<<8)|(u16)(G&0xfc)<<3|(u16)(B&0xf8)>>3);
}
