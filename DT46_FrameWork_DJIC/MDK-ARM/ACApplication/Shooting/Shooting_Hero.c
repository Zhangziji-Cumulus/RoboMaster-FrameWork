#include "Shooting_Hero.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == GIMBAL_BOARD ))

#if(ROBOT_TYPE == ROBOTTYPE_HERO)

//extern BoardTransmit_Gimbal_RX_t BoardGRX;

//** #################################################################################################### **//
//** ====================================== 定义数据、结构体 ============================================= **//
//** #################################################################################################### **//

static Shooting_Instance_t Shooting_Instance;
Shooting_State_Machine_t Shooting_State_Machine;

PID_HandleTypeDef PID_SFri_STOP;

PID_HandleTypeDef PID_SFri_UL_In;
PID_HandleTypeDef PID_SFri_UL_Ex;

PID_HandleTypeDef PID_SFri_UR_In;
PID_HandleTypeDef PID_SFri_UR_Ex;

PID_HandleTypeDef PID_SFri_DM_In;
PID_HandleTypeDef PID_SFri_DM_Ex;

//** #################################################################################################### **//
//** ========================================= 对内函数声明 ============================================== **//
//** #################################################################################################### **//

static void Friction_Update_Target(void);
static void PuahRod_Update_Target(void);

void Fire_Run(void);
void Load_Run(void);

//** #################################################################################################### **//
//** ====================================== 对外若定义覆盖函数 =========================================== **//
//** #################################################################################################### **//

//初始化函数
void Shooting_Init(void)
{
    //初始模式STOP
    Shooting_Instance.CMD.ctrl = STOP_MODE;

    //3508电机急停
    PID_Init(&PID_SFri_STOP,3.0f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-5.0f, 5.0f);

	//摩擦轮PID
	PID_Init(&PID_SFri_UL_In,1.0f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-10.0f, 10.0f);
	PID_Init(&PID_SFri_UL_Ex,30.0f,0.0f,0.0f,-10000,10000,-10.0f, 10.0f);
	
	PID_Init(&PID_SFri_UR_In,1.0f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-10.0f, 10.0f);
	PID_Init(&PID_SFri_UR_Ex,30.0f,0.0f,0.0f,-10000,10000,-10.0f, 10.0f);
	
	PID_Init(&PID_SFri_DM_In,1.0f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-10.0f, 10.0f);
	PID_Init(&PID_SFri_DM_Ex,30.0f,0.0f,0.0f,-10000,10000,-10.0f, 10.0f);
	
    Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSITION_L_DEG;
    Shooting_Instance.Calc.PushRod.CtlFlag = 1;
    Shooting_State_Machine.Push_Stroke == PUSHPL;
    
    //Shooting_Instance.Calc.PushRod.State = PUSH_FRONT_ING;

	//X_V2_Auto_Return_Sys_Params_Timed(1,S_CPHA,15);//读取张大头步进电机的实际工作电流
}

//更新状态函数
void Shooting_Update(void)
{
    // static uint8_t ZDT_Time_Count = 0;

    // if(ZDT_Time_Count < 10 / SHOOTING_TASK_TIME_MS)
    // {
    //     X_V2_Read_System_State_Params(1);
    //     ZDT_Time_Count = 0;
    // }
    // ZDT_Time_Count++;

    Shooting_Instance.CMD = *CMD_Get_point();
    Shooting_Instance.DJI_Motordata.Ptr = MotorCtrl_DJI_GetDJI_MFeedback(&FRICTION_CAN_CTRL);
    Shooting_Instance.DJI_Motordata.UL = Shooting_Instance.DJI_Motordata.Ptr[FRICTION_MOTOR_ID_FBK_UL];
    Shooting_Instance.DJI_Motordata.UR = Shooting_Instance.DJI_Motordata.Ptr[FRICTION_MOTOR_ID_FBK_UR];
    Shooting_Instance.DJI_Motordata.DM = Shooting_Instance.DJI_Motordata.Ptr[FRICTION_MOTOR_ID_FBK_DM];

    //更新开火标志位
    Shooting_State_Machine.Fired_Flag = Shooting_Instance.CMD.Shooting.Fire;
}         

//异常处理函数
void Shooting_HandleError(void)
{

}

