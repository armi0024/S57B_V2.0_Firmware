#include "key.h"
#include "gpio.h"
#include "oled.h"
#include "tle5012.h"

uint8_t KEY_Select_flag=0;       //选择键标志
uint8_t KEY_Back_flag=0;         //返回键标志
uint8_t KEY_Confirm_flag=0;      //确定键标志

uint8_t Enter_exit_flag =0;      //进入退出功能标志
uint8_t enter_num=0; 


void Key_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//关闭jtag功能
    
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
    
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //端口配置
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB

}


void Key_test(void)
{
    if(KEY_Select == 0){
        delay_ms(10);
        if(KEY_Select==0){
           if(KEY_Select_flag ==0){//等待松手
                KEY_Select_flag=1;
                 led1 = !led1;  
            } 
        }
    }else if(KEY_Back == 0){
            delay_ms(10);
            if(KEY_Back==0){
                if(KEY_Back_flag ==0){//等待松手
                KEY_Back_flag=1;
                 led1 = !led1;  
            } 
        }
    }else if(KEY_Confirm == 0){
        delay_ms(10);
        if(KEY_Confirm==0){
           if(KEY_Confirm_flag ==0){//等待松手
                KEY_Confirm_flag=1;
                 led1 = !led1;  
            } 
        }
    }else
    {
        KEY_Select_flag=0;       //选择键标志
        KEY_Back_flag=0;         //返回键标志
        KEY_Confirm_flag=0;
    }
    
}

