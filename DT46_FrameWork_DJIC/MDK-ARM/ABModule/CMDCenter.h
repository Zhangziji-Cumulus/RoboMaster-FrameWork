#ifndef __CMDCENTER_H__
#define __CMDCENTER_H__

/* system includes */
#include "main.h"
#include "cmsis_os.h"
/* user includes */
#include "Remote_Ctrl.h"
#include "VTCenter.h"
#include "IMU_Get.h"
#include "Dual_Board_Transmit.h"
#include "AdvancedKeyOperation.h"
#include "Math_Lib.h"

//** #################################################################################################### **//
//** ============================================= 宏定义 =============================================== **//
//** #################################################################################################### **//

#define CMD_CTRL_RANGE  1000    //命令控制范围(-1000 ~ 1000)

// CMD值二次滤波配置（在映射后的数值上做防抖处理，防止残余抖动）
#define CMD_DEADZONE          10    // CMD值死区阈值（映射后绝对值小于此值视为回中）
#define CMD_FILTER_THRESHOLD  5     // 滤波判定阈值
#define CMD_FILTER_CNT_INIT   6     // 计数器初始值（默认 > CMD_FILTER_THRESHOLD）

// CMD状态滤波配置（对离散的ON/OFF、枚举状态做防抖处理，防止拨杆临界抖动）
#define CMD_STATE_DEBOUNCE_SAMPLES  3     // 状态切换需要连续稳定N帧才生效

//** #################################################################################################### **//
//** ============================================ 枚举、结构体 =========================================== **//
//** #################################################################################################### **//


/* enum */
//控制模式
typedef enum
{
    STOP_MODE        = 0,    // 停止模式
    REMOTE_MODE      = 1,    // 遥控器控制
    KEYBOARD_MODE    = 2,    // 键盘控制
    AUTO_MODE        = 3,    // 哨兵全自动模式
} CtrlMode_e;
//移动模式
typedef enum
{
    NORMAL      = 0,//正常模式(地盘跟随)
    SPIN_CW     = 1,//小陀螺模式，CW 
    SPIN_CCW    = 2,//小陀螺模式，CCW
} MoveMode_e;

// 通用开关状态
typedef enum
{
    OFF = 0,
    ON  = 1
} ONOFF_State_e;

/* struct */
typedef struct CMD_t{

    //控制模式：停止模式、遥控器控制、键盘控制、哨兵全自动模式
    CtrlMode_e ctrl;
    //移动模式：正常模式(地盘跟随)；小陀螺模式
    MoveMode_e Move;

    //地盘移动指令
    struct{
        int16_t FB;//前后
        int16_t LR;//左右
        int16_t RO;//旋转
    }Chassis;

    //云台移动指令
    struct{
        int16_t Yaw;//云台水平
        int16_t Pitch;//云台垂直
    }Gimbal;

    //发射机构指令
    struct{
        ONOFF_State_e Fire;//发射标志, OFF表示不发射，ON表示发射
        ONOFF_State_e Load;//上弹标志，OFF表示不上弹，ON表示上弹
        ONOFF_State_e Dial;//拨盘标志，OFF表示不转动，ON表示转动
        ONOFF_State_e Friction;//摩擦轮开启
    }Shooting;

    struct{
        ONOFF_State_e Aim;
        ONOFF_State_e Navigation;
    }Auto;

    struct{
        ONOFF_State_e RefreshUI ;
    }other;

}CMD_t;

//这个要放在CMD_t 后面，因为该头文件要包含CMD_t
#include "Dual_Board_Transmit_Data.h"

//** #################################################################################################### **//
//** =========================================== 对外函数声明 ============================================ **//
//** #################################################################################################### **//

extern const CMD_t* CMD_Get_point(void);

#endif /* __CMDCENTER_H__ */
