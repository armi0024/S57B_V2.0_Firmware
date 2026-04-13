/**********************************************************************
 *      Author: tstern
	Copyright (C) 2018  MisfitTech,  All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Written by Trampas Stern for MisfitTech.

    Misfit Tech invests time and resources providing this open source code,
    please support MisfitTech and open-source hardware by purchasing
	products from MisfitTech, www.misifittech.net!

 *********************************************************************/

//#include "syslog.h"
#include "as5047d.h"
#include "spi.h"
#include "usart.h"
//#include "board.h"



int16_t Anglecom[16]={0};
uint8_t error=false;
uint8_t as5047d=true;

//得到错误
uint8_t getError(void) {return error;};

static int getBit(int16_t data, int bit)
{
	return (data>>bit) & 0x01;
}
//得到奇偶校验位,数数据中1的位数
//return 1(odd)
//return 0(even)
static int getParity(uint16_t data)
{
	int i,bits;
	data=data & 0x7FFF; //mask out upper bit

	//count number of bits, brute force
	bits=0;
	for(i=0; i<16; i++){
//        if(data & 0x01) bits ++;
		if (0 != (data & ((0x0001)<<i))){
			bits++;
		}
	}
	return (bits & 0x01); //return 1 if odd
}
//As5047d 初始化 
void As5047d_SPI_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟

    //设置硬件上与SD卡相关联的控制引脚输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PB12 推挽 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PB12上拉
			 
	SPI2_Init();											//SPI2初始化
	AS5047_CS=1;
}

uint8_t As5047d_begin(void)
{
    uint16_t command = 0x0000;  //W
    

    As5047d_SPI_Init();
	delay_ms(2);

	error=0;
	printf("Begin AS5047D...\r\n");
       
    command = command|AS5047D_CMD_NOP;     //写+ARRD
    command |= ((uint16_t)getParity(command)<<15); //Add 校验位+写地址

    AS5047_CS=0;
	SPI2_ReadWriteByte(command);
    AS5047_CS=1;
	delay_us(10);

    command = 0x0000;
    command = command|AS5047D_CMD_SETTINGS1;     //写+ARRD
    command |= ((uint16_t)getParity(command)<<15); //Add 校验位+写地址

    AS5047_CS=0;
	SPI2_ReadWriteByte(command);
    AS5047_CS=1;
	delay_us(10);
    
    command = 0x0000;
    command = command|AS5047D_CMD_SETTINGS2;     //写+ARRD
    command |= ((uint16_t)getParity(command)<<15); //Add 校验位+写地址

    AS5047_CS=0;
	SPI2_ReadWriteByte(command);
    AS5047_CS=1;
	delay_us(10);
    
	//wait for the LF bit to be set
//	uint16_t data=0,t0=100;
//	while (getBit(data,8)==0 && t0>0)
//	{
//		delay_ms(1);
//		t0--;
//		if (t0==0)
//		{
//			UART1_Printf("LF bit not set\r\n");
//			error=1;
//			break;
//			//return false;
//		}
//		UART1_Printf("AS5047D diag data is 0x%04X\r\n",data);
//		data=readAddress(AS5047D_CMD_DIAAGC);
//	}
#ifdef NZS_AS5047_PIPELINE
	//read encoder a few times to flush the pipeline
	readEncoderAnglePipeLineRead();
	readEncoderAnglePipeLineRead();
#endif
	return 1;
}

