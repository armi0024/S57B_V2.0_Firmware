/**
  ******************************************************************************
  * @file    uart.c
  * @author  dy_stm32
  * @version V1.0.0
  * @date    2016.8.1
  * @brief   
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define H_ENABLE_STATUS   "Enable =%d \r\n"
#define L_ENABLE_STATUS   "!Enable =%d \r\n"
#define CCW_DIR           "Dir =%d, CCW\r\n"
#define CW_DIR            "Dir =%d, CW\r\n"
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    USART1->DR = (u8) ch;
	while(USART_GetFlagStatus (USART1 , USART_FLAG_TC)== RESET);//循环发送,直到发送完毕   
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
//串口初始化函数
#if 0
void usart_init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
 	USART1->BRR=mantissa; // 波特率设置	
	
//	USART1->CR1|=1<<13;  //1位停止,无校验位
//	USART1->CR1&=~(1<<12);//一个起始位，8个数据位
//	USART1->CR2&=~(3<<12);//设置停止位为1个
//	USART1->CR1|=(1<<3);//发送使能
//	USART1->CR1|=(1<<2);//接收使能
	USART1->CR1|=0X200C;  
	
	USART1->CR1|=1<<8;		 //PE中断使能
	USART1->CR1|=1<<5;   //接收缓冲区非空中断使能	 
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
}

#endif
/************发送一个字节*********************************/
void USART1_SendByte(char data)
{
    USART1->DR = (uint8_t)data;
    /* 等待传输结束 */
    while(USART_GetFlagStatus (USART1 , USART_FLAG_TC)== RESET);//循环发送,直到发送完毕 
}


/*******************************************************************************
* 名称: UART1_SendStr
* 功能: UART1发送len个字符
* 形参: data -> 指向要发送的字符串
*       len -> 要发送的字节数
* 返回: 无
* 说明: 无 
******************************************************************************/
void USART1_SendStr(char *str)
{
    while(*str != '\0')
    {
        USART1_SendByte(*str++); /* 循环调用发送一个字符函数 */
    }   
    //*str =NULL ;
}
/**
  * @brief  System_NVIC_Priority_Configurate
  * @param  chx nvic irq Channel
  * @param  pre PreemptionPriority
  * @param  sub SubPriority
  * @retval None
  */
void System_NVIC_Priority_Configurate(uint8_t chx, uint8_t pre, uint8_t sub)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = chx;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pre;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief 	UART_BaudRate_Settings
  * @param  USARTx, baudrate
  * @retval None
  */
void UART_BaudRate_Settings(USART_TypeDef * USARTx, uint32_t baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_DeInit(USARTx);
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);
}

/**
  * @brief 	UART_Configuration
  * @param  USARTx, baudrate
  * @retval None
  */
void UART_Configuration(USART_TypeDef * USARTx, uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if (USARTx == USART1) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);
    
		GPIO_InitStructure.GPIO_Pin = UART1_TX_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(UART1_TX_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = UART1_RX_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(UART1_RX_GPIO_PORT, &GPIO_InitStructure);

		UART_BaudRate_Settings(USART1, baudrate);
        System_NVIC_Priority_Configurate(USART1_IRQn, 1, 3);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
		USART_Cmd(USART1, ENABLE);
        
		USART_ClearFlag(USART1, USART_FLAG_TC);
	}
}

void PID_Cal_value_init(void)
{
//    enmode =0;
 /*从新初始化数据，防止调细分后突然位移*/
    s=0;
    s_1=0;
    
    s_sum=0;
    r=0;
    r_1=0;
    y=0;
    y_1=0;
    yw=0;
    e=0;
    u=0;
    dterm=0;
    wrap_count=0;
    //LL_TIM_SetCounter(TIM1,0);
    
    
    r=*(volatile uint16_t*)((ReadValue(READ_ANGLE_VALUE)>>1)*2+SAMPLING_DATA_ADDR); //指令电机位置
    s_sum=r;   //外部控制脉冲输入累计值
    y=r;
    y_1=y;
    yw=y;  
    yw_1=yw;
    
    WRITE_REG(TIM2->CNT, 0);
    
}

