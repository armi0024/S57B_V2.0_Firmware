#include "angle.h"

uint16_t angle;


//	Angle0(int32_t x) {angle=(uint16_t)x;}
//	Angle1(const Angle &x) {angle=x.angle;}

int16_t operator1( const uint16_t a2)
{
    int32_t x,y,dx;
    x=(int32_t)angle;
    y=(int32_t)a2;
    dx=x-y;
    if (abs(x-y)>ANGLE_STEPS/2)
    {
        //we have a wrap condition
        if (x>y)
        {
            dx=x-(y+ANGLE_STEPS);
        }else if (x<y)
        {
            dx=(ANGLE_STEPS+x)-y;
        }
    }
    return (int16_t)dx;
}
//
//	int16_t operator-( const int32_t y)
//	{
//		int32_t x,y,dx;
//		x=(int32_t)angle;
//		while(y>ANGLE_MAX)
//		{
//			y=y-ANGLE_STEPS;
//		}
//		while(y<-ANGLE_MAX)
//		{
//			y=y+ANGLE_STEPS;
//		}
//
//		dx=x-y;
//		if (abs(x-y)>ANGLE_STEPS/2)
//		{
//			//we have a wrap condition
//			if (x>y)
//			{
//				dx=x-(y+ANGLE_STEPS);
//			}else if (x<y)
//			{
//				dx=(ANGLE_STEPS+x)-y;
//			}
//		}
//		return (int16_t)dx;
//	}

	uint16_t operator2(const uint16_t y)
	{
		uint16_t a;
		a=angle+ (uint16_t)y;
		return (a);
	}
	uint16_t operator3(const long int x)
	{
		int32_t a;
		a=(int32_t)angle+ x;
		while (a>=ANGLE_STEPS)
		{
			a=a-ANGLE_STEPS;
		}
		while (a<0)
		{
			a=a+ANGLE_STEPS;
		}
		return (uint16_t)a;
	}
	uint16_t operator4(const unsigned long int x)
	{
		uint32_t a;
		a=(uint32_t)angle+ x;
		while (a>=ANGLE_STEPS)
		{
			a=a-ANGLE_STEPS;
		}
		return ((uint16_t)a);
	}
    