void Write_As5047_cmd(uint16_t Addr,uint16_t Val)
{
    uint16_t data1 =0;
    uint16_t res;
    uint16_t command = 0x0000;  //W
    
    command = command|Addr;     //写+ARRD
    command |= ((uint16_t)getParity(command)<<15); //Add 校验位+写地址
    
    AS5047_CS=0;
	//clock out the address to read
	SPI2_ReadWriteByte(command);
	AS5047_CS=1;
	delay_us(10);
    
    command = 0x0000;  //W
    command = command|Val;     //写+Val
    command |= ((uint16_t)getParity(command)<<15); //Add 校验位+写地址

    AS5047_CS=0;
	//clock out the address to read
	SPI2_ReadWriteByte(command);
	AS5047_CS=1;
	delay_us(10);
    
    command = 0x4000;       // R
    command = command|AS5047D_CMD_NOP; 
    command |= ((uint16_t)getParity(command)<<15); //Add 校验位+读地址
    
    AS5047_CS=0;
	//clock out the address to read
	res= SPI2_ReadWriteByte(command);
	AS5047_CS=1;
	delay_us(10);
    if (res & (1<<14))
	{
		//if bit 14 is set then we have an error
		printf("Write command err");
		
		
	}else
    printf("Write command success!");
    
}
//read the encoders 
int16_t ReadAngle_Init(uint16_t addr)
{
	uint16_t data=0;
	//make sure it is a read by setting bit 14
    addr=addr | 0x4000;                             //设置读地址
	//add the parity to the command
    addr|=((uint16_t)getParity(addr)<<15);          //得到校验位后的命令

    if(1 == getParity(addr)){
        addr =(addr & 0x7fff) |0x8000;
    }
	AS5047_CS =0;
    delay_us(2);
	//clock out zeros to read in the data from address
	SPI2_ReadWriteByte(addr);
    AS5047_CS =1;
    delay_us(2);

//    UART1_Printf("read Data %d\r\n",data);
//    addr= 0x4000;                                   //设置读地址
//    addr=addr | AS5047D_CMD_NOP;                    //设置读地址
//	//add the parity to the command
//    addr|=((uint16_t)getParity(addr)<<15);
    
    AS5047_CS=0;
	//clock out the address to read
	data=SPI2_ReadWriteByte(0x00);
	AS5047_CS=1;
    
	if (data & (1<<14)){    //检查是否发生命令帧错误
		//if bit 14 is set then we have an error
		printf("read command 0x%04X failed\r\n",addr);
		error=1;
		return -1;
	}
	if (data>>15 != getParity(data)) //检查接收到的数据奇偶位，校验数据是否正确
	{
		//parity did not match
		printf("read command parity error 0x%04X \r\n",addr);
		error=1;
		return -2;
	}
	data=data & 0x3FFF; //mask off the error and parity bits
    
	return data;
}
//read the encoders 
int16_t readAddress(uint16_t addr)
{
	uint16_t data=0;
	//make sure it is a read by setting bit 14
//    TIM_Cmd(TIM1, DISABLE);  //使能TIM1
//    TIM_Cmd(TIM2, DISABLE);  //使能TIM1
//    TIM_Cmd(TIM3, DISABLE);  //使能TIM1
    TIM4->DIER&=~(1<<0);   //允许更新中断	  
    TIM4->CR1&=~(1<<0);    //使能定时器3
    EXTI->IMR &=~(1<<0);     //关闭外部中断
    
    addr=addr | 0x4000;                             //设置读地址
	//add the parity to the command
    addr|=((uint16_t)getParity(addr)<<15);          //得到校验位后的命令

    if(1 == getParity(addr)){
        addr =(addr & 0x7fff) |0x8000;
    }
	AS5047_CS =0;
    delay_us(2);
	//clock out zeros to read in the data from address
	SPI2_ReadWriteByte(addr);
    AS5047_CS =1;
    delay_us(2);

//    UART1_Printf("read Data %d\r\n",data);
//    addr= 0x4000;                                   //设置读地址
//    addr=addr | AS5047D_CMD_NOP;                    //设置读地址
//	//add the parity to the command
//    addr|=((uint16_t)getParity(addr)<<15);
    
    AS5047_CS=0;
//    delay_us(2);
	//clock out the address to read
	data=SPI2_ReadWriteByte(0x00);
	AS5047_CS=1;
    
	if (data & (1<<14)){    //检查是否发生命令帧错误
		//if bit 14 is set then we have an error
		printf("read command 0x%04X failed\r\n",addr);
		error=1;
		return -1;
	}
	if (data>>15 != getParity(data)) //检查接收到的数据奇偶位，校验数据是否正确
	{
		//parity did not match
		printf("read command parity error 0x%04X \r\n",addr);
		error=1;
		return -2;
	}
	data=data & 0x3FFF; //mask off the error and parity bits
    
//    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
    TIM4->DIER|=1<<0;   //允许更新中断	  
    TIM4->CR1|=0x01;    //使能定时器4
    
//    TIM_Cmd(TIM2, ENABLE);  //使能TIM1
//    TIM_Cmd(TIM3, ENABLE);  //使能TIM1
    EXTI->IMR |=1<<0;

	return data;
}

