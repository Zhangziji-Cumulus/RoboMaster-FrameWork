#ifndef __HERO_CHASSIS_H
#define __HERO_CHASSIS_H

#include "HERO_API.h"
#include "MY_PID.h"
#include "HERO_Math.h"


#define MECANUM_WHEEL_RADIUS_MM 		76.0f
#define CHASSIS_FORWARD_MAX_SPEED 	15.0f //线速度，单位：m/s

#define ERRORANGLE 135.0f

typedef struct{

	int16_t FB;// 前后
  int16_t RL;// 左右
  int16_t Rv;// 旋转
	
}Mecanum_Target_t;

//每个轮子单独的数据
typedef struct{

	int16_t T_Speed;
	int16_t T_rpm;     //目标转速
	int16_t C_rpm;     //当前速度
	int16_t C_Curr;    //当前电流
	int16_t C_Enc;     //当前编码器值
	int16_t Out_vel;   //输出值
	int16_t Radius;	   //

}Wheel_Data_t;

//四个轮子的数据
typedef struct{

	Wheel_Data_t W_FL;
	Wheel_Data_t W_FR;
	Wheel_Data_t W_BL;
	Wheel_Data_t W_BR;
	
}Wheels_Data_t;

//麦轮解算
typedef struct{
	
	Mecanum_Target_t Target;
	
	Wheel_Data_t FL;
	Wheel_Data_t FR;
	Wheel_Data_t BL;
	Wheel_Data_t BR;
	
}Mecanum_Data_t;

//地盘运动解算结构体
typedef struct{

	float Theta_Radian;
	float Theta_Degree;
	float Gimbal_FWVector;//云台向前向量
    float Chassis_FWVector;//地盘向前向量
	float RelativeAngle_Radian;
	float RelativeAngle_Degree;
	
	struct{
		int16_t FB;
		int16_t RL;
	}Vel;

	
}Chassis_Move_t;

//每个麦轮速度解算
void Chassis_Mecanum_Calc(Mecanum_Data_t* pMD);
void Chassis_Move_Calc(Chassis_Move_t* pCM,Mecanum_Data_t* MD,PID_HandleTypeDef *pid,uint8_t state);

#endif
