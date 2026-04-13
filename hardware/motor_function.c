#include "motor_function.h"
#include "as5047d.h"
#include "gpio.h"


//typedef struct PID 
//{
////    int SetPoint;           //设定目标    
////    long SumError;          //误差累计
//    float Proportion;      //比例常数
//    float Integral;        //积分常数
//    float Derivative;      //微分常数
//    float LastError;          //Error[-1]
//    float PrevError;          //Error[-1]
//    uint8_t index; 
//    float integral ;
//}PID;
//static PID sPID;
//static PID *sptr = &sPID;


//PID参数初始化
//增量式PID参数初始化
void IncPIDInit(void)
{
//    sptr->SumError =0;          //累计误差
    sptr->LastError =0;         //Error[-1]
    sptr->PrevError =0;         //Error[-2]
    
    sptr->Proportion =0.9;        //比例系数
    sptr->Integral =0.0001;       //积分系数
    sptr->Derivative =0.01;       //微分系数
    sptr->index =0;
    sptr->integral =0;
//    sptr->SetPoint =0;          //
}
////增量式PID计算
////
//int IncPIDCalc(int NextPoint)
//{
//    int iError,iIncpid;                                 //当前误差
//    iError = sptr->SetPoint - NextPoint;                //增量计算
//    iIncpid = sptr->Proportion * iError                 //E[K]项
//            - sptr->Integral * sptr->LastError          
//            + sptr->Derivative * sptr->PrevError;
//    
//    sptr->PrevError = sptr->LastError;
//    sptr->LastError = iError;
//    
//    return iIncpid;
//}

int Incremental_PI(int Encoder , int Target)
{
    float Kp=20,Ki=30;
    static int Bias,Pwm,Last_bias ;
    Bias = Encoder - Target;
    
    
    Pwm+= Kp*(Bias - Last_bias)+ Ki*Bias;
    Last_bias = Bias;
    return Pwm;
}
//Encoder :当前角度
float Position_PID(float Current_Angle , float Target_angle)
{
//    float Position_KP=2,Position_KI=0.0,Position_KD = 0;
    static float  iError,iIncpid;                             //当前误差
    iError = Target_angle - Current_Angle;              //增量计算

    if(iError >=18 || iError <= -18){
        sptr->index=0;
      return iError;
    }
    else{
        sptr->index=1;
    }
//        {
    
    iIncpid = sptr->Proportion * (iError - sptr->LastError)                 
            + sptr->index * sptr->Integral * iError         
            + sptr->Derivative * (iError - 2* sptr->LastError + sptr->PrevError);
        
    sptr->PrevError = sptr->LastError;
    sptr->LastError = iError;

    //    iError =Target_angle  - Current_Angle;                        //当前误差
    //    Integral_bias += Bias;                                      //积分累计误差   
    //    iIncpid +=  Position_KP*(iError - sptr->LastError) +Position_KI*iError  ;
     //    sptr->LastError = iError ;

//        if(iIncpid >=0.9 ){ //设置PID计算的超调范围,设定最大脉冲数360° x 细分/1.8°
//            iIncpid =1.8;
//        }
//        else if(iIncpid<=-0.9){
//            iIncpid=-1.8;
//        }
//    }
//    return iError;
    return iIncpid;
}


