//设置模式
void Shooting_SetMode(void)
{
    
}

//更新目标量
void Shooting_RefreshTarget(void)
{

    Fire_Run();
    Load_Run();
    Friction_Update_Target();
    // PuahRod_Update_Target();
}

//计算控制量
void Shooting_CtrlCalc(void)
{
    Shooting_Instance.Calc.Friction.UL.Ctrl_Vel = 
                                    PID_Double_Calculate(&PID_SFri_UL_In,
                                                         &PID_SFri_UL_Ex,
                                                         Shooting_Instance.Calc.Friction.UL.T_rpm,
                                                         Shooting_Instance.DJI_Motordata.UL.current_ma,
                                                         Shooting_Instance.DJI_Motordata.UL.speed_rpm,
                                                         FRICTION_PID_THRESHOLD);

    Shooting_Instance.Calc.Friction.UR.Ctrl_Vel = 
                                    PID_Double_Calculate(&PID_SFri_UR_In,
                                                         &PID_SFri_UR_Ex,
                                                         Shooting_Instance.Calc.Friction.UR.T_rpm,
                                                         Shooting_Instance.DJI_Motordata.UR.current_ma,
                                                         Shooting_Instance.DJI_Motordata.UR.speed_rpm,
                                                         FRICTION_PID_THRESHOLD);

    Shooting_Instance.Calc.Friction.DM.Ctrl_Vel = 
                                    PID_Double_Calculate(&PID_SFri_DM_In,
                                                         &PID_SFri_DM_Ex,
                                                         Shooting_Instance.Calc.Friction.DM.T_rpm,
                                                         Shooting_Instance.DJI_Motordata.DM.current_ma,
                                                         Shooting_Instance.DJI_Motordata.DM.speed_rpm,
                                                         FRICTION_PID_THRESHOLD);
}

//发送控制指令
void Shooting_SendCmd(void)
{
    //ZDT使能标志
    static uint8_t ZDT_Enable_Sate = 0;

    // 静态变量：显式初始化，记录停止状态与起始时间
    static uint32_t stop_start_time = 0;
    static bool is_stopping = false;

    // 获取当前系统时间(ms)
    uint32_t now_time = HAL_GetTick();

    if(Shooting_Instance.CMD.ctrl == STOP_MODE)
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
			PIDSTOP[FRICTION_MOTOR_ID_FBK_UL] = PID_Calculate(&PID_SFri_STOP,Shooting_Instance.DJI_Motordata.UL.speed_rpm,0);
            PIDSTOP[FRICTION_MOTOR_ID_FBK_UR] = PID_Calculate(&PID_SFri_STOP,Shooting_Instance.DJI_Motordata.UR.speed_rpm,0);
            PIDSTOP[FRICTION_MOTOR_ID_FBK_DM] = PID_Calculate(&PID_SFri_STOP,Shooting_Instance.DJI_Motordata.DM.speed_rpm,0);
           	
            ESC_Control_Raw_Group(&FRICTION_CAN_CTRL,FRICTION_CAN_GROUP,PIDSTOP);
		}
        else
        {
            // 急停超时后：发送零速度指令保持电机锁定（修复失控BUG）
            PIDSTOP[0] = 0;	
			PIDSTOP[1] = 0;
			PIDSTOP[2] = 0;
			PIDSTOP[3] = 0;
			ESC_Control_Raw_Group(&FRICTION_CAN_CTRL,FRICTION_CAN_GROUP,PIDSTOP);
        }

        if(ZDT_Enable_Sate <= 10)
        {
            //失能张大头电机
            X_V2_En_Control(1,false,false);
            ZDT_Enable_Sate++;
        }

    }
    else
    {
        // 退出停止模式，重置状态
        is_stopping = false;

        if(ZDT_Enable_Sate >= 10)
        {
            X_V2_En_Control(1,true,false);
						ZDT_Enable_Sate = 0;
        }

        int16_t PIDoutput[4] = {0};

        PIDoutput[FRICTION_MOTOR_ID_FBK_UL] = Shooting_Instance.Calc.Friction.UL.Ctrl_Vel;
        PIDoutput[FRICTION_MOTOR_ID_FBK_UR] = Shooting_Instance.Calc.Friction.UR.Ctrl_Vel;
        PIDoutput[FRICTION_MOTOR_ID_FBK_DM] = Shooting_Instance.Calc.Friction.DM.Ctrl_Vel;

        ESC_Control_Raw_Group(&FRICTION_CAN_CTRL, FRICTION_CAN_GROUP, PIDoutput);


        // if(Shooting_Instance.Calc.PushRod.State = PUSH_BACK_ENTER)
        // {
        //     X_V2_Traj_Pos_LC_Control(PUSHROD_CAN_ID,
        //                             PUSHROD_CCW,
        //                             PUSHROD_ACC,
        //                             PUSHROD_DEC,
        //                             PUSHROD_MAX_SPEED_RPM,
        //                             Shooting_Instance.Calc.PushRod.T_Angle,
        //                             PUSHROD_POS_MODE_ABSOLUTE,
        //                             false,
        //                             PUSHROD_CURRENT_MAX);
        //     Shooting_Instance.Calc.PushRod.State = PUSH_BACK_ING;
        // }
        // else if(Shooting_Instance.Calc.PushRod.State = PUSH_FRONT_ENTER)
        // {
        //     X_V2_Traj_Pos_LC_Control(PUSHROD_CAN_ID,
        //                             PUSHROD_CCW,
        //                             PUSHROD_ACC,
        //                             PUSHROD_DEC,
        //                             PUSHROD_MAX_SPEED_RPM,
        //                             Shooting_Instance.Calc.PushRod.T_Angle,
        //                             PUSHROD_POS_MODE_ABSOLUTE,
        //                             false,
        //                             PUSHROD_CURRENT_MAX);                 
        //     Shooting_Instance.Calc.PushRod.State = PUSH_FRONT_ING;
        // }

        if(Shooting_Instance.Calc.PushRod.CtlFlag)
        {
            X_V2_Traj_Pos_LC_Control(PUSHROD_CAN_ID,
                                     PUSHROD_CCW,
                                     PUSHROD_ACC,
                                     PUSHROD_DEC,
                                     PUSHROD_MAX_SPEED_RPM,
                                     Shooting_Instance.Calc.PushRod.T_Angle,
                                     PUSHROD_POS_MODE_ABSOLUTE,
                                     false,
                                     PUSHROD_CURRENT_MAX);

            Shooting_Instance.Calc.PushRod.CtlFlag = 0;
        }
    }
}

