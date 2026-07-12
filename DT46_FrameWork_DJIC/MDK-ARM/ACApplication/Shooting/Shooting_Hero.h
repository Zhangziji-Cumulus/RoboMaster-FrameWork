#ifndef SHOOTING_HERO_H_
#define SHOOTING_HERO_H_

#include "A_MCommon.h"
#include "Dual_Board_Transmit.h"
#include "User_CallBack.h"

//** #################################################################################################### **//
//** ============================================ 定义宏 ================================================= **//
//** #################################################################################################### **//

#if((BOARD_MODE == BOARD_MODE_DUAL))

/* 
    自动装弹思路，摩擦轮检测电流，电流突变很大一段时间，判断发出一颗弹丸，给一个标志；
    推杆向后给出空间后，给一个标志；拨盘上弹，检测电流过大后，给标志。
*/

typedef enum
{
    LOAD_STOP,  //停止
    LOAD_ING,   //装弹中
    //LOAD_OK,    //装弹完成
    //LOAD_WAIT   //等待下一次装弹
}LOAD_State_e;


#if(ROBOT_TYPE == ROBOTTYPE_HERO)

//** #################################################################################################### **//
//** ====================================== 定义数据、结构体 ============================================= **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ==================================== 枚举 ====================================== **//
//** ================================================================================ **//




/* 推杆状态枚举 */
typedef enum{

    PUSH_BACK_ENTER,    //向后移动
    PUSH_BACK_ING,      //向后移动中
    PUSH_FRONT_ENTER,   //向前移动
    PUSH_FRONT_ING      //向前移动中
}PushRod_State_e;

/* 填弹状态枚举 */
typedef enum{
    LOAD_START,     //开始填弹
    LOAD_OK,       //填弹完成
    LOAD_WAIT       //等待下一次填弹指令
}Load_State_e;
/* 发射机构状态枚举 */

// typedef enum{
//     FIRE_READY,     //发射准备
//     FIRE_ING,       //发射中
//     FIRE_END_B,     //推杆在点B时完成发射
//     FIRE_END_C,     //推杆在点C时完成发射
//     ReLoad          //开始重新填弹
// }Fire_State_e;
/* 推杆行程状态枚举 */
typedef enum
{
    FIRE_STA_IDLE = 0,      // 待机：推杆在A点，等待第一次开火触发
    FIRE_STA_MOVE_A2B,      // 推杆从A点移动到B点
    FIRE_STA_B_DETECT,      // B点停留，检测发射信号（带超时）
    FIRE_STA_B_READY,       // B点发射成功，待发射，等待第二次开火
    FIRE_STA_MOVE_B2C,      // 推杆从B点移动到C点
    FIRE_STA_MOVE_C2L,      // 推杆从C点返回L点

    FIRE_READY,     //发射准备
    FIRE_END_B,     //推杆在点B时完成发射
    FIRE_END_C,     //推杆在点C时完成发射

} Fire_State_e;

typedef enum{
    PUSHPL,//推杆在L点
    PUSHPA,//推杆在A点
    PUSHPB,//推杆在B点
    PUSHPC //推杆在C点
}PushRod_Stroke_State_e;

//** ================================================================================ **//
//** =================================== 结构体 ===================================== **//
//** ================================================================================ **//
typedef struct 
{
    //接收控制指令结构体
    CMD_t CMD;
    //陀螺仪数据
    const fp32* INS_angle;

    //电机反馈数据
    struct{
        const DJI_MotorFeedback_t* Ptr; //反馈电机数据

        DJI_MotorFeedback_t UL;
        DJI_MotorFeedback_t UR;
        DJI_MotorFeedback_t DM;

        DJI_MotorFeedback_t DIAL;
    }DJI_Motordata;

    struct{

        //摩擦轮
        struct{
            
            float ShootingSpeed;

            struct
            {
	            int16_t T_rpm;     //目标转速
                 int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)
            }UL;

            struct
            {
	            int16_t T_rpm;      //目标转速
                 int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)
            }UR;

            struct
            {
	            int16_t T_rpm;     //目标转速
                int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)
            }DM;

        }Friction;

        //拨盘
        struct{

            float   T_Angle;   //目标角度
            float   T_Speed;   //目标速度
	        int16_t T_rpm;     //目标转速
            int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)

        }Dial;

        //推杆
        struct
        {
            PushRod_State_e State;
            float   T_Angle;
            int16_t Ctrl_Vel;
            uint8_t CtlFlag;
        }PushRod;
        
    }Calc;

}Shooting_Instance_t;

typedef struct
{
    Load_State_e Load;
    Fire_State_e Fire;
    PushRod_Stroke_State_e Push_Stroke;

    uint8_t LoadReady_Flag;
    uint8_t Fired_Flag;//开火标志
    uint8_t Fired_Count;//一个循环发射了弹丸的次数

}Shooting_State_Machine_t;

#endif

#endif

#endif // SHOOTING_HERO_H_
