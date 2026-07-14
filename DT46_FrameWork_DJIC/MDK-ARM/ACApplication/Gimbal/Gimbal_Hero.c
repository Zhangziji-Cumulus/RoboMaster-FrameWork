#include "Gimbal_HERO.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == GIMBAL_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

#if(ROBOT_TYPE == ROBOTTYPE_HERO)

//** #################################################################################################### **//
//** ====================================== 定义数据、结构体 ============================================= **//
//** #################################################################################################### **//

//定义内部数据
Gimbal_Instance_t Gimbal_Instance;

//定义PID参数
PID_HandleTypeDef Gimbal_Motor_STOP;

PID_FF_HandleTypeDef Gimbal_Yaw_FF;
PID_HandleTypeDef Gimbal_Yaw_In;
PID_HandleTypeDef Gimbal_Yaw_Ex;

PID_FF_HandleTypeDef Gimbal_Pitch_FF;
PID_HandleTypeDef Gimbal_Pitch_In;
PID_HandleTypeDef Gimbal_Pitch_Ex;

//** #################################################################################################### **//
//** ========================================= 对内函数声明 ============================================== **//
//** #################################################################################################### **//

static void Gimbal_Update_Target(void);
static void Gimbal_SetSafe_Vel(void);

static void Gimbal_YawStable_Calc(void);
static void Gimbal_PitchStable_Calc(void);

//** #################################################################################################### **//
//** ====================================== 对外若定义覆盖函数 =========================================== **//
//** #################################################################################################### **//

//初始化函数
void Gimbal_Init(void)
{
    //初始模式STOP
    Gimbal_Instance.CMD.ctrl = STOP_MODE;

   	//电机急停
  	PID_Init(&Gimbal_Motor_STOP,3.0f,0.0f,0.0f,-DJI_STOP_A,DJI_STOP_A,-5.0f, 5.0f);

    //YAW轴PID
	PID_FF_Init(&Gimbal_Yaw_FF,3800.0f,0.05f,320000.0f,1500.0f,-DJI_GM6020_R,DJI_GM6020_R,-1000.0f, 1000.0f);
	PID_Init(&Gimbal_Yaw_In,5.0f,0.0f,0.0f,-DJI_GM6020_R,DJI_GM6020_R,-10.0f, 10.0f);
	PID_Init(&Gimbal_Yaw_Ex,200.0f,0.0f,0.0f,-1000,1000,-10.0f, 10.0f);
   
    //PITCH轴PID
    PID_FF_Init(&Gimbal_Pitch_FF,3.0f,0.0f,0.0f,1.0,-DJI_M3508_R,DJI_M3508_R,-10.0f, 10.0f);
	PID_Init(&Gimbal_Pitch_In,1.1f,0.0f,0.0f,-2000,2000,-10.0f, 10.0f);
	PID_Init(&Gimbal_Pitch_Ex,600.0f,0.01f,1500.0f,-2800,2800,-1000.0f, 1000.0f);
}