//** #################################################################################################### **//
//** ========================================= 对内算法函数 ============================================== **//
//** #################################################################################################### **//

static void Friction_Update_Target(void)
{
    if(Shooting_Instance.CMD.Shooting.Friction == ON)
    {

        Shooting_Instance.Calc.Friction.ShootingSpeed = -FRICTION_MAX_SPEED_M_S;

        Shooting_Instance.Calc.Friction.UL.T_rpm = (int16_t)calc_motor_rpm_from_speed(Shooting_Instance.Calc.Friction.ShootingSpeed,(FRICTION_RADIUS_MM / 1000),FRICTION_RATIO);
        Shooting_Instance.Calc.Friction.UR.T_rpm = (int16_t)calc_motor_rpm_from_speed(Shooting_Instance.Calc.Friction.ShootingSpeed,(FRICTION_RADIUS_MM / 1000),FRICTION_RATIO);
        Shooting_Instance.Calc.Friction.DM.T_rpm = (int16_t)calc_motor_rpm_from_speed(Shooting_Instance.Calc.Friction.ShootingSpeed,(FRICTION_RADIUS_MM / 1000),FRICTION_RATIO);
        
    }
    else
    {
        Shooting_Instance.Calc.Friction.UL.T_rpm = 0;
        Shooting_Instance.Calc.Friction.UR.T_rpm = 0;
        Shooting_Instance.Calc.Friction.DM.T_rpm = 0;
    }
}

static void PuahRod_Update_Target(void)
{

    if((Shooting_Instance.CMD.Shooting.Fire == ON) && (Shooting_Instance.Calc.PushRod.State == PUSH_FRONT_ING))
    {
        Shooting_Instance.Calc.PushRod.State = PUSH_BACK_ENTER;

        Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSTION_FRONT_DEG;
    }
    else if((Shooting_Instance.CMD.Shooting.Fire == OFF) && (Shooting_Instance.Calc.PushRod.State == PUSH_BACK_ING))
    {
        Shooting_Instance.Calc.PushRod.State = PUSH_FRONT_ENTER;

        Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSTION_BACK_DEG;
    }
}