//read the encoders 
//返回整型数据
int16_t readEncoderAngle(void)
{
    int16_t data=0;
    int16_t m=0,d=0;
	if (as5047d)        
	{
        //data=readAddress(AS5047D_CMD_ANGLECOM);
        #if 0
            data=Average_Num_Filter();
        #else 
            #if 0
                data =Move_Filter();
            #endif
        #endif
//        m=(int)((float)data*AS5047D_DEGREES_PER_BIT);
//        d=(int)((float)data*AS5047D_DEGREES_PER_BIT*100 -m*100);
//        printf("DAECANG: 0x%04X(%d) %d.%02d deg(est) , ",data,data,m,d);
////        UART1_Printf("                  \r\n");
        return readAddress(AS5047D_CMD_ANGLECOM);  //Read As5047d 角度
    }
    return m;
}
//read the encoders 
//返回浮点型 
float ReadEncoderAngle(void)
{
    int16_t data=0;
    float m=0;
    int d=0,n=0;
	if (as5047d)        
	{
        //data=readAddress(AS5047D_CMD_ANGLECOM);
        #if 1
            data = Average_Num_Filter();
        #else 
            data = Move_Filter();
        #endif
        m=((float)data*AS5047D_DEGREES_PER_BIT*10);
       // m=m/10.0;
//        sprintf(ptrStr,"%sAGC: %d\n\r", ptrStr,data & 0x0FF);
//        UART1_Printf("DAECANG: 0x%04X(%d) %d.%02d deg(est) \r\n",data,data,m,d);
//        UART1_Printf("                  \r\n");
////        return readAddress(AS5047D_CMD_DIAAGC);  //Read As5047d 角度
        
    }
	return m;
}

//pipelined read of the encoder angle used for high speed reads, but value is always one read behind
int16_t readEncoderAnglePipeLineRead(void)
{

	int16_t data;
//	int error, t0=10;
//	AS5047_CS =0;
//	//delayMicroseconds(1);
//	do {
// 
//		// doing two 8 bit transfers is faster than one 16 bit
////		data =(uint16_t)SPI.transfer(0xFF)<<8 | ((uint16_t)SPI.transfer(0xFF) & 0x0FF);
//        data = SPI2_ReadWriteByte(0xffff);
//		t0--;
//		if (t0<=0){
//			printf("AS5047D problem");
//			break;
//		}
//		//data=SPI.transfer16(0xFFFF); //to speed things up we know the parity and address for the read
//	}while(data & (1<<14)); //while error bit is set

//	data=data & 0x3FFF; //mask off the error and parity bits
//	AS5047_CS =1;
    
    data=readAddress(AS5047D_CMD_ANGLECOM);

	//TODO we really should check for errors and return a negative result or something
	return data;
}
//去掉最大最小值取平均值
int16_t Average_Num_Filter(void)
{
    uint8_t N=18;
    static uint8_t t=0;
    uint16_t Min_value=0,Max_value =0;//标记一组数据中最大值和最小值
    
    uint32_t Sum=0;
    int16_t data=0;
   
    for(t=0;t<N;t++){
        data=readAddress(AS5047D_CMD_ANGLECOM);
        if(t==0)
            Max_value =Min_value=data;
//        UART1_Printf(" data = %d , ",data);
        
        if(data<0 && data >0x3fff)                 //判断数据是否超范围
            return 0;            
        if(data>=Max_value){
            Max_value =data;                        //得到最大值
        }
        if(data<=Min_value){
            Min_value = data;                       //得到最小值
        }
        Sum +=data;
//        UART1_Printf(" Max_value = %d , Min_value = %d, %d, Sum= %d\r\n",Max_value , Min_value,t,Sum);
    }
    Sum -=Max_value; Sum -=Min_value;
    printf(" Sum = %.2f , ", Sum/16.0);
    Sum>>=4;
    return (Sum);
}
//滑动滤波
int16_t Move_Filter(void)
{
    uint8_t N=16;
    static uint8_t k=0,j=0;
    int32_t Sum=0;
    int16_t data=0;
    
    data= readAddress(AS5047D_CMD_ANGLECOM);      //采样新数据
    if(data<=0 && data >0x3fff)                 //判断数据是否超范围
        return 1;   

    Anglecom[k++] =data;                        //入队列
    if(k >= N){                                 //判断数据是否超过队列长度
        k=0;
    }
    for(j=0;j<16;j++){
        Sum +=Anglecom[j]; 
    }
    Sum>>=4;
    return Sum;
}

