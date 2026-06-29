#include "HERO_DriveSystem.h"
#include "HERO_API.h"

extern Dual_Board_Transmit_t DBT_RX;

extern HOTRC_Ctl_t RC_Ctl;
extern DJI_MotorFeedback_t DJI_MFeedback_CAN1[8];
extern DJI_MotorFeedback_t DJI_MFeedback_CAN2[8];

extern fp32 INS_angle[3];
extern float IMU_DegAngle[3];

// 全局标志：0=停止电机  1=正常运行
extern uint8_t g_motor_run_enable;

//定义地盘数据
extern PID_HandleTypeDef Chassis_Follow_PID;
Mecanum_Data_t Chassis_Mec;
Chassis_Move_t Chassis_Move;
Wheels_Data_t wheels;

//定义云台数据
Gimbal_Data_t GD;

//定义摩擦轮数据
ShootingVel_t SV;


void get_chassis_data(void);
void get_gimbal_data(void);

void HERO_DriveSystem_Init(void)
{

}

static UBaseType_t remain_HEROSystemTask;
__attribute__((used)) void HEROSystemTask(void *argument)
{
  for(;;)
  {	
		
		remain_HEROSystemTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

static UBaseType_t remain_HEROChassisTask;
__attribute__((used)) void HEROChassisTask(void *argument)
{
  for(;;)
  { 
			get_chassis_data();
			static uint16_t timecount = 200;
			if (SBUS_IsRemoteLost())
     {
			 	//电机急刹车
				if(timecount < 150)
				{
					DJI_MOTOR_EmergencySTOP_ALL(DJI_MFeedback_CAN1,&hcan1,10.0f);
					timecount++;
				}
		 }
		else if(g_motor_run_enable == 0)
		{
			//电机急刹车
			if(timecount < 150)
			{
				DJI_MOTOR_EmergencySTOP_ALL(DJI_MFeedback_CAN1,&hcan1,10.0f);
				timecount++;
			}
		}
		else
		{	
			timecount = 0;
			Chassis_Move_Calc(&Chassis_Move,&Chassis_Mec,&Chassis_Follow_PID,RC_Ctl.Switch.S3_R);
			Motor_DJI_Chassis(DJI_MFeedback_CAN1,&hcan1,1.0f,Chassis_Mec.FL.T_rpm,Chassis_Mec.FR.T_rpm,Chassis_Mec.BL.T_rpm,Chassis_Mec.BR.T_rpm);	
		}
//==============================================================//
		remain_HEROChassisTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

static UBaseType_t remain_HEROGimbalTask;
__attribute__((used)) void HEROGimbalTask(void *argument)
{
  for(;;)
  {	
		static uint16_t timecount = 0;
		
			if (SBUS_IsRemoteLost())
     {
			 	//电机急刹车
				if(timecount < 150)
				{
					DJI_MOTOR_EmergencySTOP_ALL(DJI_MFeedback_CAN2,&hcan2,10.0f);
					timecount++;
				}
		 }
		else if(g_motor_run_enable == 0)
		{
			//电机急刹车
			if(timecount < 150)
			{
				DJI_MOTOR_EmergencySTOP_ALL(DJI_MFeedback_CAN2,&hcan2,10.0f);
				timecount++;
			}
		}
		else
		{		
			if(abs(DJI_MFeedback_CAN2[0].current_ma) > 2000 
				     || DJI_MFeedback_CAN2[3].current_ma > 2000 
			       ){
					Motor_DJI_IMUPitchContral(DJI_MFeedback_CAN2,0,0,1,1);
					Motor_DJI_IMUYawContral(DJI_MFeedback_CAN2,0,0,5,2);
			}
			else
			{
				timecount = 0;
				HERO_Gimbal_PitchStable(&GD,RC_Ctl.Stick.RY,-(IMU_DegAngle[2]));
				Motor_DJI_IMUPitchContral(DJI_MFeedback_CAN2,GD.TAngle.Pitch,GD.CAngle.Pitch,1,1);
			
				HERO_Gimbal_YawStable(&GD,RC_Ctl.Stick.RX,IMU_DegAngle[0]);
				Motor_DJI_IMUYawContral(DJI_MFeedback_CAN2,GD.TAngle.Yaw,GD.CAngle.Yaw,5,2);
			}
		}
//==============================================================//
		remain_HEROGimbalTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

static UBaseType_t remain_HEROShootingTask;
__attribute__((used)) void HEROShootingTask(void *argument)
{
  for(;;)
  {
		static uint16_t timecount = 0;
		
		if (SBUS_IsRemoteLost())
    {
			 	//电机急刹车
				if(timecount < 150)
				{
					DJI_MOTOR_EmergencySTOP_ALL(DJI_MFeedback_CAN1,&hcan1,10.0f);
					timecount++;
				}
		}
		else if(g_motor_run_enable == 0)
		{
			//电机急刹车
			if(timecount < 150)
			{
				DJI_MOTOR_EmergencySTOP_ALL(DJI_MFeedback_CAN1,&hcan1,10.0f);
				timecount++;
			}
		}
		else
		{
			timecount = 0;
			ShootingVel_Calc(5,&SV,RC_Ctl.Switch.S2_R);
			Motor_DJI_ShootingFri(DJI_MFeedback_CAN1,&hcan1,1.0f,SV.UP_Lrpm,SV.UP_Rrpm,SV.Dowm_Mrpm);	
		}
		
//=============================检测剩余栈=================================//
		remain_HEROShootingTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

static UBaseType_t remain_HERODialTask;
__attribute__((used)) void HERODialTask(void *argument)
{

  for(;;)
  {

//=============================检测剩余栈=================================//
		remain_HERODialTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

void get_chassis_data(void)
{ 
	 //Chassis_Move.RelativeAngle_Degree = MyMath_cal_output_angle(DJI_MFeedback_CAN3[4].angle_deg,2)- ERRORANGLE;
	 //Chassis_Move.RelativeAngle_Degree = MyMath_normalize_m180_to_p180(Chassis_Move.RelativeAngle_Degree);
	
	 //Chassis_Move.RelativeAngle_Radian = MyMath_Degrees_To_Radians(Chassis_Move.RelativeAngle_Degree);
	 
	 Chassis_Move.Vel.FB = MyMath_Map_Range(RC_Ctl.Stick.LY,-HOTRC_RANGE,HOTRC_RANGE,-CHASSIS_FORWARD_MAX_SPEED,CHASSIS_FORWARD_MAX_SPEED);
	 Chassis_Move.Vel.RL = MyMath_Map_Range(RC_Ctl.Stick.LX,-HOTRC_RANGE,HOTRC_RANGE,-CHASSIS_FORWARD_MAX_SPEED,CHASSIS_FORWARD_MAX_SPEED);	

	
}
