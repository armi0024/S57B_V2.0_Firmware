#include "time.h"
#include "gpio.h"
#include "iwdg.h"
#include "flash.h"

//定时器2 ETR及外部脉冲

void TIM2_Cap_Init(u16 arr,u16 psc)
{	 
    TIM_DeInit(TIM2);
    
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    TIM_ICInitTypeDef  TIM2_ICInitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;
    
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  //PA0 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA0 输入 
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
    TIM_ARRPreloadConfig(TIM2,DISABLE);              //关闭预装载
//    TIM_ETRClockMode1Config(TIM2, TIM_ExtTRGPSC_OFF,  TIM_ExtTRGPolarity_NonInverted,0);
    TIM2->SMCR |= 1<<14;                        //选用外部时钟模式2 ece
//    TIM2->SMCR &= ~(1<<15);                     //外部时钟极性不反转 etp
//    TIM2->SMCR &= ~(3<<12);                     //外部触发预分频不分频 etps
//    TIM2->SMCR &= ~(0xF<<8);
    
    TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 7);//7,9
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Reset);
    TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Disable);              //关闭主从模式MSM主从模式位
    
//	//初始化TIM2输入捕获参数
//	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
//  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
//  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
//  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
//  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
//  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
#if 0
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
#endif
    TIM_SetCounter(TIM2 , 0);
    TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
}


//u8  TIM2CH1_CAPTURE_STA=0;	//输入捕获状态		    				
//u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值
// 
//定时器2中断服务程序	 
//void TIM2_IRQHandler(void)
//{ 
//    static uint16_t overflow=0;
//// 	if((TIM2CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
////	{	  
////		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
////		{	    
////            led2=!led2;
//            Receiv_pluse = TIM2->CNT;
//            overflow++ ;                      
//            if(overflow>=5){                     //估计发送脉冲间隔为1ms
//                
//                overflow=0;
//            }
//            printf("overflow = %d , Receiv_pluse = %d\r\n",overflow,Receiv_pluse);
////			if(TIM2CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
////			{
////				if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
////				{
////					TIM2CH1_CAPTURE_STA|=0X80;//标记成功捕获了一次
////					TIM2CH1_CAPTURE_VAL=0XFFFF;
////				}else TIM2CH1_CAPTURE_STA++;
////			}	 
////		}
////	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//捕获1发生捕获事件
////		{	
////			if(TIM2CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
////			{	  			
////				TIM2CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次上升沿
////				TIM2CH1_CAPTURE_VAL=TIM_GetCapture1(TIM2);
////		   		TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
////			}else  								//还未开始,第一次捕获上升沿
////			{
////				TIM2CH1_CAPTURE_STA=0;			//清空
////				TIM2CH1_CAPTURE_VAL=0;
////	 			TIM_SetCounter(TIM2,0);
////				TIM2CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
////		   		TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
////			}		    
////		}			     	    					   
//// 	}
//    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位
//}



////定时器1
//void TIM1_Init(uint16_t Arr,uint16_t Psc)
//{
////    GPIO_InitTypeDef            GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
////	TIM_OCInitTypeDef           TIM_OCInitStructure;
//	NVIC_InitTypeDef            NVIC_InitStructure;

//	//使能定时器 1  AFIO复用功能模块   GPIOA端口时钟
////    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

////   //输出TIM8 CH4的PWM脉冲波形	GPIOB.1	 输出TIM8 CH2的PWM脉冲波形	GPIOB.0
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
////    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
////	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
//	
//   //初始化TIM1
//    TIM_DeInit(TIM1);
//	TIM_TimeBaseStructure.TIM_Period = Arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//	TIM_TimeBaseStructure.TIM_Prescaler =Psc; //设置用来作为TIMx时钟频率除数的预分频值  72000/7200=10K
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

//	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;//重复寄存器，用于自动更新
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

//	
////    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2; //设置为pwm2输出模式
////    TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High; //设置输出极性
////    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //禁止该通道输出  
////	 //下面几个参数是高级定时器才会用到，通用定时器不用配置
////    TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High; //设置互补端输出极性
////    TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;//使能互补端输出
////    TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset; //死区后输出状态
////    TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;//死区后互补端输出状态
////	// TIM_OC1Init(TIM8,&TIM_OCInitStructure); //按照指定参数初始化
//    
////    TIM_SelectMasterSlaveMode(TIM1 ,TIM_MasterSlaveMode_Enable);
////    TIM_SelectOutputTrigger(TIM1 ,TIM_TRGOSource_Update);

////    TIM_OC4Init(TIM1, &TIM_OCInitStructure);    
////    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
//        
////    TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM8 OC2
////    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM8在CCR2上的预装载寄存器

//    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    
////    TIM_CtrlPWMOutputs(TIM1, ENABLE); //pwm输出使能，一定要记得打开
////    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
//    TIM_ITConfig(TIM1,  TIM_IT_Update, ENABLE);
//}

