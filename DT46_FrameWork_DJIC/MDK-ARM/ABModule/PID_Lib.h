#ifndef PID_LIB_H_
#define PID_LIB_H_

#ifdef __cplusplus
extern "C" {
	
#endif

//** #################################################################################################### **//
//** ========================================== 头文件引用 =============================================== **//
//** #################################################################################################### **//

#include "A_CommonSystem.h"
	
//** #################################################################################################### **//
//** =========================================== 宏定义 ================================================= **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ============================== PID控制模式定义 ================================== **//
//** ================================================================================ **//

// 基础模式
#define PID_CTRL_MODE_NORMAL        0    // 普通PID（无特殊逻辑，单环基础模式）

// 环路结构模式
#define PID_CTRL_MODE_SINGLE_LOOP   1    // 单环PID（位置/速度单环）
#define PID_CTRL_MODE_DOUBLE_LOOP   2    // 双环PID（位置环+速度环级联）
#define PID_CTRL_MODE_TRIPLE_LOOP   3    // 三环PID（位置+速度+电流三环串级）

// 带前馈模式（最常用）
#define PID_CTRL_MODE_SINGLE_FF     4    // 单环PID + 前馈
#define PID_CTRL_MODE_DOUBLE_FF     5    // 双环PID + 前馈
#define PID_CTRL_MODE_TRIPLE_FF     6    // 三环PID + 前馈


//** #################################################################################################### **//
//** ======================================== 数据类型定义 =============================================== **//
//** #################################################################################################### **//	
/**
  * @brief PID控制器句柄结构体
  * @note  使用前需通过 PID_Init() 初始化参数
  */
typedef struct {
    float kp;               ///< 比例系数
    float ki;               ///< 积分系数
    float kd;               ///< 微分系数

    float target;           ///< 目标设定值
    float current;          ///< 当前反馈值
    float error;            ///< 当前误差 (target - current)
    float last_error;       ///< 上一次误差
    float error_sum;        ///< 误差积分累加值（用于抗饱和限幅）

    float output_min;       ///< 输出下限
    float output_max;       ///< 输出上限
    float integral_min;     ///< 积分累加值下限
    float integral_max;     ///< 积分累加值上限

    float Output;           ///< 最终计算输出值（已限幅，供外部读取）
} PID_HandleTypeDef;

/**
 * @brief PID+前馈控制句柄
 */
typedef struct {
    // 增益参数
    float kp;
    float ki;
    float kd;
    float kff;          // 前馈增益系数

    // 状态变量
    float error;        // 当前误差
    float error_sum;    // 积分累加值
    float last_error;   // 上一拍误差
    float target;       // 目标值
    float current;      // 当前反馈值
    float output;       // 最终输出值
    float last_target;  // 上一拍目标值（用于速度前馈）

    // 限幅参数
    float output_max;
    float output_min;
    float integral_max;
    float integral_min;

    // 控制标志
    uint8_t enable;     // 1:启用, 0:禁用(输出保持或清零)
} PID_FF_HandleTypeDef;

/**
 * @brief 前馈计算模式枚举
 */
typedef enum {
    PID_FF_MODE_NONE = 0,       ///< 无/外部传入
    PID_FF_MODE_TARGET,         ///< 目标值前馈：Kff * target (克服恒定负载/重力)
    PID_FF_MODE_VELOCITY        ///< 速度前馈：Kff * Δtarget (提升动态响应/轨迹跟踪)
} PID_FF_Mode_t;


//** #################################################################################################### **//
//** =========================================== 函数声明 =============================================== **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ================================= PID基础函数 =================================== **//
//** ================================================================================ **//

/* 初始化PID控制器参数 */
void PID_Init(PID_HandleTypeDef *pid, 
              float kp, 
              float ki, 
              float kd,
              float output_min, float output_max,
              float integral_min, float integral_max);

/* 重置PID状态，适用于切换模式或停止控制时清除历史量 */											 
void PID_Reset(PID_HandleTypeDef *pid);

/* 标准单环PID计算（位置式，带积分抗饱和与输出限幅） */
float PID_Calculate(PID_HandleTypeDef *pid, float current_val, float target_val);

// /* 计算360°旋转场景下的最短路径角度误差 [-180, 180)  */
// float PID_CalcNearestAngleError(float current, float target);

/* 360°角度环专用PID计算（自动处理角度跳变与最短路径） */
float PID_Calculate_CycleAngle(PID_HandleTypeDef *pid, float current, float target);

/* 双环PID控制计算（外环 → 内环） */
float PID_Double_Calculate(PID_HandleTypeDef *PID_In, 
                           PID_HandleTypeDef *PID_Ex,
                           float Target, 
                           float Current_In, 
                           float Current_Ex, 
                           float MError);

/* 360°角度双环PID控制计算（外环角度 → 内环线性量） */
float PID_Double_CycleAngle(PID_HandleTypeDef *PID_In, 
                            PID_HandleTypeDef *PID_Ex,
                            float Target, 
                            float Current_In, 
                            float Current_Ex, 
                            float MError);

/* 三环PID控制计算（角度环 → 速度环 → 电流环） */
float PID_Triple_Calculate(PID_HandleTypeDef *PID_Angle,
                           PID_HandleTypeDef *PID_Speed,
                           PID_HandleTypeDef *PID_Current,
                           float target_angle, 
                           float current_angle,
                           float current_speed, 
                           float current_current,
                           float max_angle_error);

//** ================================================================================ **//
//** ============================ 带前馈的PID计算函数 ================================ **//
//** ================================================================================ **//									 

/* 初始化前馈 PID 参数 */
void PID_FF_Init(PID_FF_HandleTypeDef *pid, 
                 float kp, 
                 float ki, 
                 float kd,
                 float kff,
                 float out_min, float out_max, 
                 float int_min, float int_max);

/* 重置前馈 PID 状态，用于开始控制或切换状态时清除历史量 */                 
void PID_FF_Reset(PID_FF_HandleTypeDef *pid);

/* 计算前馈 PID 输出，支持外部前馈分量输入 */
float PID_FF_Calculate(PID_FF_HandleTypeDef *pid, 
                       float current_val, 
                       float target_val, 
                       float ff_value);

/* 自动计算前馈量并执行前馈 PID 控制 */                       
float PID_FF_Calculate_AutoFF(PID_FF_HandleTypeDef *pid, 
                             float current_val, 
                             float target_val, 
                             PID_FF_Mode_t mode);		

/* 单环循环角度控制，自动计算前馈量 */
float PID_FF_Calculate_CycleAngle(PID_FF_HandleTypeDef *pid,
                                  float current, 
                                  float target);	

										
																	
#ifdef __cplusplus
}
#endif

#endif // PID_LIB_H_