// //点L位置
// #define PUSHROD_POSITION_L_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_L_MM,PUSHROD_SCREW_LEAD_MM) 
// //点A位置
// #define PUSHROD_POSITION_A_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_A_MM,PUSHROD_SCREW_LEAD_MM) 
// //点B位置
// #define PUSHROD_POSITION_B_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_B_MM,PUSHROD_SCREW_LEAD_MM) 
// //点C位置
// #define PUSHROD_POSITION_C_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_C_MM,PUSHROD_SCREW_LEAD_MM)   

//检测是否发弹
uint8_t Detect_Fied(void)
{
    static uint16_t shoot_cnt = 0;
    const int16_t CURRENT_THRESHOLD = 2000;

    int32_t sum = Shooting_Instance.DJI_Motordata.DM.current_ma +
                  Shooting_Instance.DJI_Motordata.UL.current_ma +
                  Shooting_Instance.DJI_Motordata.UR.current_ma;

    int16_t I_avg = sum / 3;

    //条件：平均电流超限 或者 任意一路电机电流超限
    if(I_avg > CURRENT_THRESHOLD
        || Shooting_Instance.DJI_Motordata.DM.current_ma > CURRENT_THRESHOLD
        || Shooting_Instance.DJI_Motordata.UL.current_ma > CURRENT_THRESHOLD
        || Shooting_Instance.DJI_Motordata.UR.current_ma > CURRENT_THRESHOLD)
    {
        shoot_cnt++;
    }
    else
    {
        shoot_cnt = 0;
    }

    //连续5帧超标才确认击发，消除尖峰毛刺
    if(shoot_cnt >= 5)
    {
        shoot_cnt = 5; //防止计数溢出
        return 1;
    }
    else
    {
        return 0;
    }
}

 //与填弹逻辑有关
 void Load_Run(void)
 {
     static uint32_t start_tick = 0;

     if(Shooting_State_Machine.Push_Stroke == PUSHPL)
     {
        //发送开始填弹标志
        Dual_Board_TX_Set_LoadStartFlag(1);
     }
		 
		 
     if(BoardGRX.LoadEndFlag == 1)
     {
				 Dual_Board_TX_Set_LoadStartFlag(0);//发送清零指令
			 
         //设置推杆位置到A点
         Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSITION_A_DEG;
         Shooting_Instance.Calc.PushRod.CtlFlag = 1;

         //等待A点到位
         if(start_tick == 0)
         {
            start_tick = HAL_GetTick();
         }

         //时间窗口
         if(HAL_GetTick() - start_tick <= 500)
         {
            Shooting_State_Machine.Push_Stroke = PUSHPA;
            Shooting_State_Machine.Fire = FIRE_READY;
            start_tick = 0;
         }
     }
 }



/************************* 时间参数配置（按实际调试修改） *************************/
#define TIME_A_TO_B        1000    // A→B 推杆运动时间(ms)
#define TIME_B_DETECT      1000    // B点发射检测超时时间(ms)
#define TIME_B_TO_C        1000    // B→C 推杆运动时间(ms)
#define TIME_C_TO_L        1000    // C→L 推杆返回时间(ms)
#define FIRE_FILTER_MS     300     // 开火信号消抖时间(ms)

