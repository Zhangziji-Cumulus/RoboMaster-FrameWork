#include "MY_PID.h"
#include <stdint.h>
#include <math.h>

//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//
//** =================================================== **//
//** ================= 【PID计算函数】 ================= **//
//** =================================================== **//
//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//


//* ======================================================= *//
//* ================= 【内部核心计算函数】 ================= *//
//* ======================================================= *//

//提取公共PID算法，避免代码重复

/**
 * @brief  基于误差值的核心PID计算（位置式，含积分/输出限幅）
 * @param  pid: PID结构体指针
 * @param  error: 当前误差 (target - current)
 * @return 限幅后的输出值
 */
static float PID_CalcCore(PID_HandleTypeDef *pid, float error) {
    // 1. 比例项
    float p_out = pid->kp * error;

    // 2. 积分项 (标准形式：先累加原始误差 -> 限幅 -> 乘系数)
    pid->error_sum += error;
    if (pid->error_sum > pid->integral_max) {
        pid->error_sum = pid->integral_max;
    } else if (pid->error_sum < pid->integral_min) {
        pid->error_sum = pid->integral_min;
    }
    float i_out = pid->ki * pid->error_sum;

    // 3. 微分项
    float d_out = pid->kd * (error - pid->last_error);

    // 4. 总输出与限幅
    float total_out = p_out + i_out + d_out;
    if (total_out > pid->output_max) {
        total_out = pid->output_max;
    } else if (total_out < pid->output_min) {
        total_out = pid->output_min;
    }

    // 5. 更新状态变量
    pid->last_error = error;
    pid->Output = total_out;

    return total_out;
}

//* ================================================== *//
//* ================= 【PID 初始化】 ================= *//
//* ================================================= *//

void PID_Init(PID_HandleTypeDef *pid, float kp, float ki, float kd,
              float output_min, float output_max,
              float integral_min, float integral_max) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->target = 0.0f;
    pid->current = 0.0f;
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->error_sum = 0.0f;
    pid->Output = 0.0f;

    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
}

//* =================================================== *//
//* ================= 【单环PID计算】 ================= *//
//* ================================================== *//

//float PID_Calculate(PID_HandleTypeDef *pid, float current_val, float target_val) {
//    pid->current = current_val;
//    pid->target = target_val;
//    
//    // 计算线性误差
//    float error = pid->target - pid->current;
//    pid->error = error;
//    
//    return PID_CalcCore(pid, error);
//}

/**
  * @brief 安全型PID计算（含抗积分饱和与双向限幅）
  * @note  调用前请确保 output_min/max 和 integral_min/max 已正确配置
  */
float PID_Calculate(PID_HandleTypeDef *pid, float current_val, float target_val)
{
    pid->target = target_val;
    pid->current = current_val;
    pid->error = target_val - current_val;

    // 1. 积分累加（带硬限幅，防止数值溢出）
    pid->error_sum += pid->error;
    if (pid->error_sum > pid->integral_max) pid->error_sum = pid->integral_max;
    if (pid->error_sum < pid->integral_min) pid->error_sum = pid->integral_min;

    // 2. 计算原始输出
    float p_out = pid->kp * pid->error;
    float i_out = pid->ki * pid->error_sum;
    float d_out = pid->kd * (pid->error - pid->last_error); // 假设控制周期固定为 1ms
    float raw_output = p_out + i_out + d_out;

    // 3. 输出限幅（保护电调与机械结构）
    if (raw_output > pid->output_max) pid->Output = pid->output_max;
    else if (raw_output < pid->output_min) pid->Output = pid->output_min;
    else pid->Output = raw_output;

    // 4. 抗积分饱和（条件积分法）
    // 当输出被限幅时，冻结积分累加，防止“积分风车”效应导致过零超调
    if (pid->Output != raw_output) {
        pid->error_sum -= pid->error; // 回退本次累加，保持饱和前的积分值
    }

    pid->last_error = pid->error;
    return pid->Output;
}

/**
  * @brief PID状态重置（必须在运行模式切换瞬间调用）
  */
void PID_Reset(PID_HandleTypeDef *pid)
{
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->error_sum = 0.0f;
    pid->Output = 0.0f;
}

//* ============================================================= *//
//* ================= 【360°角度环专用PID计算】 ================= *//
//* ============================================================= *//

/**
 * @brief  计算最短路径角度误差 [-180, 180)
 */
