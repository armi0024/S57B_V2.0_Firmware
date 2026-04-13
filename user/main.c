/**
  ******************************************************************************
  * @file    main.c
  * @author  Vsion yang
  * @version V1.0.0
  * @date    2019.8.5
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
 //带CAN驱动
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "mydef.h"
#include "gpio.h"
#include "usart.h"
#include "oled.h"
//#include "spi.h"
#include "spi1.h"
#include "time.h"
#include "exit.h"
#include "key.h"
#include "can.h"
#include "display.h"
#include "flash.h"
#include "tle5012.h"
#include "iwdg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ENCLOSE_LOOP            //使能闭环
#define ENCOLSE_LOOP_CONFIG     //使能闭环配置
//#define ENFLASH_TEST            //使能flash 测试
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//PID参数变量
int16_t kp=30;     
int16_t ki=10;  
int16_t kd=250; 


#ifdef  NEW_config
    //低通滤波参数变量
    const uint8_t LPFA=100; 
    const uint8_t LPFB=28;
#else
    //低通滤波参数变量
    const uint8_t LPFA=125; 
    const uint8_t LPFB=3;
#endif
int32_t s=0;//外部控制脉冲输入定时器的值
int32_t s_1=0;
int32_t s_sum=0;//外部控制脉冲输入累计值
int32_t r=0;//指令电机位置
int32_t r_1=0;
uint8_t dir=1;//电机旋转方向
int16_t y=0;//编码器的读数值
int16_t y_1=0;
int32_t yw=0;//实际电机位置值
int32_t yw_1=0;
int16_t advance=0;//电机控制提前角度
int32_t wrap_count=0;//电机旋转圈数
int32_t e=0;//误差值等于指令电机位置值减去实际电机位置值
int32_t iterm=0;//PID控制i积分项
int32_t dterm=0;//PID控制d微分项
int16_t u=0;//矢量控制电流大小
int32_t stepnumber=0;//电机走的整步步数
uint8_t stepangle=0;//电子齿轮值

uint16_t hccount=0;//开环模式下半流控制计数值
uint8_t closemode;//开环闭环模式标志位
uint8_t enmode=0;//使能标志位

uint16_t Calibration_flag=0;      //自校准标志
uint8_t Menu_Num =0;             //菜单项
uint8_t Menu_Num_item=0;         //菜单栏子项
uint8_t Menu_move_num=0;         //向下选择标记
volatile uint8_t Menu_update_flag=1;      //界面更新标志
volatile uint8_t Data_update_flag =1; //电机实时数据更新标志
volatile uint16_t Data_update_Count =0; //25ms更新一次

uint8_t CalibrateEncoder_finish_flag=0; //校准完成标志0：没完成；1：完成
uint8_t Second_Calibrate_flag=0;        //校准完成标志0：没完成；1：完成
uint8_t Second_Menu=0;                  //二级子菜单项
uint8_t Menu_Num2_item=0;               //电流选择项  
uint8_t Menu_Num3_item=0;               //电子齿轮（细分）选择项  
uint8_t Menu_Num4_item=0;               //使能选择项  
uint8_t Menu_Num5_item=0;               //方向选择项 
uint8_t Menu_Num6_item=0;               //电机工作模式选择 

int16_t Motor_speed =0;
int16_t wap1=0;
int16_t wap2=0;
int16_t Motor_speed_count=0; 
uint8_t start_measu_V_flag=0;
uint8_t measure_once_flag=0;

uint8_t Currents=0;                 //默认800ma
uint8_t Currents_Set=0;
uint8_t Microstep_Set=0;            //默认4->4096电子齿
uint8_t Microstep=0;                //默认4->4096电子齿
uint8_t Dir_Enable=0x00;           //默认方向是位0，默认使能位 是位1;
                                    //!ENABLE = 电机低电平锁定，高电平解锁
                                    //ENABLE = 电机高电平锁定，低电平解锁
uint8_t Motor_Dir=0;               //方向

uint8_t Motor_Enable=0;            //使能
uint8_t Motor_mode=0;              //电机工作模式选择 0：闭环，1开环
volatile uint8_t Motor_ENmode_flag=0; //电机使能模式标志,默认低锁住，高解锁
volatile uint8_t enter1_once_flag =1;    //使能检测标志
volatile uint8_t enter2_once_flag =0;

volatile uint8_t dir1_once_flag =1;      //方向检测标志
volatile uint8_t dir2_once_flag =0;
volatile uint8_t close_loop1_flag =1;      //闭环模式选择标志
volatile uint8_t close_loop2_flag =0;

volatile uint8_t flash_store_flag=0;    //flash存储标志
uint16_t table1[14];                    //数据存储缓存区
volatile uint8_t Reset_status_flag=0;                    //复位状态标志

uint8_t Rx1_buff[9];
uint8_t Receive_Count=0;                //接收数据计数    
uint8_t Rx1_temp_num=0;                 //串口临时数据计数
uint8_t Receive_finish_flag=0;          //接收完成标志

volatile uint8_t  Communications_Process_flag=0;     //通信数据处理
volatile uint8_t  Uart_CRC_flag=0;                   //串口数据校验标志  
volatile uint8_t  frame_Error_flag=0;                //串口数据帧校验错误标志
volatile uint8_t  Urat_CRC_Correct_flag=0;           //串口数据校验正确标志
volatile uint8_t  info_report_flag=0;
//uint8_t Receive_statu=0x00;
volatile int16_t value_Temp=0;
uint8_t Receiv_pluse =0;

/* Private function prototypes -----------------------------------------------*/
uint16_t table[]={'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','0'};