/************************* 状态机主函数 *************************/
void Fire_Run(void)
{
    static Fire_State_e fire_state = FIRE_STA_IDLE;
    static uint32_t state_tick = 0;         // 单状态计时变量，切换状态时重置
    static uint32_t fire_filter_tick = 0;   // 开火信号消抖计时
    static uint8_t  last_raw_fire = 0;      // 上一帧原始开火电平
    uint8_t  fire_pulse = 0;                // 消抖后的触发脉冲（仅触发瞬间为1）

    uint8_t current_raw_fire = Shooting_State_Machine.Fired_Flag;

    /****************************************************************************
    * 第一部分：开火信号上升沿消抖
    * 仅0→1上升沿启动计时，持续高电平达标后产生1个触发脉冲
    * 长按不会重复触发，松开后再次按下才会产生新脉冲
    ***************************************************************************/
    if(current_raw_fire == 1)
    {
        if(last_raw_fire == 0)
        {
            fire_filter_tick = HAL_GetTick();
        }
        // 消抖时间到，且未产生过脉冲 → 输出单次触发脉冲
        else if( (fire_filter_tick != 0) 
              && (HAL_GetTick() - fire_filter_tick >= FIRE_FILTER_MS) )
        {
            fire_pulse = 1;
            fire_filter_tick = 0; // 锁死，避免持续高电平重复触发
        }
    }
    else
    {
        fire_filter_tick = 0; // 低电平直接复位消抖状态
    }
    last_raw_fire = current_raw_fire;


    /****************************************************************************
    * 第二部分：核心发射状态机
    * 严格按照 A→B→(检测)→(待命)→C→L 流程流转
    ***************************************************************************/
    switch(fire_state)
    {
        /*---------- 待机状态：在A点等待第一次开火 ----------*/
        case FIRE_STA_IDLE:
        {
            // 仅当推杆在A点 + 系统就绪 + 收到开火脉冲时，启动流程
            if( (Shooting_State_Machine.Push_Stroke == PUSHPA)
             && (Shooting_State_Machine.Fire == FIRE_READY)
             && (fire_pulse == 1) )
            {
                // 设定B点目标角度，启动电机
                Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSITION_B_DEG;
                Shooting_Instance.Calc.PushRod.CtlFlag = 1;

                state_tick = HAL_GetTick();
                fire_state = FIRE_STA_MOVE_A2B;
            }
        }
        break;

        /*---------- 推杆移动：A → B ----------*/
        case FIRE_STA_MOVE_A2B:
        {
            // 运动时间到 → 视为B点到位
            if(HAL_GetTick() - state_tick >= TIME_A_TO_B)
            {
                Shooting_State_Machine.Push_Stroke = PUSHPB; // 更新位置状态
                
                state_tick = HAL_GetTick();  // 重置计时，用于发射检测超时
                fire_state = FIRE_STA_B_DETECT;
            }
        }
        break;

        /*---------- B点：发射检测分支 ----------*/
        case FIRE_STA_B_DETECT:
        {
            // 分支1：检测到发射成功
            if(Detect_Fied() != 0)
            {
                Shooting_State_Machine.Fired_Count = 1;
                Shooting_State_Machine.Fire = FIRE_END_B;
                
                fire_state = FIRE_STA_B_READY; // 进入B点待发射，等待下一次开火
            }
            // 分支2：超时未检测到发射 → 自动前往C点归位
            else if(HAL_GetTick() - state_tick >= TIME_B_DETECT)
            {
                Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSITION_C_DEG;
                Shooting_Instance.Calc.PushRod.CtlFlag = 1;

                state_tick = HAL_GetTick();
                fire_state = FIRE_STA_MOVE_B2C;
            }
        }
        break;

        /*---------- B点待发射：等待第二次开火指令 ----------*/
        case FIRE_STA_B_READY:
        {
            // 收到第二次开火脉冲 → 前往C点
            if(fire_pulse == 1)
            {
                Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSITION_C_DEG;
                Shooting_Instance.Calc.PushRod.CtlFlag = 1;

                state_tick = HAL_GetTick();
                fire_state = FIRE_STA_MOVE_B2C;
            }
        }
        break;

        /*---------- 推杆移动：B → C ----------*/
        case FIRE_STA_MOVE_B2C:
        {
            // 运动时间到 → 视为C点到位
            if(HAL_GetTick() - state_tick >= TIME_B_TO_C)
            {
                Shooting_State_Machine.Push_Stroke = PUSHPC;
                Shooting_State_Machine.Fire = FIRE_END_C;

                // 到位后自动开始返回L点
                Shooting_Instance.Calc.PushRod.T_Angle = PUSHROD_POSITION_L_DEG;
                Shooting_Instance.Calc.PushRod.CtlFlag = 1;

                state_tick = HAL_GetTick();
                fire_state = FIRE_STA_MOVE_C2L;
            }
        }
        break;

        /*---------- 推杆返回：C → L ----------*/
        case FIRE_STA_MOVE_C2L:
        {
            // 返回时间到 → 视为L点到位，全流程复位
            if(HAL_GetTick() - state_tick >= TIME_C_TO_L)
            {
                Shooting_State_Machine.Push_Stroke = PUSHPL;
                Shooting_State_Machine.Fired_Count = 0;
                Shooting_State_Machine.Fire = FIRE_READY; // 恢复就绪态

                state_tick = 0;
                fire_state = FIRE_STA_IDLE; // 回到待机，等待外部送弹到A点
            }
        }
        break;

        /*---------- 异常保护：未知状态强制复位 ----------*/
        default:
        {
            fire_state = FIRE_STA_IDLE;
            state_tick = 0;
        }
        break;
    }
}

