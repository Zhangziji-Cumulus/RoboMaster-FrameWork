#ifndef A_MCOMMON_H_
#define A_MCOMMON_H_

/* #include "A_MCommon.h" */

#include "A_CommonSystem.h"
#include "Dual_Board_Transmit.h"

//** #################################################################################################### **//
//** ========================================= 功能层头文件 ============================================== **//
//** #################################################################################################### **//

#include "IMU_Get.h"
#include "PID_Lib.h"        //PID控制算法库
#include "Math_Lib.h"       //数学工具库
#include "CMDCenter.h"      //控制命令中心
#include "Remote_Ctrl.h"    //遥控器数据处理
#include "sound_effects_task.h" //蜂鸣器音效


//电机模块
#include "MotorCtrl_DJI.h"      //DJI电机控制协议
#include "MotorCtrl_ZDT.h"      //ZDT电机控制协议

//ZDT 电机控制
#include "ZDT_X_V2.h"
#include "ZDT_CAN_Bsp.h"

//** #################################################################################################### **//
//** ========================================= 应用层头文件 ============================================== **//
//** #################################################################################################### **//

#include "APP_Config.h"
#include "Auto_Config.h"
#include "AutoAim.h"
#include "Robot_Hero_Config.h"
#include "Chassis_Advanced.h"

#endif // A_MCOMMON_H_