//更新状态函数
void Gimbal_Update(void)
{

    //获取控制命令数据结构体指针
    Gimbal_Instance.CMD = *CMD_Get_point();

    //获取IMU数据数组指针
    Gimbal_Instance.INS_angle = IMU_Get_point();

    //获取IMU当前角度
    Gimbal_Instance.Calc.Yaw.C_Angle = GYRO_YAW(MyMath_Radians_To_Degrees(Gimbal_Instance.INS_angle[GIMBAL_IMU_INDEX_YAW]));
    Gimbal_Instance.Calc.Pitch.C_Angle = GYRO_PITCH(MyMath_Radians_To_Degrees(Gimbal_Instance.INS_angle[GIMBAL_IMU_INDEX_PITCH]));
    
    Gimbal_Instance.Calc.Pitch.C_Angle = MyMath_Limit_Float(
                                              Gimbal_Instance.Calc.Pitch.C_Angle,   //注意陀螺仪极性！
                                             -GIMBAL_PITCH_MAX_DEP,GIMBAL_PITCH_MAX_ELE,0);

    //获取电机反馈数据指针
    Gimbal_Instance.MotorData.Ptr = MotorCtrl_DJI_GetDJI_MFeedback(&GIMBAL_CAN_CTRL);
    
    //更新电机数据
    Gimbal_Instance.MotorData.Yaw = Gimbal_Instance.MotorData.Ptr[GIMBAL_MOTOR_ID_FBK_YAW];
    Gimbal_Instance.MotorData.Pitch = Gimbal_Instance.MotorData.Ptr[GIMBAL_MOTOR_ID_FBK_PITCH];

    //获取自瞄控制数据
#if(AUTOAIM_IFOPEN)

        Gimbal_Instance.Auto.Aim = *AutoAim_Ctrl_Get_point();

#endif

}
//异常处理函数
void Gimbal_HandleError(void)
{

}
//设置模式
void Gimbal_SetMode(void)
{

}
//更新目标量
void Gimbal_RefreshTarget(void)
{
    if (Gimbal_Instance.CMD.ctrl == STOP_MODE)
    {
        Gimbal_SetSafe_Vel();
    }
    else
    {
        Gimbal_Update_Target();
    }
}
//计算控制量
void Gimbal_CtrlCalc(void)
{
    Gimbal_YawStable_Calc();
    Gimbal_PitchStable_Calc();
}
//发送控制指令
void Gimbal_SendCmd(void)
{
    // 静态变量：显式初始化，记录停止状态与起始时间
    static uint32_t stop_start_time = 0;
    static bool is_stopping = false;

    // 获取当前系统时间(ms)
    uint32_t now_time = HAL_GetTick();

    if (Gimbal_Instance.CMD.ctrl == STOP_MODE)
    {
			
        // 首次进入停止模式，记录时间
        if (!is_stopping)
        {
            stop_start_time = now_time;
            is_stopping = true;
        }
        // 无溢出安全判断：急停周期内执行紧急停止
        if ((now_time - stop_start_time) < DJI_MOTOR_STOP_TIME_MS)
        {
            // 底盘电机急停
	        int16_t PIDSTOPYAW  = PID_Calculate(&Gimbal_Motor_STOP,Gimbal_Instance.MotorData.Yaw.speed_rpm,0);
            int16_t PIDSTOPITCH = PID_Calculate(&Gimbal_Motor_STOP,Gimbal_Instance.MotorData.Pitch.speed_rpm,0);
	        ESC_Control_Raw_Single(&GIMBAL_CAN_CTRL,GIMBAL_CAN_ID_YAW,PIDSTOPYAW);
	        ESC_Control_Raw_Single(&GIMBAL_CAN_CTRL,GIMBAL_CAN_ID_PITCH,PIDSTOPITCH);
        }
        else
        {
            // 急停超时后：发送零速度指令保持电机锁定（修复失控BUG）
            ESC_Control_Raw_Single(&GIMBAL_CAN_CTRL, GIMBAL_CAN_ID_YAW, 0);
            ESC_Control_Raw_Single(&GIMBAL_CAN_CTRL, GIMBAL_CAN_ID_PITCH, 0);
        }
    }
    else
    {
        // 退出停止模式，重置状态
        is_stopping = false;

        // 正常模式：发送速度控制指令
        ESC_Control_Raw_Single(&GIMBAL_CAN_CTRL, GIMBAL_CAN_ID_YAW, Gimbal_Instance.Calc.Yaw.Ctrl_Vel);
        ESC_Control_Raw_Single(&GIMBAL_CAN_CTRL, GIMBAL_CAN_ID_PITCH, Gimbal_Instance.Calc.Pitch.Ctrl_Vel);
    }
}

//** #################################################################################################### **//
//** ========================================= 对内算法函数 ============================================== **//
//** #################################################################################################### **//