void SetT1Pwm1(uint16_t Duty)
{
    TIM1 ->CCR4 = Duty; 
}
//定时器3 CH1、CH2 重映射到PB4\PB5 输出 PWM
void TIM3_Init(void)
{
    
   	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	GPIO_InitTypeDef  GPIO_InitStruct;
//    GPIO_AFIODeInit();
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//使能PB端口时钟 使能复用IO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);   //使能定时器3的时钟
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭JTAG功能
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);    //此管脚只能使用部分从映射，PB4、PB5（重点）
    
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;			
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	

    
   	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1; //预分频值，10us计数一次
	TIM_TimeBaseInitStruct.TIM_Period = 256;		//重装值，决定PWM周期
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);

//	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);//TIM3为门控模式	
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Disable);//使能TIM3的主从模式	
//	TIM_SelectInputTrigger(TIM3,TIM_TS_ITR1);//内部触发，TIM_TS_ITR1对应TIM2

    TIM_ARRPreloadConfig(TIM3,DISABLE);              //关闭预装载
    
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//在向上计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
	TIM_OCInitStruct.TIM_Pulse = 3;//比较捕获1的预装载值，占空比为50%
    
	TIM_OC1Init(TIM3,&TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能定时器3的通道1预装载寄存器

	TIM_OC2Init(TIM3,&TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能定时器3的通道1预装载寄存器
    
//    TIM_ARRPreloadConfig(TIM3,ENABLE);              //使能预装载
    TIM_CtrlPWMOutputs(TIM3, ENABLE);               //pwm输出使能，一定要记得打开
	TIM_Cmd(TIM3,ENABLE);                           //使能TIM3
}
//定时器4初始化用于位置环更新
void TIM4_Init(u16 arr,u16 psc)
{	 
//	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM4时钟
		
	//初始化定时器4 TIM4	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
     
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
//	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
    TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
}
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){	    
//            led1=!led1;
        IWDG_Feed();//清看门狗
        if(enmode==1)
        {
          //SET_BIT(TIM3->CR1, TIM_CR1_CEN);
          if(closemode==1) 
          {    
            y=*(volatile uint16_t*)((ReadValue(READ_ANGLE_VALUE)>>1)*2+SAMPLING_DATA_ADDR);//读出编码器的角度位置值
            s=TIM_GetCounter(TIM2);//读出计数器计数的外部step控制脉冲数
            if(s-s_1<-32768)
              s_sum+=stepangle*65536;
            else if(s-s_1>32768)
              s_sum-=stepangle*65536;
            r=s_sum+stepangle*s;//溢出处理后根据电子齿轮算出指令电机位置
            s_1=s;
            
            if(y-y_1>8192) {
              wrap_count--;
                start_measu_V_flag=1;
            }                
            else if(y-y_1<-8192) {
              wrap_count++; 
              start_measu_V_flag=1;  
            }
            else measure_once_flag =0;
            yw=y+16384*wrap_count;//编码器的角度位置值溢出处理后根据圈数算出实际电机位置值            
            e=r-yw;//误差值
            if(e>1638)//误差值大小限制，误差范围内指示灯灭，否则亮起
            {
              e=1638;
              led1 = LED_ON;            
            }
            else if(e<-1638){
              e=-1638;
              led1 = LED_ON;  
            }else {
                led1 = LED_OFF;
            }
            iterm+=ki*e/32;//积分项计算
            #if 1
                if(iterm>UMAXSUM)//积分饱和限制
                  iterm=UMAXSUM;
                else if(iterm<-UMAXSUM) 
                  iterm=-UMAXSUM; 
            #else 
                if(iterm>(Currents*128))//积分饱和限制
                  iterm = Currents*128;
                else if(iterm<-(Currents*128)) 
                  iterm = -(Currents*128); 
            #endif
               
            dterm=LPFA*dterm/128-LPFB*kd*(yw-yw_1)/8;//微分项计算
            u=(kp*e+iterm+dterm)/128;//PID三项计算值
            
//            advance=(yw-yw_1)*1.5f;//前馈角计算，1.5改为2可以提高些转速，如果电机容易跑丢改为1.2,还可以再高些，但需要考考实际编码器响应速度
            advance=(yw-yw_1)*3;
            y_1=y;  
            yw_1=yw;
        
            if(u>0){
              if(advance>68)//前馈角限制，最大不能超过1.8度即82编码器值
                advance=68;
              else if(advance<0)
                advance=0; 		  
              y+=(82+advance);//电流矢量等于1.8度加上前馈角
            }
            else if(u<0){
              if(advance<-68)
                advance=-68; 
              else if(advance>0)
                advance=0; 
              y-=(82-advance);
              u=-u;
            }
            
            #if 1
                if(u>Currents){
                    u=Currents;//电流矢量最大值限制
    //                led1 = LED_ON;
                }
            #else 
                if(u>UMAXCL)     
                {
                  u=UMAXCL;//电流矢量最大值限制
                  LED_H;
                }
            #endif
//            else
//              led1 = LED_OFF;
            Output(y,u);    
          }          
          else{		
            s=TIM_GetCounter(TIM2);
            if(s-s_1<-32768)
              s_sum+=stepangle*65536;
            else if(s-s_1>32768)
              s_sum-=stepangle*65536;
            r=s_sum+stepangle*s;
            s_1=s;
            
            if(r==r_1){
              hccount++;
              if(hccount>=1000)
                hccount=1000;
            }
            else
              hccount=0;
            
            if(hccount>=1000)//0.1s自动进入半流模式
              Output(r,UMAXOP/2);
            else
              Output(r,UMAXOP);
            r_1=r;
        }
            Motor_speed_count++;
            if(start_measu_V_flag==1){
                Data_update_Count++;
                if(Data_update_Count>=10000){       //250ms更新一次数据
                    Data_update_Count=0;
                    wap1=wap2;
                    wap2=wrap_count;
                    start_measu_V_flag=0;           //关闭转速测量标志
                    measure_once_flag =1;           //一次测量完成标志
                    
    //              Motor_speed = wap2-wap1;//计算1秒钟所转的圈速
                }
            }
            if(Motor_speed_count>=2000){
                Motor_speed_count-=2000;
                Data_update_flag=1;           //电机参数实时显示标志
                
                if(measure_once_flag == 0)    //  
                    start_measu_V_flag=1;
                if(wap1== wap2) start_measu_V_flag=0; //如果前后两次采集的圈数相等说明电机没转动。则关闭启动测转速标志
            }
        }
    }
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //清除中断标志位
}