void test_uart(void)
{
    if(info_report_flag ==1){
        info_report_flag =0;
    
        switch(Rx1_buff[3]){         //解析指令码
            case 0xA0:
                    kp = value_Temp;
                    table1[11]=kp;
                    
                    printf("new kp = %d\r\n",kp);
            break;
            case 0xA1:
                    ki = value_Temp;
                    table1[12]=ki;
                    
                    printf("new ki = %d\r\n",ki);
                break;
            case 0xA2: kd = value_Temp;
                    table1[13]=kd;
                    
                    printf("new kd = %d\r\n",kd);
                break;
            case 0xA3:      //set Current 
                    Currents = (value_Temp/100)<<3;
                    Menu_Num2_item = Currents>>3;
            
                    table1[1]=Currents;
                    table1[2]=Menu_Num2_item;
                    
                break;                                      //功能命令自行扩展
            case 0xA4:
                    enmode =0;                      //关闭使能，等参数设置完成再使能
                    PID_Cal_value_init();                   /*从新初始化数据，防止调细分后突然位移*/
                    stepangle = 64/value_Temp;
                    //stepangle =Microstep_Set;              //设置电子齿轮
                    table1[3]=stepangle;                    //数组3、4号元素为电子齿轮的设置
                    switch(stepangle & 0x3e){
                        case 0x02:Menu_Num3_item =4;  break;
                        case 0x04:Menu_Num3_item =3;  break;
                        case 0x08:Menu_Num3_item =2;  break;
                        case 0x10:Menu_Num3_item =1;  break;
                        case 0x20:Menu_Num3_item =0;  break;
                        default: Menu_Num3_item =3; break;
                    }
                    table1[4]=Menu_Num3_item;
                     
                break;
            case 0xA5:
                    enmode =0;                      //关闭使能，等参数设置完成再使能
                    PID_Cal_value_init();                               //重新设置初始值
                    if(0xaa == value_Temp){
                        Motor_ENmode_flag=0;Menu_Num4_item=1;           //低电平使能
                    }else if(0x55 == value_Temp) {
                        Motor_ENmode_flag=1;Menu_Num4_item=0;
                    }
                    table1[5]=Motor_ENmode_flag;                        //数组5、6号元素为使能信号的设置
                    table1[6]=Menu_Num4_item;
                    
                break; 
            case 0xA6:                                                  //设置电机方向
                    if(0x11 == value_Temp){
                        Motor_Dir=1;Menu_Num5_item=0;                   //逆时针
                    }else if(0x22 == value_Temp) {
                        Motor_Dir=0;Menu_Num5_item=1;
                    }
                    table1[7]=Motor_Dir;                                //数组7、8号元素为电方向的设置
                    table1[8]=Menu_Num5_item;
                   
                break; 
            case 0xB0: if((Rx1_buff[4]==0xaa) && (Rx1_buff[5]== 0xaa)){                     //读PID参数
                   
                    printf("p --- kp =%d\r\n",kp);
                    printf("i --- ki =%d\r\n",ki);
                    printf("d --- kd =%d\r\n",kd);
                    }else{
                        USART1_SendStr("Read PID err\r\n");
                    }
                break;
            case 0xB1: if((Rx1_buff[4]==0xaa) && (Rx1_buff[5]== 0xaa)){                     //读电流参数
                 
                        printf("Currents =%d mA\r\n",(Currents>>3)*100);
                    }else{
                        USART1_SendStr("Read Current err\r\n");
                    }
                break;
            case 0xB2: if((Rx1_buff[4]==0xaa) && (Rx1_buff[5]== 0xaa)){                     //读细分状态
                 
                        printf("Microstep =%d -->%d Gear\r\n",64/stepangle,16384/stepangle);
                    }else{
                        USART1_SendStr("Read Microstep err\r\n");
                    }
                break;
            case 0xB3: if((Rx1_buff[4]==0xaa) && (Rx1_buff[5]== 0xaa)){                     //读使能信号状态
                
                        if(1 == Motor_ENmode_flag)
                            printf(H_ENABLE_STATUS,Motor_ENmode_flag);
                        if (0== Motor_ENmode_flag)
                            printf(L_ENABLE_STATUS,Motor_ENmode_flag);
                    }else{
                        USART1_SendStr("Read enable err\r\n");
                    }
                break;                           
            case 0xB4: if((Rx1_buff[4]==0xaa) && (Rx1_buff[5]== 0xaa)){                     //读方向信号状态
                   
                        if(0 == Motor_Dir)
                            printf(CCW_DIR,Motor_Dir);
                        if(1 == Motor_Dir)
                            printf(CW_DIR,Motor_Dir);
                    }else{
                        USART1_SendStr("Read Dir err\r\n");
                    }
                break;
            default: USART1_SendStr("Function Code Undefined\r\n");
                break;
        }
    }
}
void data_Process(void)
{
   if(Urat_CRC_Correct_flag){               //检验正确标志
        Urat_CRC_Correct_flag=0;
        if(Rx1_buff[2] ==0x05){                 //数据长度正确
            value_Temp = Rx1_buff[4];
            value_Temp<<=8;
            value_Temp|= Rx1_buff[5];
            if(Rx1_buff[3]>=0xA0 && Rx1_buff[3]<=0xBF){
                if((Rx1_buff[3] &0xA0)== 0xA0)
                    flash_store_flag=1;                     //若是修改参数就开启储存标志
                else
                    flash_store_flag=0;
                info_report_flag =1;
                //test_uart();
            }else{
                USART1_SendStr("Function Code err\r\n");
            }
        }
        else{
            ////错误待处理,返回错误数据
            USART1_SendStr("Data Length err\r\n");
        }
        //USART_Cmd(USART1, ENABLE);
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    }
}
//串口数据接收处理
void usart_Receive_Process(void)
{
    static uint8_t t;
//    uint32_t Temporary_val=0;
    uint16_t CRC_val=0;
    if(Communications_Process_flag){
        Communications_Process_flag=0;
        if(Uart_CRC_flag ){                                   //开始校验   
            Uart_CRC_flag=0;
            for(t=2;t<Rx1_temp_num-2;t++){//8-2
                CRC_val+=Rx1_buff[t];
                if(t==Rx1_temp_num-3) CRC_val=CRC_val & 0xff;//数据校验计算
            } 
            if(Rx1_buff[t]==CRC_val){
                Urat_CRC_Correct_flag=1;                    //校验正确标志开
                //返回正确数据信号 OK
                //USART1_SendStr("OK\r\n");
                printf("OK\r\n");
            }
            else{
//                Uart_CRC_Error_flag=0;                      //校验错误标志置位
                //错误待处理,返回错误数据
                printf("CRC verify err\r\n");
                //USART1_SendStr("CRC verify err\r\n");
                
                //USART_Cmd(USART1, ENABLE);
                USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
                USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);   
            }
        }
        else{
            //USART_Cmd(USART1, ENABLE);
            USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
            USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
        }
    }
}



