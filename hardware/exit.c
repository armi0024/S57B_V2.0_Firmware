#include "exit.h"


////////////////////////////////////////////////////////////////////////////////// 	  
 
 
//外部中断初始化函数
//外部脉冲方向和使能端口初始化
void EXTIX_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
    #if 1
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;				 //LED0-->PB.10 端口配置
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.10
    
        //GPIOA.1	  中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
    #else
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_11;				 //PB.11 端口配置
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.10
        //GPIOA.1	  中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
    #endif

   	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
}

 //外部中断方向控制
void EXTI1_IRQHandler(void)
{

     if(DIRIN==1){
        if(Motor_Dir==1){
            TIM_CounterModeConfig(TIM2,  TIM_CounterMode_Up);
        }
        else{
            TIM_CounterModeConfig(TIM2,TIM_CounterMode_Down);
        }
    }
	else{
        if(Motor_Dir ==1){
            TIM_CounterModeConfig(TIM2,TIM_CounterMode_Down);
        }
        else{
            TIM_CounterModeConfig(TIM2,TIM_CounterMode_Up);
        }
    }
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI1线路挂起位
}