//诊断
void diagnostics(char *ptrStr)
{
	int32_t data;
	int m,d;

	if (as5047d)
	{
        data=readAddress(AS5047D_CMD_DIAAGC);

        if (NULL == ptrStr){
            printf("DIAAGC: 0x%04X(%d), ", data,data);
            printf("MAGL: %d, ", getBit(data,11));
            printf("MAGH: %d, ", getBit(data,10));
            printf("COF: %d, ", getBit(data,9));
            printf("LFGL: %d, ", getBit(data,8));
            printf("AGC: %d \r\n", data & 0x0FF);

            data=readAddress(AS5047D_CMD_MAG);
            printf("CMAG: 0x%04X(%d) \r\n",data,data);

            data=readAddress(AS5047D_CMD_ANGLEUNC);
            m=(int)((float)data*AS5047D_DEGREES_PER_BIT);
            d=(int)((float)data*AS5047D_DEGREES_PER_BIT*100 -m*100);
            printf("CORDICANG: 0x%04X(%d) %d.%02d deg(est) \r\n",data,data,m,d);

//            data=readAddress(AS5047D_CMD_ANGLECOM);
            data=Average_Num_Filter();
            m=(int)((float)data*AS5047D_DEGREES_PER_BIT);
            d=(int)((float)data*AS5047D_DEGREES_PER_BIT*100 -m*100);
            printf("DAECANG: 0x%04X(%d) %d.%02d deg(est) \r\n",data,data,m,d);
            printf("                  \r\n");
        }
        else{
            sprintf(ptrStr,"DIAAGC: 0x%04X\n\r", data);
            sprintf(ptrStr,"%sMAGL: %d\n\r", ptrStr,getBit(data,11));
            sprintf(ptrStr,"%sMAGH: %d\n\r", ptrStr,getBit(data,10));
            sprintf(ptrStr,"%sCOF: %d\n\r", ptrStr, getBit(data,9));
            sprintf(ptrStr,"%sLFGL: %d\n\r", ptrStr, getBit(data,8));
            sprintf(ptrStr,"%sAGC: %d\n\r", ptrStr,data & 0x0FF);

            data=readAddress(AS5047D_CMD_MAG);
            sprintf(ptrStr,"%sCMAG: 0x%04X(%d)\n\r", ptrStr,data,data);

            data=readAddress(AS5047D_CMD_ANGLEUNC);
            m=(int)((float)data*AS5047D_DEGREES_PER_BIT);
            d=(int)((float)data*AS5047D_DEGREES_PER_BIT*100 -m*100);
            sprintf(ptrStr,"%sCORDICANG: 0x%04X(%d) %d.%02d deg(est)\n\r", ptrStr,data,data,m,d);

            data=readAddress(AS5047D_CMD_ANGLECOM);
            m=(int)((float)data*AS5047D_DEGREES_PER_BIT);
            d=(int)((float)data*AS5047D_DEGREES_PER_BIT*100 -m*100);
            sprintf(ptrStr,"%sDAECANG: 0x%04X(%d) %d.%02d deg(est)\n\r", ptrStr,data,data,m,d);

        }
	} else{
		data=readAddress(AS5048A_CMD_DIAAGC);
		sprintf(ptrStr,"AS5048A DIAAGC: 0x%04X\n\r", data);
		data=readAddress(AS5048A_CMD_MAG);
		sprintf(ptrStr,"%sMagnitude: %d\n\r", ptrStr,data);
		data=readAddress(AS5048A_CMD_ANGLE);
		sprintf(ptrStr,"%sAngle: %d\n\r", ptrStr,data);
	}

}