float PID_CalcNearestAngleError(float current, float target) {
    float error = fmodf(target - current, 360.0f);
    if (error > 180.0f) {
        error -= 360.0f;
    } else if (error <= -180.0f) {
        error += 360.0f;
    }
    return error;
}
//单环循环角度控制
float PID_Calculate_CycleAngle(PID_HandleTypeDef *pid, float current, float target) {
    // 1. 角度归一化到 [0, 360)
    current = fmodf(current, 360.0f);
    if (current < 0.0f) current += 360.0f;
    
    target = fmodf(target, 360.0f);
    if (target < 0.0f) target += 360.0f;

    pid->current = current;
    pid->target = target;

    // 2. 计算最短路径误差
    float error = PID_CalcNearestAngleError(current, target);
    pid->error = error;

    return PID_CalcCore(pid, error);
}

//双环控制循环角度
float PID_Double_CycleAngle(PID_HandleTypeDef* PID_In, PID_HandleTypeDef* PID_Ex, 
                            float Target, float Current_In, float Current_Ex, float MError)
{
    float PID_ExOutput = PID_Calculate_CycleAngle(PID_Ex, Current_Ex, Target);

    float PID_InOutput = 0.0f;
    if (fabsf(Current_Ex - Target) > MError) {
        PID_InOutput = PID_Calculate(PID_In, Current_In, PID_ExOutput);
    }

    PID_Ex->Output = PID_ExOutput;
    PID_In->Output = PID_InOutput;
    
    return PID_InOutput;
}

//* =================================================== *//
//* ================= 【多环PID控制】 ================= *//
//* ================================================== *//

//双环控制
float PID_Double_Calculate(PID_HandleTypeDef* PID_In, PID_HandleTypeDef* PID_Ex, 
                           float Target, float Current_In, float Current_Ex, float MError)
{
    // 外环计算
    float PID_ExOutput = PID_Calculate(PID_Ex, Current_Ex, Target);
		
    float PID_InOutput = 0.0f;
    // 死区判断：误差小于阈值时内环清零（防抖动）
    if (fabsf(Current_Ex - Target) > MError) {
        PID_InOutput = PID_Calculate(PID_In, Current_In, PID_ExOutput);
    }

    PID_Ex->Output = PID_ExOutput;
    PID_In->Output = PID_InOutput;
    
    return PID_InOutput;
}
//三环控制
float PID_Triple_Calculate(PID_HandleTypeDef* PID_Angle,
                           PID_HandleTypeDef* PID_Speed,
                           PID_HandleTypeDef* PID_Current,
                           float target_angle, float current_angle,
                           float current_speed, float current_current,
                           float max_angle_error)
{
		PID_Angle->current   = current_angle;
		PID_Speed->current   = current_speed;
	  PID_Current->current = current_current;
    // 1. 外环：角度 -> 目标速度
    float pid_angle_output = PID_Calculate(PID_Angle, current_angle, target_angle);
    PID_Angle->Output = pid_angle_output;

    // 2. 中环：速度 -> 目标电流
    float pid_speed_output = 0.0f;
    if (fabsf(target_angle - current_angle) > max_angle_error) {
        pid_speed_output = PID_Calculate(PID_Speed, current_speed, pid_angle_output);
    }
    PID_Speed->Output = pid_speed_output;

    // 3. 内环：电流 -> 最终控制量
    float pid_current_output = PID_Calculate(PID_Current, current_current, pid_speed_output);
    PID_Current->Output = pid_current_output;

    return pid_current_output;
}


//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//
//** =========================================================== **//
//** ================= 【带前馈的PID计算函数】 ================= **//
//** =========================================================== **//
//** @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ **//


/**
 * @brief 初始化PID参数
 */
void PID_FF_Init(PID_FF_HandleTypeDef *pid, float kp, float ki, float kd, float kff,
              float out_min, float out_max, float int_min, float int_max) {
    if (pid == NULL) return;

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->kff = kff;

    pid->output_min = out_min;
    pid->output_max = out_max;
    pid->integral_min = int_min;
    pid->integral_max = int_max;

    PID_FF_Reset(pid);
    pid->enable = 1;
}

/**
 * @brief 重置PID状态（切换工作模式/急停/上电时调用）
 */
void PID_FF_Reset(PID_FF_HandleTypeDef *pid) {
    if (pid == NULL) return;
    pid->error = 0.0f;
    pid->error_sum = 0.0f;
    pid->last_error = 0.0f;
    pid->last_target = 0.0f;
    pid->target = 0.0f;
    pid->current = 0.0f;
    pid->output = 0.0f;
}