#endif

#endif

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == CHASSIS_BOARD ))

#if(ROBOT_TYPE == ROBOTTYPE_HERO)

//** #################################################################################################### **//
//** ====================================== 定义数据、结构体 ============================================= **//
//** #################################################################################################### **//

static Shooting_Instance_t Shooting_Instance;
Shooting_State_Machine_t Shooting_State_Machine;

PID_HandleTypeDef Dial_Motor_STOP;

PID_HandleTypeDef Dial_In;
PID_HandleTypeDef Dial_Ex;

//** #################################################################################################### **//
//** ========================================= 对内函数声明 ============================================== **//
//** #################################################################################################### **//

static void Dial_Update_Target(void);
static void Auto_Load(void);

//** ====================================== 对外若定义覆盖函数 =========================================== **//
//** #################################################################################################### **//

//初始化函数
void Shooting_Init(void)
{
    Shooting_State_Machine.Load = LOAD_WAIT;

    //初始模式STOP
    Shooting_Instance.CMD.ctrl = STOP_MODE;
    
    // //初始化拨盘状态
    // Shooting_Instance.Logic.LoadState = LOAD_STOP;

    PID_Init(&Dial_Motor_STOP,3.0f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-5.0f, 5.0f);

	//拨盘PID
	PID_Init(&Dial_In,1.1f,0.0f,0.0f,-DJI_M3508_R,DJI_M3508_R,-10.0f, 10.0f);
	//PID_Init(&Dial_Ex,50.0f,0.003f,50.0f,-10000,10000,-1000.0f, 1000.0f);
    PID_Init(&Dial_Ex,10.0f,0.003f,50.0f,-10000,10000,-1000.0f, 1000.0f);

}          

//更新状态函数
void Shooting_Update(void)
{
    //获取控制命令数据结构体指针
    Shooting_Instance.CMD = *CMD_Get_point();

    //获取电机反馈数据指针
    Shooting_Instance.DJI_Motordata.Ptr = MotorCtrl_DJI_GetDJI_MFeedback(&DIAL_CAN_CTRL);
    Shooting_Instance.DJI_Motordata.DIAL = Shooting_Instance.DJI_Motordata.Ptr[DIAL_MOTOR_ID_FBK];

}         

//异常处理函数
void Shooting_HandleError(void)
{

}

//设置模式
void Shooting_SetMode(void)
{

}

//更新目标量
void Shooting_RefreshTarget(void)
{
    Dial_Update_Target();
    Auto_Load();
}

//计算控制量
void Shooting_CtrlCalc(void)
{
    if(Shooting_State_Machine.Load == LOAD_START)
    {
        Shooting_Instance.Calc.Dial.Ctrl_Vel = PID_Double_Calculate(&Dial_In,
                                                                    &Dial_Ex,
                                                                    Shooting_Instance.Calc.Dial.T_rpm,
                                                                    Shooting_Instance.DJI_Motordata.DIAL.current_ma,
                                                                    Shooting_Instance.DJI_Motordata.DIAL.speed_rpm,
                                                                    DIAL_PID_THRESHOLD);
    }
    else
    {
        Shooting_Instance.Calc.Dial.Ctrl_Vel = 0;
    }
}

