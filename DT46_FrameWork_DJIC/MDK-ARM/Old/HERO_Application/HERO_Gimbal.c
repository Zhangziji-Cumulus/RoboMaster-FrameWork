#include "HERO_Gimbal.h"

extern PID_HandleTypeDef Pitch_PID_In;
extern PID_HandleTypeDef Pitch_PID_Ex;


int16_t HERO_IMUStable_PID(Gimbal_Data_t* GD);

void HERO_Gimbal_YawStable(Gimbal_Data_t* GD,int16_t RC_ctl,float IMU_Yaw)
{
	GD->TAngle.Yaw = GD->TAngle.Yaw - MyMath_Map_Range(RC_ctl,-HOTRC_RANGE,HOTRC_RANGE,-0.25f,0.25f);
	GD->TAngle.Yaw = MyMath_Limit_Float(GD->TAngle.Yaw,-180.00f,180.00f,1);

	GD->CAngle.Yaw = IMU_Yaw;
}

void HERO_Gimbal_PitchStable(Gimbal_Data_t* GD,int16_t RC_ctl,float IMU_Pitch)
{
	GD->TAngle.Pitch = GD->TAngle.Pitch - MyMath_Map_Range(RC_ctl,-HOTRC_RANGE,HOTRC_RANGE,-0.1f,0.1f);
	GD->TAngle.Pitch = MyMath_Limit_Float(GD->TAngle.Pitch,-GIMBAL_PITCH_MAX_ELE,GIMBAL_PITCH_MAX_DEP,0);
	GD->CAngle.Pitch = MyMath_Limit_Float(IMU_Pitch,-GIMBAL_PITCH_MAX_ELE,GIMBAL_PITCH_MAX_DEP,0);
}

int16_t HERO_IMUStable_PID(Gimbal_Data_t* GD)
{
	int16_t PIDOutput;
	
	//PIDOutput = PID_Double_Calculate(&Pitch_PID_In,&Pitch_PID_Ex,GD->TAngle.Pitch,GD->,GD->CAngle.Pitch);
	
	return PIDOutput;
}