/**
 * @brief 核心计算（静态内部函数）
 */
static float PID_FF_CalcCore(PID_FF_HandleTypeDef *pid, float error, float ff_out) {
    // 1. 比例项
    float p_out = pid->kp * error;

    // 2. 积分项 (标准抗饱和：先累加 -> 限幅 -> 乘系数)
    pid->error_sum += error;
    if (pid->error_sum > pid->integral_max) {
        pid->error_sum = pid->integral_max;
    } else if (pid->error_sum < pid->integral_min) {
        pid->error_sum = pid->integral_min;
    }
    float i_out = pid->ki * pid->error_sum;

    // 3. 微分项
    float d_out = pid->kd * (error - pid->last_error);

    // 4. 总输出 = PID + 前馈
    float total_out = p_out + i_out + d_out + ff_out;

    // 5. 输出限幅 (前馈与PID叠加后统一限幅，保护执行器)
    if (total_out > pid->output_max) {
        total_out = pid->output_max;
    } else if (total_out < pid->output_min) {
        total_out = pid->output_min;
    }

    // 6. 更新状态
    pid->last_error = error;
    pid->output = total_out;

    return total_out;
}

/**
 * @brief 标准计算接口（推荐：外部计算前馈传入，灵活性最高）
 * @param ff_value 外部计算好的前馈补偿值
 */
float PID_FF_Calculate(PID_FF_HandleTypeDef *pid, float current_val, float target_val, float ff_value) {
    if (pid == NULL) return 0.0f;
    if (!pid->enable) return pid->output; // 禁用时保持上一拍输出

    pid->current = current_val;
    pid->target = target_val;
    pid->error = pid->target - pid->current;

    return PID_FF_CalcCore(pid, pid->error, ff_value);
}

/**
 * @brief 内置前馈计算接口（适合快速部署）
 */
float PID_FF_Calculate_AutoFF(PID_FF_HandleTypeDef *pid, float current_val, float target_val, PID_FF_Mode_t mode) {
    if (pid == NULL || !pid->enable) return pid ? pid->output : 0.0f;

    float ff_value = 0.0f;
    switch (mode) {
        case PID_FF_MODE_TARGET:
            ff_value = pid->kff * target_val;
            break;
        case PID_FF_MODE_VELOCITY:
            ff_value = pid->kff * (target_val - pid->last_target);
            pid->last_target = target_val; // 更新历史目标值
            break;
        default:
            break;
    }
    return PID_FF_Calculate(pid, current_val, target_val, ff_value);
}


/**
 * @brief 计算最短角度差 (返回值范围: [-180.0, 180.0))
 */
static inline float PID_AngleShortestError(float from, float to) {
    float err = to - from;
    if (err > 180.0f) err -= 360.0f;
    else if (err < -180.0f) err += 360.0f;
    return err;
}

/**
 * @brief 周期角度环 PID + 前馈计算
 * @param mode: 前馈模式 (TARGET / VELOCITY)
 * @return 限幅后的控制输出
 */
float ff_value = 0.0f;

float PID_FF_Calculate_CycleAngle(PID_FF_HandleTypeDef *pid, float current, float target) {
    if (pid == NULL || !pid->enable) return pid ? pid->output : 0.0f;

    // ================= 1. 前馈计算 (必须在归一化前处理边界) =================
    //float ff_value = 0.0f;

    
    float delta_target = PID_AngleShortestError(pid->last_target, target);
    ff_value = pid->kff * delta_target; // kff 实际已吸收 1/dt


    // ================= 2. 角度归一化 (统一映射到 [-180, 180)) =================
    pid->current = fmodf(current, 360.0f);
    if (pid->current >= 180.0f) pid->current -= 360.0f;
    else if (pid->current < -180.0f) pid->current += 360.0f;

    pid->target = fmodf(target, 360.0f);
    if (pid->target >= 180.0f) pid->target -= 360.0f;
    else if (pid->target < -180.0f) pid->target += 360.0f;

    // ================= 3. 计算最短路径误差 =================
    pid->error = PID_AngleShortestError(pid->current, pid->target);

    // ================= 4. 更新历史状态 =================
    // 必须更新 last_target，否则下一次速度前馈 delta 计算会出错
    pid->last_target = pid->target;

    // ================= 5. 调用核心计算 =================
    // 直接传入已计算好的 error 和 ff_value，避免核心函数重复计算或覆盖
    return PID_FF_CalcCore(pid, pid->error, ff_value);
}