//发送控制指令
void Shooting_SendCmd(void)
{
    // 静态变量：显式初始化，记录停止状态与起始时间
    static uint32_t stop_start_time = 0;
    static bool is_stopping = false;

    // 获取当前系统时间(ms)
    uint32_t now_time = HAL_GetTick();

    if(Shooting_Instance.CMD.ctrl == STOP_MODE)
    {
        
        if (!is_stopping)
        {
            stop_start_time = now_time;
            is_stopping = true;
        }
        // 无溢出安全判断：急停周期内执行紧急停止
        if ((now_time - stop_start_time) < DJI_MOTOR_STOP_TIME_MS)
        {
            int16_t PIDSTOP = { 0 };

			PIDSTOP = PID_Calculate(&Dial_Motor_STOP,Shooting_Instance.DJI_Motordata.DIAL.speed_rpm,0);
           
			ESC_Control_Raw_Single(&DIAL_CAN_CTRL,DIAL_CAN_ID,PIDSTOP);
		}
        else
        {
            ESC_Control_Raw_Single(&DIAL_CAN_CTRL,DIAL_CAN_ID,0);
        }

    }
    else
    {
        // 退出停止模式，重置状态
        is_stopping = false;

        ESC_Control_Raw_Single(&DIAL_CAN_CTRL,DIAL_CAN_ID,Shooting_Instance.Calc.Dial.Ctrl_Vel);
    }
}

//** #################################################################################################### **//
//** ========================================= 对内算法函数 ============================================== **//
//** #################################################################################################### **//

static void Dial_Update_Target(void)
{
    if(Shooting_State_Machine.Load == LOAD_START)
    {
        Shooting_Instance.Calc.Dial.T_Speed = DIAL_MAX_SPEED_M_S;
        Shooting_Instance.Calc.Dial.T_rpm = (int16_t)calc_motor_rpm_from_speed(Shooting_Instance.Calc.Dial.T_Speed,(DIAL_RADIUS_MM / 1000),DIAL_RATIO);
    }
    else if(Shooting_State_Machine.Load == LOAD_WAIT)
    {
        Shooting_Instance.Calc.Dial.T_rpm = 0;
    }
}

static uint8_t Detect_Load(void)
{
    static uint16_t shoot_cnt = 0;
    const int16_t CURRENT_THRESHOLD = 2000;

    if(fabs(Shooting_Instance.DJI_Motordata.DIAL.current_ma) > CURRENT_THRESHOLD)
    {
        shoot_cnt++;
    }
    else
    {
        shoot_cnt = 0;
    }

    //连续5帧超标才确认击发，消除尖峰毛刺
    if(shoot_cnt >= 5)
    {
        shoot_cnt = 0; //防止计数溢出
        return 1;
    }
    else
    {
        return 0;
    }
}

static void Auto_Load(void)
{
    static uint8_t LoadStartFlag;
    static uint8_t LoadStartFlag_Last;
    static uint32_t LoadStartTick = 0;
    static uint32_t LoadEndTick = 0;
    static uint8_t LoadDelayFlag = 0;
    static uint8_t EndDelayFlag = 0;

    //1. 边沿检测
    LoadStartFlag_Last = LoadStartFlag;
    LoadStartFlag = BoardCRX.LoadStartFlag;

    //只在空闲且延时未开启时，捕获上升沿启动计时
    if(LoadStartFlag && !LoadStartFlag_Last)
    {
        if(Shooting_State_Machine.Load == LOAD_WAIT && LoadDelayFlag == 0) 
        {
            LoadStartTick = HAL_GetTick();
            LoadDelayFlag = 1;
        }
    }

    //延时500ms进入装填
    if(LoadDelayFlag)
    {
        if(HAL_GetTick() - LoadStartTick >= 500)
        {
            Shooting_State_Machine.Load = LOAD_START;
            LoadDelayFlag = 0;
        }
    }

	if(Shooting_State_Machine.Load == LOAD_START)
    { 
        if(Detect_Load())
        {
            Shooting_State_Machine.Load = LOAD_OK;
			Dual_Board_TX_Set_LoadEndFlag(1);
        }
    }
    else if(Shooting_State_Machine.Load == LOAD_OK)
    {
        if(!EndDelayFlag)
        {
            LoadEndTick = HAL_GetTick();
            EndDelayFlag = 1;
        }

        if(HAL_GetTick() - LoadEndTick >= 500)
        {
            Dual_Board_TX_Set_LoadEndFlag(0);
            Shooting_State_Machine.Load = LOAD_WAIT;
            EndDelayFlag = 0;
            LoadDelayFlag = 0; //兜底复位，防止标志卡死
        }
    }
}

#endif

#endif
