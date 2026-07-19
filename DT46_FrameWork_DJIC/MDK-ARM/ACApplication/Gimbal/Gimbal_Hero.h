#ifndef GIMBAL_HERO_H_
#define GIMBAL_HERO_H_

#include "A_MCommon.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == GIMBAL_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

//** #################################################################################################### **//
//** ====================================== 定义数据、结构体 ============================================= **//
//** #################################################################################################### **//

#if(ROBOT_TYPE == ROBOTTYPE_HERO)


typedef struct 
{
    //接收控制指令结构体
    CMD_t CMD;
    //陀螺仪数据
    const fp32* INS_angle;

    //电机反馈数据
    struct{
        const DJI_MotorFeedback_t* Ptr; //反馈电机数据
        DJI_MotorFeedback_t Yaw;
        DJI_MotorFeedback_t Pitch;
    }MotorData;

    struct{

        //Yaw轴控制量
        struct{

            float   T_Angle;   //目标角度
            float   C_Angle;   //IMU Yaw角度
            int16_t T_Speed;   //目标速度
	        int16_t T_rpm;     //目标转速
            int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)

            //自瞄线性插值状态
            float   LerpStart;      //插值起点角度
            float   LerpTarget;     //插值目标角度（绝对角度）
            uint32_t LerpStartTick; //插值起始时间戳(ms)
            uint32_t LerpTick;      //上次触发的自瞄时间戳(ms)，用于检测新数据
            uint16_t LerpDuration;  //插值周期(ms)，默认100ms

        }Yaw;

        //Pitch轴控制量
        struct{

            float   T_Angle;   //目标角度
            float   C_Angle;   //IMU Pitch角度
            int16_t T_Speed;   //目标速度
	        int16_t T_rpm;     //目标转速
            int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)

            //自瞄线性插值状态
            float   LerpStart;      //插值起点角度
            float   LerpTarget;     //插值目标角度（绝对角度）
            uint32_t LerpStartTick; //插值起始时间戳(ms)
            uint32_t LerpTick;      //上次触发的自瞄时间戳(ms)，用于检测新数据
            uint16_t LerpDuration;  //插值周期(ms)，默认100ms

        }Pitch;

    }Calc;

    //自瞄结构体
    struct{

        AutoAim_Ctrl_t Aim; //自瞄控制结构体

    }Auto;
}Gimbal_Instance_t;

#endif

#endif

#endif // GIMBAL_HERO_H_