//void  TIM1_UP_IRQHandler(void)
//{
//    if(TIM_GetITStatus(TIM1,TIM_IT_Update)!= RESET){
//        TIM_ClearITPendingBit(TIM1 ,TIM_IT_Update);
//        TIM_Cmd(TIM1, DISABLE);                     //关闭TIM1
//        TIM1->CNT = 0;                              //清除计数器值
//    }
//}
//void TIM_Init(void)
//{	
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
//	TIM_OCInitTypeDef TIM_OCInitStruct;
//	GPIO_InitTypeDef  GPIO_InitStruct;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PC,PA端口时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);   //使能定时器23的时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);						  //使能复用IO时钟	

////	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);	

//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;			
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);	
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
//	GPIO_Init(GPIOA,&GPIO_InitStruct); 

//	//TIM2工作在单脉冲下
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
//	TIM_TimeBaseInitStruct.TIM_Prescaler = 7199;				//预分频值,每100us计数一次
//	TIM_TimeBaseInitStruct.TIM_Period = 99; 					//重装值，决定单脉冲周期
//	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);	

//	TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);		 //设置TIM2在单脉冲模式，且是单一的脉冲，在下一个更新事件停止	  
//	TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC1Ref); //OC1REF signal is used as the trigger output (TRGO).单一模式不需要设置

//	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);	 //使能定时器2的通道1预装载寄存器
//	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;		 //在向上计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平
//	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//OC1输出使能
//	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;		//有效电平为高
//	TIM_OCInitStruct.TIM_Pulse = 20;		//比较捕获1的预装载值，决定单脉冲占空比，这个20就是低电平延续的来源
//	TIM_OC1Init(TIM2,&TIM_OCInitStruct);		
//	TIM_Cmd(TIM2,DISABLE); //Disable the TIM Counter

//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
//	TIM_TimeBaseInitStruct.TIM_Prescaler = 719; //预分频值，10us计数一次
//	TIM_TimeBaseInitStruct.TIM_Period = 29;		//重装值，决定PWM周期
//	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);

//	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);//TIM3为门控模式	
//	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);//使能TIM3的主从模式	
//	TIM_SelectInputTrigger(TIM3,TIM_TS_ITR1);//内部触发，TIM_TS_ITR1对应TIM2

//	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能定时器3的通道1预装载寄存器
//	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平
//	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
//	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
//	TIM_OCInitStruct.TIM_Pulse = 15;//比较捕获1的预装载值，占空比为50%
//	TIM_OC1Init(TIM3,&TIM_OCInitStruct);
//	TIM_Cmd(TIM3,ENABLE);//使能TIM3
//}

/*********************************************************
名称：Pulse_Control(u16 cycle, u16 pulse_num)
说明：生成指定个数脉冲，每个脉冲周期为cycle微秒，脉冲个数生成的个数
      和单脉冲高电平时间有关系，脉冲个数就由高电平时间来确定
参数cycle：为TIM3一个脉冲周期,单位(us)
参数pulse_num：为脉冲个数，决定步进电机步数
要求：cycle * pulse_num < 6551600
返回值：无
*********************************************************/
void Pulse_Control(u16 cycle, u16 pulse_num)
{
	u16 arr3 = 0;
	u32 time = 0;
	if(pulse_num)
	{ 
		time = cycle * pulse_num / 100;//预分频为720,100us计数一次*//
		arr3 = cycle / 10;             //++++,10us计数一次 //50
		TIM_SetAutoreload(TIM2, time + 19);//低电平周期加高电平周期
		TIM_SetAutoreload(TIM3, arr3 - 1);
		TIM_SetCompare1(TIM3,arr3 / 2); //设置PWM占空比为50%//25
		TIM_Cmd(TIM2,ENABLE);//使能TIM2
       
	}
}
