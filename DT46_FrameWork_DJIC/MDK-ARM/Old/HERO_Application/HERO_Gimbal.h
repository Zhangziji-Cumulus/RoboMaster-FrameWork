#ifndef __HERO_GIMBAL_H
#define __HERO_GIMBAL_H

#include "main.h"
#include "HERO_API.h"

#define GIMBAL_PITCH_MAX_ELE 17.0f
#define GIMBAL_PITCH_MAX_DEP 43.0f

typedef struct{

	struct{
		float Yaw;
		float Pitch;
	}TAngle;
	
	struct{
		float Yaw;
		float Pitch;
	}CAngle;
	
}Gimbal_Data_t;


void HERO_Gimbal_YawStable(Gimbal_Data_t* GD,int16_t RC_ctl,float IMU_Yaw);
void HERO_Gimbal_PitchStable(Gimbal_Data_t* GD,int16_t RC_ctl,float IMU_Pitch);

#endif
