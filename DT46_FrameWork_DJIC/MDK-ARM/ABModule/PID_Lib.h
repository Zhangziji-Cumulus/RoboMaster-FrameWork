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
    float last_current; // 上一拍当前值（用于微分对测量值，防止 Derivative Kick）

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


//** ================================================================================ **//
//** ======================== 复合多环 PID 结构体定义 ================================ **//
//** ================================================================================ **//

/**
 * @brief 外环模式枚举：决定最外层环使用线性PID还是角度环PID
 */
typedef enum {
    PID_OUTER_MODE_LINEAR = 0,   ///< 外环线性 PID（默认）
    PID_OUTER_MODE_ANGLE  = 1,   ///< 外环角度环，自动最短路径 [0,360)
} PID_OuterMode_t;

/**
 * @brief 双环PID复合结构体
 * @note  封装内环+外环，支持内环目标值覆盖调参模式
 */
typedef struct {
    PID_HandleTypeDef inner;                ///< 内环 (电流环 / 速度环)
    PID_HandleTypeDef outer;                ///< 外环 (速度环 / 角度环)
    PID_OuterMode_t    outer_mode;          ///< 外环模式：线性/角度
    float  inner_target_override;           ///< 内环目标值手动覆盖（调参用）
    uint8_t inner_override_enable;          ///< 1=启用覆盖, 0=外环输出
    float  threshold;                       ///< 误差阈值，小于该值时内环停止跟随
} PID_Double_t;

/**
 * @brief 三环PID复合结构体
 * @note  封装角度环+速度环+电流环，支持最内层目标值覆盖
 */
typedef struct {
    PID_HandleTypeDef current;              ///< 电流环 (最内层)
    PID_HandleTypeDef speed;                ///< 速度环 (中间层)
    PID_HandleTypeDef angle;                ///< 角度环 (最外层)
    PID_OuterMode_t    outer_mode;          ///< 最外层模式：线性/角度
    float  inner_target_override;           ///< 最内层目标值手动覆盖
    uint8_t inner_override_enable;          ///< 1=启用覆盖
    float  max_angle_error;                 ///< 角度环误差阈值
} PID_Triple_t;


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

//** ================================================================================ **//
//** ==================== 复合多环 PID 初始化与计算函数 ============================== **//
//** ================================================================================ **//

/**
 * @brief  初始化双环PID复合结构体（内环和外环各自独立限幅）
 * @param  pid           双环PID结构体指针
 * @param  kp_in         内环比例系数
 * @param  ki_in         内环积分系数
 * @param  kd_in         内环微分系数
 * @param  kp_ex         外环比例系数
 * @param  ki_ex         外环积分系数
 * @param  kd_ex         外环微分系数
 * @param  out_min_in    内环输出下限
 * @param  out_max_in    内环输出上限
 * @param  int_min_in    内环积分累加值下限
 * @param  int_max_in    内环积分累加值上限
 * @param  out_min_ex    外环输出下限
 * @param  out_max_ex    外环输出上限
 * @param  int_min_ex    外环积分累加值下限
 * @param  int_max_ex    外环积分累加值上限
 * @param  threshold     误差阈值，小于该值时内环停止跟随
 * @param  outer_mode    外环模式（线性/角度）
 */
void PID_Double_Init(PID_Double_t *pid,
                     float kp_in, float ki_in, float kd_in,
                     float kp_ex, float ki_ex, float kd_ex,
                     float out_min_in, float out_max_in,
                     float int_min_in, float int_max_in,
                     float out_min_ex, float out_max_ex,
                     float int_min_ex, float int_max_ex,
                     float threshold,
                     PID_OuterMode_t outer_mode);

/**
 * @brief  初始化三环PID复合结构体（各环独立限幅）
 * @param  pid             三环PID结构体指针
 * @param  kp_cur          电流环(最内层)比例系数
 * @param  ki_cur          电流环(最内层)积分系数
 * @param  kd_cur          电流环(最内层)微分系数
 * @param  kp_spd          速度环(中间层)比例系数
 * @param  ki_spd          速度环(中间层)积分系数
 * @param  kd_spd          速度环(中间层)微分系数
 * @param  kp_ang          角度环(最外层)比例系数
 * @param  ki_ang          角度环(最外层)积分系数
 * @param  kd_ang          角度环(最外层)微分系数
 * @param  out_min_cur     电流环输出下限
 * @param  out_max_cur     电流环输出上限
 * @param  int_min_cur     电流环积分累加值下限
 * @param  int_max_cur     电流环积分累加值上限
 * @param  out_min_spd     速度环输出下限
 * @param  out_max_spd     速度环输出上限
 * @param  int_min_spd     速度环积分累加值下限
 * @param  int_max_spd     速度环积分累加值上限
 * @param  out_min_ang     角度环输出下限
 * @param  out_max_ang     角度环输出上限
 * @param  int_min_ang     角度环积分累加值下限
 * @param  int_max_ang     角度环积分累加值上限
 * @param  max_angle_error 角度环误差阈值
 * @param  outer_mode      最外层模式（线性/角度）
 */
