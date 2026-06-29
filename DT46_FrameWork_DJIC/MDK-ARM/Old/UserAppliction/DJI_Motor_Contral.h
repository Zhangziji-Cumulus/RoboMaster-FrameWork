#ifndef __MOTOR_CONTRAL_H
#define __MOTOR_CONTRAL_H

#include "main.h"
#include "MY_PID.h"
#include "DJI_Motor_CAN.h"
#include "can.h"
#include "My_Math.h"
#include "Dual_board_Transmit.h"


void Motor_Init(void);
void Motor_DJI_Speed_SingleContral(int16_t MotorVel);
void Motor_DJI_Angle_SingleContral(DJI_MotorFeedback_t DJI_MFeedback[],float TargetAngle,uint8_t ID,uint16_t gear_ratio);

//¿ØÖÆµØÅÌ
void Motor_DJI_Chassis(DJI_MotorFeedback_t DJI_MFeedback[],
											 CAN_HandleTypeDef *hcan,
											 float   MError,
											 int16_t MotorVel_1,
											 int16_t MotorVel_2,
										   int16_t MotorVel_3,
											 int16_t MotorVel_4);
//¿ØÖÆÔÆÌ¨
void Motor_DJI_IMUPitchContral(DJI_MotorFeedback_t DJI_MFeedback[],float TargetAngle,float IMUAngle,uint8_t ID,uint16_t gear_ratio);
void Motor_DJI_IMUYawContral(DJI_MotorFeedback_t DJI_MFeedback[],float TargetAngle,float IMUAngle,uint8_t ID,uint16_t gear_ratio);
//¿ØÖÆÄ¦²ÁÂÖ
void Motor_DJI_ShootingFri(DJI_MotorFeedback_t DJI_MFeedback[],
														CAN_HandleTypeDef *hcan,
	                          float   MError,
														int16_t SFri_UL_M,
														int16_t SFri_UR_M,
														int16_t SFri_MD_M);

//¼±Í£
void DJI_MOTOR_EmergencySTOP_ALL(DJI_MotorFeedback_t DJI_MFeedback[],CAN_HandleTypeDef *hcan,float MError);
void DJI_MOTOR_STOP_ALL(CAN_HandleTypeDef *hcan);


#endif
