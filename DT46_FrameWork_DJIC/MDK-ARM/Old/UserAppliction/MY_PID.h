/**
  ******************************************************************************
  * @file    MY_PID.h
  * @author  [Your Name/Team]
  * @brief   PID控制器库头文件
  *          支持：单环PID、360°角度环、双环PID、三环PID（角度-速度-电流）
  ******************************************************************************
  */

#ifndef __MY_PID_H
#define __MY_PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "MY_PID.h"
#include "math.h"
#include <stdlib.h> 
	
//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//
//** =================================================== **//
//** ================= 【PID计算函数】 ================= **//
//** =================================================== **//
//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//	

	
/* ============================ 数据类型定义 ============================ */
	
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

/* ============================ 函数声明 ============================ */

/**
  * @brief  初始化PID控制器参数
  * @param  pid: PID句柄指针
  * @param  kp, ki, kd: PID系数
  * @param  output_min, output_max: 输出限幅范围
  * @param  integral_min, integral_max: 积分累加值限幅范围
  */
void PID_Init(PID_HandleTypeDef *pid, float kp, float ki, float kd,
              float output_min, float output_max,
              float integral_min, float integral_max);

/**
  * @brief  标准单环PID计算（位置式，带积分抗饱和与输出限幅）
  * @param  pid: PID句柄指针
  * @param  current_val: 当前反馈值
  * @param  target_val: 目标设定值
  * @retval 计算后的控制输出值
  */
float PID_Calculate(PID_HandleTypeDef *pid, float current_val, float target_val);

/**
  * @brief  计算360°旋转场景下的最短路径角度误差 [-180, 180)
  * @param  current: 当前角度 (°)
  * @param  target: 目标角度 (°)
  * @retval 标准化后的角度误差
  */
float PID_CalcNearestAngleError(float current, float target);

/**
  * @brief  360°角度环专用PID计算（自动处理角度跳变与最短路径）
  * @param  pid: PID句柄指针
  * @param  current: 当前角度 (°)
  * @param  target: 目标角度 (°)
  * @retval 计算后的控制输出值
  */
float PID_Calculate_CycleAngle(PID_HandleTypeDef *pid, float current, float target);

/**
  * @brief  双环PID控制计算（外环 → 内环）
  * @param  PID_In: 内环PID句柄
  * @param  PID_Ex: 外环PID句柄
  * @param  Target: 外环目标值
  * @param  Current_In: 内环当前反馈值
  * @param  Current_Ex: 外环当前反馈值
  * @param  MError: 外环死区阈值，小于此值时内环输出清零
  * @retval 内环输出值（最终控制量）
  */
float PID_Double_Calculate(PID_HandleTypeDef *PID_In, PID_HandleTypeDef *PID_Ex,
                           float Target, float Current_In, float Current_Ex, float MError);

/**
  * @brief  360°角度双环PID控制计算（外环角度 → 内环线性量）
  * @param  PID_In: 内环PID句柄（通常为速度或位置环）
  * @param  PID_Ex: 外环角度PID句柄
  * @param  Target: 目标角度 (°)
  * @param  Current_In: 内环当前反馈值
  * @param  Current_Ex: 当前角度 (°)
  * @param  MError: 角度死区阈值
  * @retval 内环输出值
  */
float PID_Double_CycleAngle(PID_HandleTypeDef *PID_In, PID_HandleTypeDef *PID_Ex,
                            float Target, float Current_In, float Current_Ex, float MError);

/**
  * @brief  三环PID控制计算（角度环 → 速度环 → 电流环）
  * @param  PID_Angle: 外环（角度）PID句柄
  * @param  PID_Speed: 中环（速度）PID句柄
  * @param  PID_Current: 内环（电流）PID句柄
  * @param  target_angle: 目标角度
  * @param  current_angle: 当前角度
  * @param  current_speed: 当前速度
  * @param  current_current: 当前电流
  * @param  max_angle_error: 角度激活阈值
  * @retval 电流环输出值（最终控制量）
  */
float PID_Triple_Calculate(PID_HandleTypeDef *PID_Angle,
                           PID_HandleTypeDef *PID_Speed,
                           PID_HandleTypeDef *PID_Current,
                           float target_angle, float current_angle,
                           float current_speed, float current_current,
                           float max_angle_error);
													 
void PID_Reset(PID_HandleTypeDef *pid);
												 
//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//
//** =========================================================== **//
//** ================= 【带前馈的PID计算函数】 ================= **//
//** =========================================================== **//
//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//
													 
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

/* ================= 公开API ================= */
void PID_FF_Init(PID_FF_HandleTypeDef *pid, float kp, float ki, float kd, float kff,
              float out_min, float out_max, float int_min, float int_max);
void PID_FF_Reset(PID_FF_HandleTypeDef *pid);
float PID_FF_Calculate(PID_FF_HandleTypeDef *pid, float current_val, float target_val, float ff_value);
float PID_FF_Calculate_AutoFF(PID_FF_HandleTypeDef *pid, float current_val, float target_val, PID_FF_Mode_t mode);													 
//单环循环角度控制
float PID_FF_Calculate_CycleAngle(PID_FF_HandleTypeDef *pid, float current, float target);	

													 
#ifdef __cplusplus
}
#endif

#endif /* __MY_PID_H */