/* Private functions ---------------------------------------------------------*/
void flash_test(void);
void restart_init(void);
//OLED驱动OK
//2019-10-21 新板界面
//2019-10-22 新增按键菜单功能
//2019-10-23 增加加各个参数保存flash,便于二次加载 电流和细分 方向 PID参数
//2019-10-28 电机实时转速，误差、数据显示
//2019-10-29 串口修改PID参数，遵循自定义协议
//2019-11-02 修复bug,计入看门狗不能校准的现象
//2019-11-04 新增串口可以读取设置的电流 和 微步（电子齿距）
//2019-11-07 增加使能信号设置数据存储，使能检测改为在主函数扫描。
//2019-11-11 修复bug,在进行微步切换和使能模式切换设置时,出现电机移动一大截的现象
//2019-11-15 限定uart通信时功能码数据范围0xA0 - 0xBF
//2019-11-18 修改为电机参数显示在电机使能时显示刷新，初始化采样并显示一次电机角度
//2019-11-19 优化校准完成后出现显示异常的现象
//2020-01-03 增加不带OLEd屏，通过串口调电流，微步，方向，使能参数
//2020-01-16 修复bug,在进行微步切换或使能模式或主板使能与非使能信号切换设置时,出现电机移动一大截的现象
//2020-04-20 增加CAN Bus驱动,移植到STM32f103c8xx上
//2020-04-29 移植到stm32F103 OK 
//功能：闭环驱动模组，首次校准带记忆存储，校准需要根据提示复位重启 
//2020-07-11 优化电机角度数据显示
//2020-07-24 SKR V1.3 主板+S42B ,主板发送移动指令，电机不转的现象，是由于采样频率偏小，修改定时器2，ETR的采样频率即可fclk/4 ->8
/**
  * @brief  main program
  * @param  None
  * @retval None
  */
