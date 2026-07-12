#include "Chassis_Mecanu.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == CHASSIS_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

#if(CHASSIS_TYPE == CHASSIS_MECANUM)

//定义内部数据
static Chassis_Instance_t Chassis_Instance = {0};

PID_HandleTypeDef Chassis_Motor_STOP;

PID_HandleTypeDef Chassis_MotorFL_In;
PID_HandleTypeDef Chassis_MotorFL_Ex;
PID_HandleTypeDef Chassis_MotorFR_In;
PID_HandleTypeDef Chassis_MotorFR_Ex;
PID_HandleTypeDef Chassis_MotorBL_In;
PID_HandleTypeDef Chassis_MotorBL_Ex;
PID_HandleTypeDef Chassis_MotorBR_In;
PID_HandleTypeDef Chassis_MotorBR_Ex;

PID_HandleTypeDef Chassis_Follow_PID;

//** #################################################################################################### **//
//** ========================================= 对内函数声明 ============================================== **//
//** #################################################################################################### **//
static void Chassis_Update_Target(uint8_t state);
static void Chassis_Mecanu_Calc(void);
//** #################################################################################################### **//
//** ====================================== 对外若定义覆盖函数 =========================================== **//
//** #################################################################################################### **//



//初始化函数
void Chassis_Init(void)
{
    //初始模式STOP
    Chassis_Instance.CMD.ctrl = STOP_MODE;

	Chassis_Instance.Calc.Theta.Degree = 0.0f;

    //3508电机急停
    PID_Init(&Chassis_Motor_STOP,3.0f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-5.0f, 5.0f);
	
	//地盘PID初始化
	PID_Init(&Chassis_MotorFL_In,1.2f,0.05f,0.0f,-DJI_M3508_R,DJI_M3508_R,-8000.0f,8000.0f);
	PID_Init(&Chassis_MotorFL_Ex,30.0f,0.1f,0.0f,-5000,5000,-2500.0f,2500.0f);
	
	PID_Init(&Chassis_MotorFR_In,1.2f,0.05f,0.0f,-DJI_M3508_R,DJI_M3508_R,-8000.0f,8000.0f);
	PID_Init(&Chassis_MotorFR_Ex,30.0f,0.1f,0.0f,-5000,5000,-2500.0f,2500.0f);
	
	PID_Init(&Chassis_MotorBL_In,1.2f,0.05f,0.0f,-DJI_M3508_R,DJI_M3508_R,-8000.0f,8000.0f);
	PID_Init(&Chassis_MotorBL_Ex,30.0f,0.1f,0.0f,-5000,5000,-2500.0f,2500.0f);
	
	PID_Init(&Chassis_MotorBR_In,1.2f,0.05f,0.0f,-DJI_M3508_R,DJI_M3508_R,-8000.0f,8000.0f);
	PID_Init(&Chassis_MotorBR_Ex,30.0f,0.1f,0.0f,-5000,5000,-2500.0f,2500.0f);
	
	PID_Init(&Chassis_Follow_PID,0.05f,0.0,0.15f,-CHASSIS_MAX_SPEED_FOLLOWING,CHASSIS_MAX_SPEED_FOLLOWING,-0.75f,0.75f);


    GyroSmoothRand_Init(0.70f,1.20f);
}          

//更新数据函数
void Chassis_Update(void)
{
    //获取控制命令数据结构体指针
    Chassis_Instance.CMD = *CMD_Get_point();

    //获取IMU数据数组指针
    Chassis_Instance.INS_angle = IMU_Get_point();

    //获取电机反馈数据指针
    Chassis_Instance.MotorData.Ptr = MotorCtrl_DJI_GetDJI_MFeedback(&CHASSIS_CAN_CTRL);

    //更新电机数据
    Chassis_Instance.MotorData.W_FL = Chassis_Instance.MotorData.Ptr[CHASSIS_MOTOR_ID_FBK_FL];  // 左前轮
    Chassis_Instance.MotorData.W_FR = Chassis_Instance.MotorData.Ptr[CHASSIS_MOTOR_ID_FBK_FR];  // 右前轮
    Chassis_Instance.MotorData.W_BL = Chassis_Instance.MotorData.Ptr[CHASSIS_MOTOR_ID_FBK_BL];  // 左后轮
    Chassis_Instance.MotorData.W_BR = Chassis_Instance.MotorData.Ptr[CHASSIS_MOTOR_ID_FBK_BR];  // 右后轮

    //地盘和云台的夹角获取
    Chassis_Instance.Calc.Yaw_Angle.Ptr = MotorCtrl_DJI_GetDJI_MFeedback(&CHASSIS_CAN_YAW);
    Chassis_Instance.Calc.Yaw_Angle.YAW = Chassis_Instance.Calc.Yaw_Angle.Ptr[GIMBAL_MOTOR_ID_FBK_YAW];

    //限幅云台的夹角，并且重映射电机零位
    if(Chassis_Instance.Calc.Yaw_Angle.YAW.is_online == 1)
    {
				Chassis_Instance.Calc.Theta.Degree = MyMath_cal_output_angle(Chassis_Instance.Calc.Yaw_Angle.YAW.angle_deg - YAW_ZERO_ANGLE,GIMBAL_YAW_RATIO);//使用函数计算减速比后的地盘、云台角度
    }
        
    //Chassis_Instance.Calc.Theta.Degree = MyMath_normalize_m180_to_p180(Chassis_Instance.Calc.Theta.Degree);//规范角度范围
		
		Chassis_Instance.Calc.Theta.Degree = MyMath_normalize_0_to_360(Chassis_Instance.Calc.Theta.Degree);//规范角度范围
		
    Chassis_Instance.Calc.Theta.Radian = MyMath_Degrees_To_Radians(Chassis_Instance.Calc.Theta.Degree);//弧度制云台地盘角度
}         