//按键扫描
void KeyScan(void)
{
    if(KEY_Select == 0){        //功能选择键
        delay_ms(10);
        if(KEY_Select == 0){
            if(Menu_Num ==0){//在校准界面有效
                if(KEY_Select_flag ==0){//等待松手
                    KEY_Select_flag=1;
                    Menu_update_flag=1;                 //允许更新显示
                    
                    if((Calibration_flag>>8)!=0xAA){         //出产校准界面标志
                        Menu_Num_item++;                //子菜单标记
                        if(Menu_Num_item>1)             //出产校准界面菜单项限制
                            Menu_Num_item=0;
                    }
                    else if(Second_Menu==1){
                            Menu_Num_item++;                //子菜单标记
                            if(Menu_Num_item>MENU_NUM){            //功能主界面菜单项限制
                                Menu_Num_item =2;
                        }
                    }
                    else if(Second_Menu==2){                //进入电流设置界面选择
                            Menu_Num2_item++;
                            if(Menu_Num2_item>33)
                                Menu_Num2_item=0;
                    }
                    else if(Second_Menu==3){                //进入细分设置界面选择
                            Menu_Num3_item++;
                            if(Menu_Num3_item>4)
                                Menu_Num3_item=0;
                    }
                    else if(Second_Menu==4){                //进入使能设置界面选择
                            Menu_Num4_item++;
                            if(Menu_Num4_item>1)
                                Menu_Num4_item=0;
                    }
                    else if(Second_Menu==5){                //进入方向设置界面选择
                            Menu_Num5_item++;
                            if(Menu_Num5_item>1)
                                Menu_Num5_item=0;
                    }
//                    else if(Second_Menu==6){                //进入电机工作模式界面选择
//                            Menu_Num6_item++;
//                            if(Menu_Num6_item>1)
//                                Menu_Num6_item=0;
//                    }
                }
            }
        }
    }else if(KEY_Back == 0){
        delay_ms(10);
        if(KEY_Back == 0){
            if(KEY_Back_flag ==0){
                KEY_Back_flag=1;
                Menu_update_flag=1;         //标记界面更新标志
                OLED_Clear();                   //清屏
                //如果是通过菜单界面进入的返回功能就不标记菜单选择功能，
                if(Enter_exit_flag == 1){
                    Enter_exit_flag=0;
                    Menu_Num=0;                         //复位界面标记
                    Menu_Num_item=2;                    //复位子项目标记
                    Second_Menu=1;
                    //break;
                }else {
                    Menu_Num++;
                    if(Menu_Num >1 ){
                        Menu_Num=0;
                    }
                }
            }
        }
    }else if(KEY_Confirm == 0){
        delay_ms(10);
        if(KEY_Confirm == 0){
            if(KEY_Confirm_flag ==0){
                KEY_Confirm_flag=1;
                switch(Menu_Num){
                    case 0:     
                        switch(Menu_Num_item){                  //菜单项选择
                            case 0:
                                    SetModeCheck();             //执行校准功能
//                                    CalibrateEncoder_finish_flag=1; //校准完成
//                                    //写flash 标记已校准过一次 ，待添加
//                                    flash_store_flag =1;            //标记flash更新
//                                    table1[0] =0xAA;
//                                    Calibration_flag =0xAA;    //标记已校准标志
//                                    Second_Menu=1;             //标记进入二级子菜单 
//                            
//                                    OLED_Clear();   //校准完成后清屏进入校准后的主界面
//                                    OLED_ShowString(0,0,"->Calibrate");
//                                    OLED_ShowString(0,16,"  Motor mA");
//                                    OLED_ShowString(0,32,"  Microstep");
//                                    OLED_ShowString(0,48,"  EnablePin");
//                                    Menu_Num_item=2;                                //设置初始菜单项
                            break;
                            case 1: Enter_exit_flag=1;    //执行退出功能
                                    Menu_Num=1;           //标记界面为主显示界面  
                                    Menu_update_flag=1;   //允许更新一次界面
                                    OLED_Clear();         //清屏一次
                            break ;
                            case 2: 
                                    TIM_Cmd(TIM2,DISABLE);
                                    enmode=0;                       ////关闭使能
 //                                   NVIC_DisableIRQ(EXTI0_IRQn);
                                    NVIC_DisableIRQ(EXTI1_IRQn);//执行菜单校准功能前先关闭中断
                                    Second_Calibrate_flag=1;                        //标记二次校准

                                    SetModeCheck();             //执行校准功能，执行完成后要按复位重新执行
//                                    NVIC_EnableIRQ(EXTI0_IRQn);
                                    NVIC_EnableIRQ(EXTI1_IRQn);
                                    TIM_Cmd(TIM2,ENABLE);
                                    enmode=1;                       ////关闭使能
                                    Second_Menu=1;              //
                                break ;
                            case 3:
                                    OLED_Clear();   //校准完成后清屏进入校准后的主界面
                                    Menu_update_flag=1;   //允许更新一次界面
                                    enter_num++;
                                    if(enter_num==1)
                                        Second_Menu=2;        //电流设置界面
                                    if(enter_num==2){
                                        enter_num=0;
                                        Second_Menu=1;
                                        //将电流值大小传给指定变量进行计算匹配
                                        //将界面值传到此处
                                        Currents= Currents_Set;//设置电流
                                        flash_store_flag =1;
                                        table1[1]=Currents;
                                        table1[2]=Menu_Num2_item;
                                    }
                                break ;
                            case 4: OLED_Clear();   //校准完成后清屏进入校准后的主界面
                                    Menu_update_flag=1;   //允许更新一次界面
                                    enter_num++;
                                    if(enter_num==1)
                                        Second_Menu=3;        //微步设置
                                    if(enter_num==2){
                                        enter_num=0;
                                        Second_Menu=1;
                                        enmode =0;                      //关闭使能，等参数设置完成再使能
                                        PID_Cal_value_init();           /*从新初始化数据，防止调细分后突然位移*/
                                        //Reset_status_flag=0xff;
                                        //将界面值传到此处
                                        stepangle =Microstep_Set;   //设置电子齿轮
                                        
                                        flash_store_flag =1;        //存储标志打开
                                        table1[3]=stepangle;        //数组3、4号元素为电子齿轮的设置
                                        table1[4]=Menu_Num3_item;
                                    }
                                break ;
                            case 5: OLED_Clear();   //校准完成后清屏进入校准后的主界面
                                    Menu_update_flag=1;   //允许更新一次界面
                                    enter_num++;
                                    if(enter_num==1)
                                        Second_Menu=4;        //使能设置
                                    if(enter_num==2){
                                        enter_num=0;
                                        Second_Menu=1;
                                        enmode =0;                          //关闭使能，等参数设置完成再使能
                                        PID_Cal_value_init();               //重新设置初始值
                                        //将界面值传到此处
                                        if(Dir_Enable == 0xaa ){
                                            //enmode =1;
                                          Motor_ENmode_flag=0;
                                        }else if(Dir_Enable == 0x55  ){
                                            Motor_ENmode_flag=1;
                                            //enmode =0;
                                        }
                                        if(Dir_Enable==0x55 || Dir_Enable ==0xAA){
                                            Dir_Enable =0;
                                            flash_store_flag =1;//
                                            table1[5]=Motor_ENmode_flag;        //数组5、6号元素为使能信号的设置
                                            table1[6]=Menu_Num4_item;
                                            
                                        }
                                    }
                                break ;
                            case 6: OLED_Clear();   //校准完成后清屏进入校准后的主界面
                                    Menu_update_flag=1;   //允许更新一次界面
                                    enter_num++;
                                    if(enter_num==1)
                                        Second_Menu=5;        //方向设置
                                    if(enter_num==2){
                                        enter_num=0;
                                        Second_Menu=1;
                                        
                                        //将界面值传到此处
                                        //Dir_Enable 默认方向是位1，默认使能位 是位0;
                                        if(Dir_Enable ==0x11){
                                            Motor_Dir =1;
                                        }else if(Dir_Enable == 0x22){
                                            Motor_Dir=0;
                                        }
                                        //if(Dir_Enable ==0x11 || Dir_Enable==0x22){
                                            Dir_Enable=0;
                                        //}
                                        flash_store_flag =1;//待验证方向
                                        table1[7]=Motor_Dir;                    //数组7、8号元素为电方向的设置
                                        table1[8]=Menu_Num5_item;
                                    }
                                break ;
                                    
//                            case 7:OLED_Clear();   //校准完成后清屏进入校准后的主界面
//                                    Menu_update_flag=1;   //允许更新一次界面
//                                    enter_num++;
//                                    if(enter_num==1)
//                                        Second_Menu=6;        //电设置
//                                    if(enter_num==2){
//                                        enter_num=0;
//                                        Second_Menu=1;
//                                        //将界面值传到此处
//                                        //Dir_Enable 默认方向是位1，默认使能位 是位0;
//                                        if(Dir_Enable ==0x33){
//                                            Motor_mode =0;
//                                            closemode=1;        //闭环模式打开
//                                        }else if(Dir_Enable == 0x44){
//                                            Motor_mode=1;
//                                            closemode=0;        //闭环模式关闭
//                                        }
//                                        
//                                        Dir_Enable=0;
////                                        flash_store_flag =1;        //存储
////                                        table1[9]=Motor_mode;        //
////                                        table1[10]=Menu_Num6_item;
//                                    }
//                                break ;
                            case 7:Enter_exit_flag=1;    //执行退出功能
                                    Menu_Num=1;           //标记界面为主显示界面  
                                    Menu_update_flag=1;   //允许更新一次界面
                                    OLED_Clear();         //清屏一次
                                break ;
                            default:break ;
                        }
                    break ;
//                            case 1: Enter_exit_flag=1;    //执行退出功能
//                                
//                            break ;
                }
            }
        }
    }else{
        KEY_Select_flag=0;       //选择键标志
        KEY_Back_flag=0;         //返回键标志
        KEY_Confirm_flag=0;
    }
}