int main(void)
{
	static uint8_t res = 0;
//    static uint8_t key=0,i=1;
	u16 temp;
    uint8_t canbufTx[]="12345678";
//    uint8_t canbufRx[8]={0};
    
//    uint8_t k3_flag = 0,;
//    uint8_t k4_flag = 0;
    uint8_t mode=1;//CAN工作模式;0,普通模式;1,环回模式
    
    System_Clock_Init();            //系统时钟配置
    delay_init(72);					//延时初始化 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
	LED_Init();					                    //LED初始化
	OLED_Init();					                //OLED初始化
    OLED_ShowString(0,0,"Oled Init...OK");
    delay_ms(100);   
    Key_init();                                     //按键初始化
    
#ifdef ENFLASH_TEST    
    flash_test();                            //flash 存储，测试用 
#endif
//    SetModeCheck();                          //执行校准功能，测试用
//       while(1);
#ifdef ENCOLSE_LOOP_CONFIG
    OLED_Clear();                             //清屏
    OLED_ShowString(0,0,"Close Loop Model");

    tle5012b_init();            //磁编码芯片初始化
    Motor_init();               //电机IO初始化
    TIM3_Init();                            //PWM 输出配置 
    
    FLASH_Unlock();
    Calibration_flag = FlashReadHalfWord(Data_Store_Arrdess);
    FLASH_Lock();
    if((Calibration_flag>>8) == 0xAA){
        OLED_ShowString(0,25,"  Calibration  ");
        OLED_ShowString(40,45,"  OK!");
        delay_ms(500);
        //进入主显示界面显示
        OLED_Clear();   //校准完成后清屏进入校准后的主界面
        OLED_ShowString(0,2,"Simp:  0000 RPM");//主显示界面
        OLED_ShowString(0,22," Err:  000.00 ");
        OLED_ShowString(0,42," Deg:  0000.0");
        Menu_Num_item=2;                                        //如果已校准，初始菜单项变量，直接进入功能设置界面
        Second_Menu=1;                                          //二级菜单1
        Menu_update_flag=0;                                     //已校准就进入主显示界面
        Menu_Num=1;                                             //标记显示界面   
        Reset_status_flag=1;                                    //标记复位状态完成
//        Menu_Num2_item=8;                                       //出产设置800ma
        //Motor_Enable = enmode;                                //获取初始使能脚状态，不用获取初始状态，获取转动后的
/**************读flash初始化各个参数***************************************/

        STMFLASH_Read(Data_Store_Arrdess,table1,14);            //读存储
        SetModeCheck();                 //设置模式检测

        Currents=table1[1];             //默认800ma
        Menu_Num2_item =table1[2];
        
        stepangle=table1[3];             //默认4->4096电子齿
        Menu_Num3_item=table1[4];        //数组3、4号元素为电子齿轮的设置
        
        Motor_ENmode_flag =table1[5];    //数组5、6号元素为使能信号的设置
        Menu_Num4_item= table1[6];  
        
        Motor_Dir =table1[7];            //数组7、8号元素为电方向的设置
        Menu_Num5_item =table1[8];
        
//        Motor_Dir =table1[9];           //数组9、10号元素为模式的设置
//        Menu_Num5_item =table1[10];

        kp = table1[11];                //Kp
        ki = table1[12];                //Ki
        kd = table1[13];                //Kd
    }
    else{//进行校正
        OLED_ShowString(48,16,"NOT");
        OLED_ShowString(16,32,"Calibration");
        OLED_ShowString(0,48,"Please calibrate");
        delay_ms(500);
        //进入初始校准菜单界面显示
        OLED_Clear();
        OLED_ShowString(0,0,"->");

        while(1){
            KeyScan();
            Oled_display();
        }            
    }

    EXTIX_Init();                       //外部中断初始化
    NVIC_EnableIRQ(EXTI1_IRQn);         //使能外部中断1
    UART_Configuration(USART1, UART1_DEFAULT_BAUDRATE);     //串口初始化
    CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_5tq,6,CAN_Mode_LoopBack);//CAN初始化环回模式,波特率500Kbps 
    res=CAN1_Send_Msg(canbufTx,8);                      //发送8个字节
    delay_ms(100);
    if(res){                                            //出产测试用
        printf("CAN1 Transport fail!\r\n");
    }
    else{
        printf("CAN1 Transport OK!\r\n");
    }
    TIM2_Cap_Init(0xFFFF,0);          //配置定时器2为ETR外部脉冲计数
    TIM4_Init(7200-1, 0);             //定时器4初始化