#ifdef USART1_FIFO  

/**
  * @brief  USART1_IRQHandler
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    uint8_t _data;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据 //LL_USART_IsActiveFlag_IDLE
    {
        Rx1_buff[Receive_Count++]= USART1->DR;            //接收数据的同时会清除中断标志
    }
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) == 1){
        
        _data = USART1->SR;                             //清除空闲中断标志
        _data = USART1->DR;
                       
        //Receive_finish_flag=1;
        if((Rx1_buff[0]==0xfe)&&(Rx1_buff[1]==0xfe)&&(Rx1_buff[Receive_Count-1]==0x16))//判断是否为帧头和帧尾                                
        {
            //USART_Cmd(USART1, DISABLE);
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
            USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);

            //Rx_Datanum==Rx_Databuff[13]+16;            //接收数据是否等于数据计算个数
            Rx1_temp_num=Receive_Count;                  
            Receive_Count=0;
//            Communications_flag=1;                      //数据传输完成
            Communications_Process_flag=1;              //通信处理标志
            Uart_CRC_flag=1;                            //数据校验标志开
            
//            value_Temp = (int16_t)Rx1_buff[4];
//            value_Temp<<=8;
//            value_Temp|=(int16_t) Rx1_buff[5];
//            if((Rx1_buff[3] &0xA0)== 0xA0)
//                    flash_store_flag=1;                     //若是修改参数就开启储存标志
//                else
//                    flash_store_flag=0;
//                info_report_flag =1;
        }
        else{
            Receive_Count=0;
            //帧错误
             frame_Error_flag=1;   
        }  
    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);     //清除接受中断标志
}
#endif


/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/


////功能:发送函数
////从下位机发送到上位机
//void usart_putchar(u8 ch)
//{
//	while((USART1->SR &0X40)==0);//判禪SART1->SR寄存器的第六位是否接受
//	USART1->DR=ch;
//}

////功能:上位机发送数据给单片机
////开启串口中断接收数据
//void USART1_IRQHandler(void)
//{
//	u8 res;	
//	if(USART1->SR&(1<<5))//接收到数据
//	{
//		res=USART1->DR; 
//		USART1->DR=res;// 将接收到的数据返回给串口助手
//		
//		GPIOA->ODR=res;
//		delay_ms(1);
////////		if((USART_RX_STA&0x8000)==0)//接收未完成
////////		{
////////			if(USART_RX_STA&0x4000)//接收到了0x0d
////////			{
////////				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
////////				else USART_RX_STA|=0x8000;	//接收完成了 
////////			}else //还没收到0X0D
////////			{	
////////				if(res==0x0d)USART_RX_STA|=0x4000;
////////				else
////////				{
////////					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
////////					USART_RX_STA++;
////////					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
////////				}		 
////////			}
////////		}  		 									     
//	}
//}