//异常处理函数
void Chassis_HandleError(void)
{

}

//设置模式
void Chassis_SetMode(void)
{

}

//更新目标量
void Chassis_RefreshTarget(void)
{
    Chassis_Update_Target(Chassis_Instance.CMD.Move);//根据移动模式来设置：正常/小陀螺CW CCW
    Chassis_Mecanu_Calc();//麦轮解算
}

//计算控制量
void Chassis_CtrlCalc(void)
{
    
    Chassis_Instance.Calc.W_FL.Ctrl_Vel = PID_Double_Calculate(&Chassis_MotorFL_In,
                                                               &Chassis_MotorFL_Ex, 
                                                               Chassis_Instance.Calc.W_FL.T_rpm,
                                                               Chassis_Instance.MotorData.W_FL.current_ma,
                                                               Chassis_Instance.MotorData.W_FL.speed_rpm,
                                                               CHASSIS_PID_THRESHOLD);

    Chassis_Instance.Calc.W_FR.Ctrl_Vel = PID_Double_Calculate(&Chassis_MotorFR_In,
                                                               &Chassis_MotorFR_Ex, 
                                                               Chassis_Instance.Calc.W_FR.T_rpm,
                                                               Chassis_Instance.MotorData.W_FR.current_ma,
                                                               Chassis_Instance.MotorData.W_FR.speed_rpm,
                                                               CHASSIS_PID_THRESHOLD);

    Chassis_Instance.Calc.W_BL.Ctrl_Vel = PID_Double_Calculate(&Chassis_MotorBL_In,
                                                               &Chassis_MotorBL_Ex, 
                                                               Chassis_Instance.Calc.W_BL.T_rpm,
                                                               Chassis_Instance.MotorData.W_BL.current_ma,
                                                               Chassis_Instance.MotorData.W_BL.speed_rpm,
                                                               CHASSIS_PID_THRESHOLD);

    Chassis_Instance.Calc.W_BR.Ctrl_Vel = PID_Double_Calculate(&Chassis_MotorBR_In,
                                                               &Chassis_MotorBR_Ex, 
                                                               Chassis_Instance.Calc.W_BR.T_rpm,
                                                               Chassis_Instance.MotorData.W_BR.current_ma,
                                                               Chassis_Instance.MotorData.W_BR.speed_rpm,
                                                               CHASSIS_PID_THRESHOLD);
}

//发送控制指令
void Chassis_SendCmd(void)
{
    // 静态变量：显式初始化，记录停止状态与起始时间
    static uint32_t stop_start_time = 0;
    static bool is_stopping = false;

    // 获取当前系统时间(ms)
    uint32_t now_time = HAL_GetTick();

    if(Chassis_Instance.CMD.ctrl == STOP_MODE)
    {
        int16_t PIDSTOP[4] = { 0 };
				
        if (!is_stopping)
        {
            stop_start_time = now_time;
            is_stopping = true;
        }
        // 无溢出安全判断：急停周期内执行紧急停止
        if ((now_time - stop_start_time) < DJI_MOTOR_STOP_TIME_MS)
        {
			PIDSTOP[CHASSIS_MOTOR_ID_FBK_FL] = PID_Calculate(&Chassis_Motor_STOP,Chassis_Instance.MotorData.W_FL.speed_rpm,0);
            PIDSTOP[CHASSIS_MOTOR_ID_FBK_FR] = PID_Calculate(&Chassis_Motor_STOP,Chassis_Instance.MotorData.W_FR.speed_rpm,0);
            PIDSTOP[CHASSIS_MOTOR_ID_FBK_BL] = PID_Calculate(&Chassis_Motor_STOP,Chassis_Instance.MotorData.W_BL.speed_rpm,0);
            PIDSTOP[CHASSIS_MOTOR_ID_FBK_BR] = PID_Calculate(&Chassis_Motor_STOP,Chassis_Instance.MotorData.W_BR.speed_rpm,0);

			ESC_Control_Raw_Group(&CHASSIS_CAN_CTRL,CHASSIS_CAN_GROUP,PIDSTOP);
		}
        else
        {
            // 急停超时后：发送零速度指令保持电机锁定（修复失控BUG）
            PIDSTOP[0] = 0;	
			PIDSTOP[1] = 0;
			PIDSTOP[2] = 0;
			PIDSTOP[3] = 0;
			ESC_Control_Raw_Group(&CHASSIS_CAN_CTRL,CHASSIS_CAN_GROUP,PIDSTOP);
        }
				
				
				
    }
    else
    {
        // 退出停止模式，重置状态
        is_stopping = false;

        int16_t PIDoutput[4] = {0};

        PIDoutput[CHASSIS_MOTOR_ID_FBK_FL] = Chassis_Instance.Calc.W_FL.Ctrl_Vel;
        PIDoutput[CHASSIS_MOTOR_ID_FBK_FR] = Chassis_Instance.Calc.W_FR.Ctrl_Vel;
        PIDoutput[CHASSIS_MOTOR_ID_FBK_BL] = Chassis_Instance.Calc.W_BL.Ctrl_Vel;
        PIDoutput[CHASSIS_MOTOR_ID_FBK_BR] = Chassis_Instance.Calc.W_BR.Ctrl_Vel;

        ESC_Control_Raw_Group(&CHASSIS_CAN_CTRL, CHASSIS_CAN_GROUP, PIDoutput);
    }
}      

