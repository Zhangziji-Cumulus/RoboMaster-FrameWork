#ifndef CHASSIS_MECANU_H_
#define CHASSIS_MECANU_H_

#include "A_MCommon.h"
#include "Chassis_Advanced.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == CHASSIS_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

#if(CHASSIS_TYPE == CHASSIS_MECANUM)

//** ------------------------------------------------------------ **//
//** ======================= 定义公式宏 ========================== **//
//** ------------------------------------------------------------ **//

#ifndef MAP_CMD_RANGE_TO_M_S
#define MAP_CMD_RANGE_TO_M_S(cmd) \
    MyMath_Map_Range_Float((float)(cmd), -CMD_CTRL_RANGE, CMD_CTRL_RANGE, \
                            -CHASSIS_MAX_SPEED, CHASSIS_MAX_SPEED)
#endif

//每个轮子单独的数据
typedef struct{
    float T_Speed;
	int16_t T_rpm;     //目标转速
    int16_t Ctrl_Vel;  //最终控制值，(PID计算输出值)
}Wheel_Ctrl_t;

//定义一个地盘数据结构体
typedef struct 
{
    //接收控制指令结构体
    CMD_t CMD;
    //陀螺仪数据
    const fp32* INS_angle;

    //电机反馈数据
    struct{
        const DJI_MotorFeedback_t* Ptr;
        DJI_MotorFeedback_t W_FL;
        DJI_MotorFeedback_t W_FR;
        DJI_MotorFeedback_t W_BL;
        DJI_MotorFeedback_t W_BR;
    }MotorData;
   
    //地盘运动解算结构体
    struct{
        
        //地盘运动目标值
        struct{

            float FB;// 前后
            float LR;// 左右
            float RO;// 旋转
            
        }Target;

        //获取YAW轴电机数据
        struct{
            const DJI_MotorFeedback_t* Ptr;
			DJI_MotorFeedback_t YAW;
        }Yaw_Angle;

        //云台与地盘之间的夹角
        struct 
        {
            float Degree;
            float Radian;    
        }Theta;

        //地盘四个轮子的目标和控制值
        Wheel_Ctrl_t W_FL;
	    Wheel_Ctrl_t W_FR;
	    Wheel_Ctrl_t W_BL;
	    Wheel_Ctrl_t W_BR;

    }Calc;

    uint8_t Mode;

}Chassis_Instance_t;




#endif 

#endif

#endif // CHASSIS_MECANU_H_