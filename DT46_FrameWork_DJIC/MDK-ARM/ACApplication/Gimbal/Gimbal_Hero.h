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

        }Yaw;

        //Pitch轴控制量
        struct{

            float   T_Angle;   //目标角度
            float   C_Angle;   //IMU Pitch角度
            int16_t T_Speed;   //目标速度
	        int16_t T_rpm;     //目标转速
            int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)

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