//** #################################################################################################### **//
//** ========================================= 对内算法函数 ============================================== **//
//** #################################################################################################### **//

float SpinSpeed;

static void Chassis_Update_Target(uint8_t state)
{
    float Theta_Degree = Chassis_Instance.Calc.Theta.Degree;
    float Theta_Radian = Chassis_Instance.Calc.Theta.Radian;

    SpinSpeed = GyroSmoothRand_Run();

    if(state == NORMAL)
    {
        Chassis_Instance.Calc.Target.FB = MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.FB);
        Chassis_Instance.Calc.Target.LR = MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.LR);
        Chassis_Instance.Calc.Target.RO = PID_Calculate_CycleAngle(&Chassis_Follow_PID,Theta_Degree,0.0f); //MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.RO);
    }
    else if(state == SPIN_CW)
    {
        Chassis_Instance.Calc.Target.FB = MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.LR) * sin(Theta_Radian) 
                                                + MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.FB) * cos(Theta_Radian);

        Chassis_Instance.Calc.Target.LR = MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.LR) * cos(Theta_Radian) 
                                                - MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.FB) * sin(Theta_Radian);

        Chassis_Instance.Calc.Target.RO = CHASSIS_MAX_SPIN_SPEED;   //顺时针
    }
    else if(state == SPIN_CCW)
    {
        Chassis_Instance.Calc.Target.FB = MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.LR) * sin(Theta_Radian) 
                                                + MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.FB) * cos(Theta_Radian);

        Chassis_Instance.Calc.Target.LR = MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.LR) * cos(Theta_Radian) 
                                                - MAP_CMD_RANGE_TO_M_S(Chassis_Instance.CMD.Chassis.FB) * sin(Theta_Radian);

        Chassis_Instance.Calc.Target.RO = -CHASSIS_MAX_SPIN_SPEED;  //逆时针
    }
}

static void Chassis_Mecanu_Calc(void)
{
    Chassis_Instance.Calc.W_FL.T_Speed =  (Chassis_Instance.Calc.Target.LR) + (Chassis_Instance.Calc.Target.FB) + (Chassis_Instance.Calc.Target.RO);
    Chassis_Instance.Calc.W_BL.T_Speed = -(Chassis_Instance.Calc.Target.LR) + (Chassis_Instance.Calc.Target.FB) + (Chassis_Instance.Calc.Target.RO);
    Chassis_Instance.Calc.W_FR.T_Speed =  (Chassis_Instance.Calc.Target.LR) - (Chassis_Instance.Calc.Target.FB) + (Chassis_Instance.Calc.Target.RO);
    Chassis_Instance.Calc.W_BR.T_Speed = -(Chassis_Instance.Calc.Target.LR) - (Chassis_Instance.Calc.Target.FB) + (Chassis_Instance.Calc.Target.RO);

    Chassis_Instance.Calc.W_FL.T_rpm = (int16_t)calc_motor_rpm_from_speed(Chassis_Instance.Calc.W_FL.T_Speed,(CHASSIS_WHEEL_RADIUS_MM / 1000),DJI_M3508_RATIO);
    Chassis_Instance.Calc.W_FR.T_rpm = (int16_t)calc_motor_rpm_from_speed(Chassis_Instance.Calc.W_FR.T_Speed,(CHASSIS_WHEEL_RADIUS_MM / 1000),DJI_M3508_RATIO);
    Chassis_Instance.Calc.W_BL.T_rpm = (int16_t)calc_motor_rpm_from_speed(Chassis_Instance.Calc.W_BL.T_Speed,(CHASSIS_WHEEL_RADIUS_MM / 1000),DJI_M3508_RATIO);
    Chassis_Instance.Calc.W_BR.T_rpm = (int16_t)calc_motor_rpm_from_speed(Chassis_Instance.Calc.W_BR.T_Speed,(CHASSIS_WHEEL_RADIUS_MM / 1000),DJI_M3508_RATIO);
}

#endif

#endif