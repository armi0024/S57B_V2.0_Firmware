#include "rtc.h"
#include "delay.h"
#include "oled.h"

struct date_typedef  calendar;
u8 time_value[16];

static void RTC_NVIC_Config(void)
{	
    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}


u8 RTC_Init(void)
{
	u8 temp=0;
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
	{	 												//当想从新修改时间时，可以将不等于该为等于，即可修改成功

		//打开APB1总线上的PWR,BKP时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);
		////使能后备寄存器访问 
		PWR_BackupAccessCmd(ENABLE);	//PWR_CR寄存器的DBP=1
		BKP_DeInit();	//复位备份区域 
		//开启外部时钟
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)//&&temp<250	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=250)return 1;//初始化时钟失败,晶振有问题	
		//选择时钟
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		//使能RTC时钟  
		RCC_RTCCLKCmd(ENABLE);
		//等待RTC寄存器同步  
		RTC_WaitForSynchro();
		//	//等待最近一次对RTC寄存器写操作完成
		RTC_WaitForLastTask();	
		//使能RTC秒中断
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//此处还可以添加其他中断
		//等待最近一次对RTC寄存器的写操作完成	
		RTC_WaitForLastTask();
		//设置RTC预分频的值
		RTC_SetPrescaler(32767); //1HZ

		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_Set(2018,04,15,18,50,0);  //设置时间	
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//向指定的后备寄存器中写入用户程序数据
	}
	else
	{
		RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		
	}
		RTC_NVIC_Config();//RCT中断分组设置		    				     
		RTC_Get();//更新时间	
	return 0; //ok

}

//RTC时钟中断
//每秒触发一次  
//extern u16 tcnt; 
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
	{							
		RTC_Get();//更新时间   
 	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟中断	  	   
  	} 				  								 
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//清闹钟中断
	RTC_WaitForLastTask();	  	    						 	   	 
}

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //必须能被4整除
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	

//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//把所有年份的秒钟相加
	{
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;			  //平年的秒钟数
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //把前面月份的秒钟数相加
	{
		seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加 
	seccount+=(u32)hour*3600;//小时秒钟数
    seccount+=(u32)min*60;	 //分钟秒钟数
	seccount+=sec;//最后的秒钟加上去

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	RTC_SetCounter(seccount);	//设置RTC计数器的值

	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
	return 0;	    
}

//得到当前的时间
//返回值:0,成功;其他:错误代码.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=RTC_GetCounter();	 
 	temp=timecount/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp)//超过一天了
	{	  
		daycnt=temp;
		temp1=1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}   
		calendar.w_year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//得到月份
		calendar.w_date=temp+1;  	//得到日期 
	}
	temp=timecount%86400;     		//得到秒钟数   	   
	calendar.hour=temp/3600;     	//小时
	calendar.min=(temp%3600)/60; 	//分钟	
	calendar.sec=(temp%3600)%60; 	//秒钟
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//获取星期   
	return 0;
}

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日 
//返回值：星期号																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  
//日期显示处理函数
void date_process(u16 year,u8 month,u8 day,u8 week,u8 hour,u8 min,u8 sec)
{
	time_value[0]=year/1000;					//年
	time_value[1]=year%1000/100;
	time_value[2]=year%1000%100/10;
	time_value[3]=year%10;
	
	time_value[4]=month/10;						//月
	time_value[5]=month%10;
	
	time_value[6]=day/10;						//日
	time_value[7]=day%10;
	
	time_value[8]=hour/10;						//小时
	time_value[9]=hour%10;
	
	time_value[10]=min/10;						//分钟
	time_value[11]=min%10;	
	
	time_value[12]=sec/10;						//秒钟
	time_value[13]=sec%10;
	//以下是显示年月日
	OLED_ShowChar(40,0,time_value[0]+'0',16,1);//显示ASCII
	OLED_ShowChar(48,0,time_value[1]+'0',16,1);//显示ASCII
	OLED_ShowChar(56,0,time_value[2]+'0',16,1);//显示ASCII
	OLED_ShowChar(64,0,time_value[3]+'0',16,1);//显示ASCII
	
	OLED_ShowChar(80,0,time_value[4]+'0',16,1);//显示ASCII
	OLED_ShowChar(88,0,time_value[5]+'0',16,1);//显示ASCII
	
	OLED_ShowChar(104,0,time_value[6]+'0',16,1);//显示ASCII
	OLED_ShowChar(112,0,time_value[7]+'0',16,1);//显示ASCII
	
	switch(week)								//星期
	{
		case(0):OLED_ShowChar(110,16,'0',16,1);break;//显示ASCII
		case(1):OLED_ShowChar(110,16,'1',16,1);break;//显示ASCII
		case(2):OLED_ShowChar(110,16,'2',16,1);break;//显示ASCII
		case(3):OLED_ShowChar(110,16,'3',16,1);break;//显示ASCII
		case(4):OLED_ShowChar(110,16,'4',16,1);break;//显示ASCII
		case(5):OLED_ShowChar(110,16,'5',16,1);break;//显示ASCII
		case(6):OLED_ShowChar(110,16,'6',16,1);break;//显示ASCII
	}
	//以下是时分秒显示
	OLED_ShowChar(40,16,time_value[8]+'0',16,1);//显示ASCII
	OLED_ShowChar(48,16,time_value[9]+'0',16,1);//显示ASCII
	
	OLED_ShowChar(64,16,time_value[10]+'0',16,1);//显示ASCII
	OLED_ShowChar(72,16,time_value[11]+'0',16,1);//显示ASCII
	
	OLED_ShowChar(88,16,time_value[12]+'0',16,1);//显示ASCII
	OLED_ShowChar(96,16,time_value[13]+'0',16,1);//显示ASCII
	
}
//日期显示函数
void Date_display(void)
{
	date_process(calendar.w_year,calendar.w_month,calendar.w_date,calendar.week,calendar.hour,calendar.min,calendar.sec);		
}

