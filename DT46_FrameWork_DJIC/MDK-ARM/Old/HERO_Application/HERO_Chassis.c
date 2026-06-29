#include "HERO_Chassis.h"


//地盘运动解算
/*
* @param state 值为1时是地盘跟随模式，值为2时是小陀螺模式
*/
void Chassis_Move_Calc(Chassis_Move_t* pCM,Mecanum_Data_t* pMD,PID_HandleTypeDef *pid,uint8_t state)
{
	pCM->Theta_Radian = pCM->RelativeAngle_Radian - MyMath_Degrees_To_Radians(0.0f);
	
	pCM->Theta_Degree = pCM->RelativeAngle_Degree;
	
	if(state == 1)
	{
		pMD->Target.FB = pCM->Vel.FB;
		pMD->Target.RL = pCM->Vel.RL;
		pMD->Target.Rv = PID_Calculate_CycleAngle(pid,pCM->Theta_Degree,0.0f);
	}
	else if(state == 2)
	{
		pMD->Target.FB = ((pCM->Vel.RL * sin(pCM->RelativeAngle_Radian)) + (pCM->Vel.FB * cos(pCM->RelativeAngle_Radian)));
		pMD->Target.RL = ((pCM->Vel.RL * cos(pCM->RelativeAngle_Radian)) - (pCM->Vel.FB * sin(pCM->RelativeAngle_Radian)));
		pMD->Target.Rv = 5;//顺时针
	}
	else if(state == 3)
	{
		pMD->Target.FB = (-(pCM->Vel.RL * sin(pCM->RelativeAngle_Radian)) + (pCM->Vel.FB * cos(pCM->RelativeAngle_Radian)));
		pMD->Target.RL = (-(pCM->Vel.RL * cos(pCM->RelativeAngle_Radian)) - (pCM->Vel.FB * sin(pCM->RelativeAngle_Radian)));
		pMD->Target.Rv = -5;//逆时针
	}
	
	Chassis_Mecanum_Calc(pMD);
}

//每个麦轮速度解算
// 麦轮标准解算：FL 左前  FR 右前  BR 右后  BL 左后
void Chassis_Mecanum_Calc(Mecanum_Data_t* pMD)
{
	pMD->FL.T_Speed =  (pMD->Target.RL) + (pMD->Target.FB) + (pMD->Target.Rv);
	pMD->BL.T_Speed = -(pMD->Target.RL) + (pMD->Target.FB) + (pMD->Target.Rv);
	pMD->FR.T_Speed =  (pMD->Target.RL) - (pMD->Target.FB) + (pMD->Target.Rv);
	pMD->BR.T_Speed = -(pMD->Target.RL) - (pMD->Target.FB) + (pMD->Target.Rv);
	
	pMD->FL.T_rpm = calc_wheel_rpm(pMD->FL.T_Speed,(MECANUM_WHEEL_RADIUS_MM/1000.0f));
	pMD->FR.T_rpm = calc_wheel_rpm(pMD->FR.T_Speed,(MECANUM_WHEEL_RADIUS_MM/1000.0f));
	pMD->BL.T_rpm = calc_wheel_rpm(pMD->BL.T_Speed,(MECANUM_WHEEL_RADIUS_MM/1000.0f));
	pMD->BR.T_rpm = calc_wheel_rpm(pMD->BR.T_Speed,(MECANUM_WHEEL_RADIUS_MM/1000.0f));
}