void PID_Triple_Init(PID_Triple_t *pid,
                     float kp_cur, float ki_cur, float kd_cur,
                     float kp_spd, float ki_spd, float kd_spd,
                     float kp_ang, float ki_ang, float kd_ang,
                     float out_min_cur, float out_max_cur,
                     float int_min_cur, float int_max_cur,
                     float out_min_spd, float out_max_spd,
                     float int_min_spd, float int_max_spd,
                     float out_min_ang, float out_max_ang,
                     float int_min_ang, float int_max_ang,
                     float max_angle_error,
                     PID_OuterMode_t outer_mode);

/**
 * @brief  双环PID计算
 * @note   根据 outer_mode 自动选择线性PID或角度环PID(CycleAngle)进行外环计算。
 *         内环目标值支持 override 覆盖模式（调参使用）。
 * @param  pid        双环PID结构体指针
 * @param  target     外环目标值
 * @param  current_in 内环当前反馈值
 * @param  current_ex 外环当前反馈值
 * @retval 内环PID输出值（已限幅）
 */
float PID_Double_Calc(PID_Double_t *pid,
                      float target, float current_in, float current_ex);

/**
 * @brief  三环PID计算（角度环 → 速度环 → 电流环）
 * @note   最外层根据 outer_mode 自动选择线性或角度PID，中间层与最内层始终线性PID。
 *         最内层(电流环)支持 override 覆盖模式。
 * @param  pid              三环PID结构体指针
 * @param  target_angle     角度环目标值
 * @param  current_angle    角度环当前反馈值
 * @param  current_speed    速度环当前反馈值
 * @param  current_current  电流环当前反馈值
 * @retval 最内层(电流环)PID输出值（已限幅）
 */
float PID_Triple_Calc(PID_Triple_t *pid,
                      float target_angle, float current_angle,
                      float current_speed, float current_current);

//** ================================================================================ **//
//** ======================== 复合多环 PID 调参辅助函数 ============================== **//
//** ================================================================================ **//

// === 双环调参辅助 ===

/**
 * @brief  启用内环目标值覆盖模式
 * @note   启用后内环使用手动设定的固定目标值而非外环输出，用于由内到外分步调参。
 * @param  pid    双环PID结构体指针
 * @param  target 内环手动目标值
 */
void  PID_Double_SetInnerOverride(PID_Double_t *pid, float target);

/**
 * @brief  禁用内环目标值覆盖模式
 * @note   恢复内环跟随外环输出的级联模式。
 * @param  pid 双环PID结构体指针
 */
void  PID_Double_DisableInnerOverride(PID_Double_t *pid);

/**
 * @brief  读取外环输出中间量
 * @param  pid 双环PID结构体指针
 * @retval 外环PID当前输出值
 */
float PID_Double_GetOuterOutput(PID_Double_t *pid);

/**
 * @brief  读取内环实际目标值
 * @param  pid 双环PID结构体指针
 * @retval 内环当前实际使用的目标值（覆盖值 or 外环输出）
 */
float PID_Double_GetInnerTarget(PID_Double_t *pid);

// === 三环调参辅助 ===

/**
 * @brief  启用三环最内层目标值覆盖模式
 * @param  pid    三环PID结构体指针
 * @param  target 最内层(电流环)手动目标值
 */
void  PID_Triple_SetInnerOverride(PID_Triple_t *pid, float target);

/**
 * @brief  禁用三环最内层目标值覆盖模式
 * @param  pid 三环PID结构体指针
 */
void  PID_Triple_DisableInnerOverride(PID_Triple_t *pid);

/**
 * @brief  读取三环最外层输出中间量
 * @param  pid 三环PID结构体指针
 * @retval 角度环PID当前输出值
 */
float PID_Triple_GetOuterOutput(PID_Triple_t *pid);

/**
 * @brief  读取三环最内层实际目标值
 * @param  pid 三环PID结构体指针
 * @retval 最内层(电流环)当前实际使用的目标值
 */
float PID_Triple_GetInnerTarget(PID_Triple_t *pid);

										
#ifdef __cplusplus
}
#endif

#endif // PID_LIB_H_