#endif    
//    IWDG_Init(4,625);                 //看门狗初始化
 	while(1) 
    {
    #ifdef ENCLOSE_LOOP 
/**************************************************************/
        if(Motor_ENmode_flag ==1){
            if(ENIN==1) {                            //高电平锁
                restart_init();                //重新初始化一次电机初始参数 
//                Reset_status_flag=0;
//                enmode=1;                
            } 
            else{
                Reset_status_flag++;     //0++
                enmode=0; //跑到此处说明在使能状态为低电平使能时，使能脚由低变为高。可以说明长时间没有操作电机移动，或一次打印完成
            }    
        }
        if(Motor_ENmode_flag ==0){
            if(ENIN==0){
                restart_init(); //重新初始化一次电机初始参数   
//                Reset_status_flag=0;
//                enmode=1;
            }
            else{
                Reset_status_flag++;     //0++
                enmode=0; //跑到此处说明在使能状态为低电平使能时，使能脚由低变为高。可以说明长时间没有操作电机移动，或一次打印完成
            }
        }
/*******************************************************************/
        if(Reset_status_flag == 1){       //打印完成后复位状态
            enmode=0;
            Reset_status_flag ++;           //1++            
            PID_Cal_value_init();           //标记回到初始化状态
                
            wap1=0;
            wap2=0;
            Data_update_flag=1;//更新一次显示
        }else{
            if(Reset_status_flag>3)
                Reset_status_flag--;
        }
        
        usart_Receive_Process();                        //串口数据接收处理
        data_Process();
        test_uart();
        
        if(frame_Error_flag){
            frame_Error_flag =0;
            USART1_SendStr("Frame Err!\r\n");   
        }
        if(flash_store_flag ==1){
            flash_store_flag =0;

            //USART_Cmd(USART1, DISABLE);
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
            USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
            
            STMFLASH_Write(Data_Store_Arrdess,table1,14);//存储数据
            
            //USART_Cmd(USART1, ENABLE);
            USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
            USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
            
            //Reset_status_flag=1;
            //restart_init();
        }
        KeyScan();                                      //按键扫描
        Oled_display();                                 //菜单显示
        Motor_data_dis();                               //电机数据显示
/*************************************************************************************************/
#endif
#if 0
        if(KEY_Select==0){
			delay_ms(10);
			if(KEY_Select==0){
                if(k3_flag == 0){
                    k3_flag =1;
                    led1=!led1;			//熄灭
                    res=CAN1_Send_Msg(canbufTx,8);//发送8个字节
                    if(res)
                        printf("CAN1 Transport fail!\r\n");
                    else
                        printf("OK!\r\n");
                }
			}
		}
        else if(KEY_Confirm==0){
			delay_ms(10);
			if(KEY_Confirm==0){
                if(k4_flag == 0){
                    k4_flag =1; 
                    mode=!mode;
                    CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_5tq,6,mode);	//CAN普通模式初始化,普通模式,波特率500Kbps
                     
                    if(mode==0)//普通模式，需要2个开发板
                    {
                        printf("Nnormal Mode \r\n");	    
                    }else //回环模式,一个开发板就可以测试了.
                    {
                        printf("LoopBack Mode\r\n");
                    }                    
                    led1=1;			//off
                    delay_ms(200);
                    led1=0;			//on
                    delay_ms(200);
                    led1=1;			//off
                    delay_ms(200);
                    led1=0;			//on
                    delay_ms(200);
                }
            }
		}
        else{
            k3_flag = 0;
            k4_flag = 0;
        }
        key=CAN1_Receive_Msg(canbufRx);
		if(key)//接收到有数据
		{	
            printf("CAN Receive: ");	
 			for(i=0;i<key;i++){		
                printf("%x ",canbufRx[i]);
 			}
            printf("\r\n");
		}
        key=0;
#endif
	}
}
//重新初始化一次电机参数
void restart_init(void)
{
    //重新初始化一次电机初始参数
    if(Reset_status_flag !=0 ){
        CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN); 
        PID_Cal_value_init();           //清除标记回到初始化完成状态
        SET_BIT(TIM2->CR1, TIM_CR1_CEN);
    }
    enmode=1;
    Reset_status_flag=0;
}
#ifdef ENFLASH_TEST 
void flash_test(void)
{
    static char t=0;
    STMFLASH_Write(Data_Store_Arrdess,table,16);
    delay_ms(10);
    FLASH_Unlock();						//解锁
    STMFLASH_Read(Data_Store_Arrdess,table1,16);
    FLASH_Lock();//上锁
     printf("flash Read: ");	
    for(t=0;t<16;t++){		
        printf("%c ",table1[t]);
    }
    while(1);
}
#endif























/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