static void Gimbal_Update_Target(void)
{
#if(AUTOAIM_IFOPEN == AUTOAIM_OPEN)

    /* 更新云台Yaw Pitch角度的目标值 */
    float ManualYaw = MyMath_Map_Range_Float(Gimbal_Instance.CMD.Gimbal.Yaw,-CMD_CTRL_RANGE,CMD_CTRL_RANGE,-GIMBAL_MAX_ANGLE_STEP_DEG_YAW,GIMBAL_MAX_ANGLE_STEP_DEG_YAW);
    float AutoYaw   = Gimbal_Instance.Auto.Aim.Yaw;

    float ManualPitch = MyMath_Map_Range_Float(Gimbal_Instance.CMD.Gimbal.Pitch,-CMD_CTRL_RANGE,CMD_CTRL_RANGE,-GIMBAL_MAX_ANGLE_STEP_DEG_PITCH,GIMBAL_MAX_ANGLE_STEP_DEG_PITCH);
    float AutoPitch   = Gimbal_Instance.Auto.Aim.Pitch;

    float FusionYaw = AutoAim_WeightFusion_Float(ManualYaw,
                                              AutoYaw,
                                              Gimbal_Instance.Auto.Aim.IsOnline,
                                             -GIMBAL_MAX_ANGLE_STEP_DEG_YAW,
                                              GIMBAL_MAX_ANGLE_STEP_DEG_YAW);

    float FusionPitch = AutoAim_WeightFusion_Float(ManualPitch,
                                              AutoPitch,
                                              Gimbal_Instance.Auto.Aim.IsOnline,
                                             -GIMBAL_MAX_ANGLE_STEP_DEG_PITCH,
                                              GIMBAL_MAX_ANGLE_STEP_DEG_PITCH);

    if(Gimbal_Instance.CMD.Auto.Aim == AUTOAIM_ON)
    {
        Gimbal_Instance.Calc.Yaw.T_Angle = Gimbal_Instance.Calc.Yaw.T_Angle - FusionYaw;
        Gimbal_Instance.Calc.Pitch.T_Angle = Gimbal_Instance.Calc.Pitch.T_Angle - FusionPitch;
    }
    else
    {
        Gimbal_Instance.Calc.Yaw.T_Angle = Gimbal_Instance.Calc.Yaw.T_Angle - ManualYaw;
        Gimbal_Instance.Calc.Pitch.T_Angle = Gimbal_Instance.Calc.Pitch.T_Angle - ManualPitch;
    }
        
    //限幅角度-180 ~ 180 循环模式
    Gimbal_Instance.Calc.Yaw.T_Angle = MyMath_Limit_Float(
                                            Gimbal_Instance.Calc.Yaw.T_Angle,
                                            -180.00f,180.00f,1);

    //限幅俯仰角，非循环模式
    Gimbal_Instance.Calc.Pitch.T_Angle = MyMath_Limit_Float(
                                            Gimbal_Instance.Calc.Pitch.T_Angle,
                                            -GIMBAL_PITCH_MAX_DEP,GIMBAL_PITCH_MAX_ELE,0);

#endif

#if(AUTOAIM_IFOPEN == AUTOAIM_NOPEN)

    /* 更新云台Yaw Pitch角度的目标值 */
    float AddYaw = Gimbal_Instance.CMD.Gimbal.Yaw;
    Gimbal_Instance.Calc.Yaw.T_Angle = Gimbal_Instance.Calc.Yaw.T_Angle
                                        - MyMath_Map_Range_Float(AddYaw,-CMD_CTRL_RANGE,CMD_CTRL_RANGE,-GIMBAL_MAX_ANGLE_STEP_DEG_YAW,GIMBAL_MAX_ANGLE_STEP_DEG_YAW);
    //限幅角度-180 ~ 180 循环模式
    Gimbal_Instance.Calc.Yaw.T_Angle = MyMath_Limit_Float(
                                            Gimbal_Instance.Calc.Yaw.T_Angle,
                                            -180.00f,180.00f,1);

    float AddPitch = Gimbal_Instance.CMD.Gimbal.Pitch;
    Gimbal_Instance.Calc.Pitch.T_Angle = Gimbal_Instance.Calc.Pitch.T_Angle
                                        - MyMath_Map_Range_Float(AddPitch,-CMD_CTRL_RANGE,CMD_CTRL_RANGE,-GIMBAL_MAX_ANGLE_STEP_DEG_PITCH,GIMBAL_MAX_ANGLE_STEP_DEG_PITCH);
    //限幅俯仰角，非循环模式
    Gimbal_Instance.Calc.Pitch.T_Angle = MyMath_Limit_Float(
                                            Gimbal_Instance.Calc.Pitch.T_Angle,
                                            -GIMBAL_PITCH_MAX_DEP,GIMBAL_PITCH_MAX_ELE,0);

#endif

}

static void Gimbal_SetSafe_Vel(void)
{
    Gimbal_Instance.Calc.Yaw.T_Angle = Gimbal_Instance.Calc.Yaw.C_Angle;
    Gimbal_Instance.Calc.Pitch.T_Angle = Gimbal_Instance.Calc.Pitch.C_Angle;
}

static void Gimbal_YawStable_Calc(void)
{

    #if(PID_CTRL_MODE_YAW == PID_CTRL_MODE_SINGLE_FF)

    Gimbal_Instance.Calc.Yaw.Ctrl_Vel = PID_FF_Calculate_CycleAngle(
                                            &Gimbal_Yaw_FF,
                                            Gimbal_Instance.Calc.Yaw.C_Angle,
                                            Gimbal_Instance.Calc.Yaw.T_Angle);

    #endif

    #if(PID_CTRL_MODE_YAW == PID_CTRL_MODE_DOUBLE_LOOP)

    Gimbal_Instance.Calc.Yaw.Ctrl_Vel = PID_Double_CycleAngle(
                                            &Gimbal_Yaw_In,
                                            &Gimbal_Yaw_Ex,
                                            Gimbal_Instance.Calc.Yaw.T_Angle,
                                            Gimbal_Instance.MotorData.Yaw.speed_rpm,
                                            Gimbal_Instance.Calc.Yaw.C_Angle,
                                            GIMBAL_PID_THRESHOLD);

    #endif

}

static void Gimbal_PitchStable_Calc(void)
{

    #if(PID_CTRL_MODE_PITCH == PID_CTRL_MODE_SINGLE_FF)
    
    Gimbal_Instance.Calc.Pitch.Ctrl_Vel = PID_FF_Calculate_CycleAngle(
                                            &Gimbal_Pitch_FF,
                                            Gimbal_Instance.Calc.Pitch.C_Angle,
                                            Gimbal_Instance.Calc.Pitch.T_Angle);
    #endif

    #if(PID_CTRL_MODE_PITCH == PID_CTRL_MODE_DOUBLE_LOOP)
    Gimbal_Instance.Calc.Pitch.Ctrl_Vel = PID_Double_CycleAngle(
                                            &Gimbal_Pitch_In,
                                            &Gimbal_Pitch_Ex,
                                            Gimbal_Instance.Calc.Pitch.T_Angle,
                                            Gimbal_Instance.MotorData.Pitch.speed_rpm,
                                            Gimbal_Instance.Calc.Pitch.C_Angle,
                                            GIMBAL_PID_THRESHOLD);
    #endif

}


#endif

#endif
