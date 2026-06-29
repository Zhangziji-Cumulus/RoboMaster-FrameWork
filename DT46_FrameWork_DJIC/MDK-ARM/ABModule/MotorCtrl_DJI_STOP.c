#include "MotorCtrl_DJI_STOP.h"

const DJI_MotorFeedback_t* test;

void DJI_MOTOR_EmergencySTOP_ALL(PID_HandleTypeDef* Motor_STOP,const DJI_MotorFeedback_t DJI_MFeedback[],CAN_HandleTypeDef *hcan,float MError)
{
	int16_t PIDoutput1[4];
	int16_t PIDoutput2[4];
	
	
	
	test = DJI_MFeedback;

	// -------------------------- 第一组 4个电机 --------------------------
	// 电机0
	PID_Reset(Motor_STOP);
	PIDoutput1[0] = PID_Calculate(Motor_STOP, test[0].speed_rpm, 0);
	
	// 电机1
	PID_Reset(Motor_STOP);
	PIDoutput1[1] = PID_Calculate(Motor_STOP, test[1].speed_rpm, 0);
	
	// 电机2
	PID_Reset(Motor_STOP);
	PIDoutput1[2] = PID_Calculate(Motor_STOP, test[2].speed_rpm, 0);
	
	// 电机3
	PID_Reset(Motor_STOP);
	PIDoutput1[3] = PID_Calculate(Motor_STOP, test[3].speed_rpm, 0);
	
	// 发送
	ESC_Control_Raw_Group(hcan, 1, PIDoutput1);

	// -------------------------- 第二组 4个电机 --------------------------
	// 电机4
	PID_Reset(Motor_STOP);
	PIDoutput2[0] = PID_Calculate(Motor_STOP, test[4].speed_rpm, 0);
	
	// 电机5
	PID_Reset(Motor_STOP);
	PIDoutput2[1] = PID_Calculate(Motor_STOP, test[5].speed_rpm, 0);
	
	// 电机6
	PID_Reset(Motor_STOP);
	PIDoutput2[2] = PID_Calculate(Motor_STOP, test[6].speed_rpm, 0);
	
	// 电机7
	PID_Reset(Motor_STOP);
	PIDoutput2[3] = PID_Calculate(Motor_STOP, test[7].speed_rpm, 0);

	
	ESC_Control_Raw_Group(hcan,1,PIDoutput1);
	ESC_Control_Raw_Group(hcan,4,PIDoutput2);
}


