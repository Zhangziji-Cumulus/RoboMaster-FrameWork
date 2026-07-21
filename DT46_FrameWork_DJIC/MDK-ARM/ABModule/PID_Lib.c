#include "PID_Lib.h"


//** #################################################################################################### **//
//** ========================================= PID 基础函数 ============================================= **//
//** #################################################################################################### **//

/**
 * @brief  PID 初始化函数
 */
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

/**
  * @brief 重置 PID 状态，适用于切换模式或停止控制时清除历史量
  */
void PID_Reset(PID_HandleTypeDef *pid)
{
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->error_sum = 0.0f;
    pid->Output = 0.0f;
}

/**
 * @brief  优化版 PID 计算函数，支持积分项限幅、条件积分抗饱和与输出限幅
 * 
 * @param  pid: PID 句柄指针
 * @param  error: 当前误差值 (目标值 - 当前值)
 * @return PID 输出值
 */
static float PID_CalcCore(PID_HandleTypeDef *pid, float error) {
    // 1. 比例项
    float p_out = pid->kp * error;

    // 2. 微分项 (保留误差微分形式，实际工程建议改为对 PV 微分或加一阶低通滤波)
    float d_out = pid->kd * (error - pid->last_error);

    // 3. 积分项：采用“条件积分”防饱和
    // 预估当前总输出，判断是否已饱和。若饱和且误差方向会加剧饱和，则停止累加
    float pre_total = p_out + pid->ki * pid->error_sum + d_out;
    bool integrate = true;
    
    if (pre_total >= pid->output_max && error > 0) {
        integrate = false; // 正向饱和，误差为正，停止积分
    } else if (pre_total <= pid->output_min && error < 0) {
        integrate = false; // 负向饱和，误差为负，停止积分
    }

    if (integrate) {
        pid->error_sum += error;
    }

    float i_out = pid->ki * pid->error_sum;

    // 4. 积分项硬性限幅 (对 i_out 限幅，阈值物理意义明确)
    if (i_out > pid->integral_max) {
        i_out = pid->integral_max;
        // 同步修正 error_sum，保持 Ki 一致性 (防止 Ki=0 时除零，此处假设 Ki>0)
        if (pid->ki > 1e-6f) pid->error_sum = i_out / pid->ki;
    } else if (i_out < pid->integral_min) {
        i_out = pid->integral_min;
        if (pid->ki > 1e-6f) pid->error_sum = i_out / pid->ki;
    }

    // 5. 叠加输出与输出限幅
    float total_out = p_out + i_out + d_out;
    if (total_out > pid->output_max) {
        total_out = pid->output_max;
    } else if (total_out < pid->output_min) {
        total_out = pid->output_min;
    }

    // 6. 更新状态
    pid->last_error = error;
    pid->Output = total_out;

    return total_out;
}

//** #################################################################################################### **//
//** ========================================= PID 计算函数 ========================================= **//
//** #################################################################################################### **//


//** ================================================================================ **//
//** ================================= 基础 PID ===================================== **//
//** ================================================================================ **//

float PID_Calculate(PID_HandleTypeDef *pid, float current_val, float target_val) {
   pid->current = current_val;
   pid->target = target_val;
   
   // 计算线性误差
   float error = pid->target - pid->current;
   pid->error = pid->target - pid->current;
   
   return PID_CalcCore(pid, error);
}

// /**
//   * @brief  单环 PID 计算，包含抗积分饱和保护与输出限幅
//   * @note   使用前请先配置好 output_min/max 与 integral_min/max
//   */
// float PID_Calculate(PID_HandleTypeDef *pid, float current_val, float target_val)
// {
//     pid->target = target_val;
//     pid->current = current_val;
//     pid->error = target_val - current_val;

//     // 1. 误差积分，并进行积分限幅
//     pid->error_sum += pid->error;
//     if (pid->error_sum > pid->integral_max) pid->error_sum = pid->integral_max;
//     if (pid->error_sum < pid->integral_min) pid->error_sum = pid->integral_min;

//     // 2. 计算比例、积分、微分项
//     float p_out = pid->kp * pid->error;
//     float i_out = pid->ki * pid->error_sum;
//     float d_out = pid->kd * (pid->error - pid->last_error); // 差分周期假定为 1 单位时间
//     float raw_output = p_out + i_out + d_out;

//     // 3. 输出限幅
//     if (raw_output > pid->output_max) pid->Output = pid->output_max;
//     else if (raw_output < pid->output_min) pid->Output = pid->output_min;
//     else pid->Output = raw_output;

//     // 4. 抗积分饱和保护
//     // 当输出被限幅时，将本次误差从积分项中回退，避免积分持续累积
//     if (pid->Output != raw_output) {
//         pid->error_sum -= pid->error;
//     }

//     pid->last_error = pid->error;
//     return pid->Output;
// }

/**
 * @brief  双环 PID 计算，外环控制给内环目标值（一般外环为角度环，内环为速度或电流环）
 */
float PID_Double_Calculate(PID_HandleTypeDef* PID_In, PID_HandleTypeDef* PID_Ex, 
                           float Target, float Current_In, float Current_Ex, float MError)
{
    // 1. 外环 PID 计算
    float PID_ExOutput = PID_Calculate(PID_Ex, Current_Ex, Target);
		
    float PID_InOutput = 0.0f;
    // 当外环误差超过阈值时，内环才开始跟随外环输出
    if (fabsf(Current_Ex - Target) > MError) {
        PID_InOutput = PID_Calculate(PID_In, Current_In, PID_ExOutput);
    }

    PID_Ex->Output = PID_ExOutput;
    PID_In->Output = PID_InOutput;
    
    return PID_InOutput;
}

/**
 * @brief  三级 PID 计算，角度环 -> 速度环 -> 电流环
 */
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

    // 1. 角度环计算期望速度
    float pid_angle_output = PID_Calculate(PID_Angle, current_angle, target_angle);
    PID_Angle->Output = pid_angle_output;

    // 2. 速度环计算期望电流
    float pid_speed_output = 0.0f;
    if (fabsf(target_angle - current_angle) > max_angle_error) {
        pid_speed_output = PID_Calculate(PID_Speed, current_speed, pid_angle_output);
    }
    PID_Speed->Output = pid_speed_output;

    // 3. 电流环输出控制量
    float pid_current_output = PID_Calculate(PID_Current, current_current, pid_speed_output);
    PID_Current->Output = pid_current_output;

    return pid_current_output;
}

//** ================================================================================ **//
//** =========================== 360° 角度 PID 处理 ================================== **//
//** ================================================================================ **//

/**
 * @brief  计算最短角度误差，结果范围为 [-180, 180)
 */
static float PID_CalcNearestAngleError(float current, float target) {
    float error = fmodf(target - current, 360.0f);
    if (error > 180.0f) {
        error -= 360.0f;
    } else if (error <= -180.0f) {
        error += 360.0f;
    }
    return error;
}

/**
 * @brief  计算单位循环角度 PID，适用于角度环控制
 */
float PID_Calculate_CycleAngle(PID_HandleTypeDef *pid, float current, float target) {
    // 1. 统一角度到 [0, 360)
    current = fmodf(current, 360.0f);
    if (current < 0.0f) current += 360.0f;
    
    target = fmodf(target, 360.0f);
    if (target < 0.0f) target += 360.0f;

    pid->current = current;
    pid->target = target;

    // 2. 计算最短角度误差
    float error = PID_CalcNearestAngleError(current, target);
    pid->error = error;

    return PID_CalcCore(pid, error);
}

/**
 * @brief  双环角度控制：外环角度 -> 内环量
 * @param PID_In: 内环 PID
 * @param PID_Ex: 外环 PID
 * @param Target: 目标角度
 * @param Current_In: 内环当前值
 * @param Current_Ex: 外环当前值
 * @param MError: 误差阈值，小于该值时内环停止跟随
 */
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

//** #################################################################################################### **//
//** ======================================= 前馈 PID 控制算法 =========================================== **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ================================ 前馈 PID 基础 ================================== **//
//** ================================================================================ **//

/**
 * @brief 初始化前馈 PID 参数
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
 * @brief 重置前馈 PID 状态，用于开始控制或切换状态时清除历史量
 */
void PID_FF_Reset(PID_FF_HandleTypeDef *pid) {
    if (pid == NULL) return;
    pid->error = 0.0f;
    pid->error_sum = 0.0f;
    pid->last_error = 0.0f;
    pid->last_target = 0.0f;
    pid->last_current = 0.0f;
    pid->target = 0.0f;
    pid->current = 0.0f;
    pid->output = 0.0f;
}

//** ================================================================================ **//
//** ============================== 前馈PID计算基础  ================================= **//
//** ================================================================================ **//

/**
 * @brief 前馈 PID 核心计算，包含 P、I、D 以及前馈分量
 * @note  微分项对测量值（PV）求导，避免目标跳变引起的 Derivative Kick
 * @note  积分项采用条件积分抗饱和，输出饱和时停止累加
 */
static float PID_FF_CalcCore(PID_FF_HandleTypeDef *pid, float error, float ff_out) {
    // 1. 比例项
    float p_out = pid->kp * error;

    // 2. 微分项：对测量值(PV)求导，提供真实速度阻尼
    //    d_out = -kd * (current - last_current)
    //    目标变化时电流不会突变，因此不会产生 Derivative Kick
    float d_out = pid->kd * (-(pid->current - pid->last_current));

    // 3. 积分项：条件积分抗饱和
    //    预估总输出，若已饱和且误差方向会加剧饱和则停止积分
    float i_tmp = pid->ki * pid->error_sum;
    float pre_total = p_out + i_tmp + d_out + ff_out;
    bool integrate = true;
    if (pre_total >= pid->output_max && error > 0.0f) {
        integrate = false; // 正向饱和，误差为正，停止积分
    } else if (pre_total <= pid->output_min && error < 0.0f) {
        integrate = false; // 负向饱和，误差为负，停止积分
    }
    if (integrate) {
        pid->error_sum += error;
    }

    float i_out = pid->ki * pid->error_sum;

    // 4. 积分输出硬性限幅（对 i_out 本身限幅，物理意义明确）
    if (i_out > pid->integral_max) {
        i_out = pid->integral_max;
        if (pid->ki > 1e-6f) pid->error_sum = i_out / pid->ki;
    } else if (i_out < pid->integral_min) {
        i_out = pid->integral_min;
        if (pid->ki > 1e-6f) pid->error_sum = i_out / pid->ki;
    }

    // 5. 叠加输出与输出限幅
    float total_out = p_out + i_out + d_out + ff_out;
    if (total_out > pid->output_max) {
        total_out = pid->output_max;
    } else if (total_out < pid->output_min) {
        total_out = pid->output_min;
    }

    // 6. 更新状态
    pid->last_error = error;
    pid->last_current = pid->current;
    pid->output = total_out;

    return total_out;
}

/**
 * @brief 计算前馈 PID 输出，支持外部前馈分量输入
 * @param ff_value 外部前馈控制值
 */
float PID_FF_Calculate(PID_FF_HandleTypeDef *pid, float current_val, float target_val, float ff_value) {
    if (pid == NULL) return 0.0f;
    if (!pid->enable) return pid->output; // 禁用时直接返回上次输出

    pid->current = current_val;
    pid->target = target_val;
    pid->error = pid->target - pid->current;

    return PID_FF_CalcCore(pid, pid->error, ff_value);
}

/**
 * @brief 自动计算前馈量并执行前馈 PID 控制
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

//** ================================================================================ **//
//** ============================ 前馈角度 PID 处理 ================================== **//
//** ================================================================================ **//

/**
 * @brief 计算最短旋转角度误差，结果范围 [-180.0, 180.0)
 */
static inline float PID_AngleShortestError(float from, float to) {
    float err = to - from;
    if (err > 180.0f) err -= 360.0f;
    else if (err < -180.0f) err += 360.0f;
    return err;
}

/**
 * @brief 前馈角度 PID 计算，支持循环角度处理
 */
float PID_FF_Calculate_CycleAngle(PID_FF_HandleTypeDef *pid, float current, float target) {
    if (pid == NULL || !pid->enable) return pid ? pid->output : 0.0f;

    // 1. 计算目标增量前馈
    float delta_target = PID_AngleShortestError(pid->last_target, target);
    float ff_value = pid->kff * delta_target; // kff 用于估计目标变化率

    // 2. 将当前角度统一到 [-180, 180)
    pid->current = fmodf(current, 360.0f);
    if (pid->current >= 180.0f) pid->current -= 360.0f;
    else if (pid->current < -180.0f) pid->current += 360.0f;

    pid->target = fmodf(target, 360.0f);
    if (pid->target >= 180.0f) pid->target -= 360.0f;
    else if (pid->target < -180.0f) pid->target += 360.0f;

    // 3. 计算最短角度误差
    pid->error = PID_AngleShortestError(pid->current, pid->target);

    // 4. 更新历史目标
    pid->last_target = pid->target;

    // 5. 执行前馈 PID 计算并返回结果
    return PID_FF_CalcCore(pid, pid->error, ff_value);
}

//** #################################################################################################### **//
//** ==================================== 复合多环 PID 结构体实现 ======================================== **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ============================= 双环复合 PID 实现 ================================ **//
//** ================================================================================ **//

/**
 * @brief  初始化双环PID复合结构体
 * @param  pid       双环PID结构体指针
 * @param  kp_in     内环比例系数
 * @param  ki_in     内环积分系数
 * @param  kd_in     内环微分系数
 * @param  kp_ex     外环比例系数
 * @param  ki_ex     外环积分系数
 * @param  kd_ex     外环微分系数
 * @param  out_min   输出下限
 * @param  out_max   输出上限
 * @param  int_min   积分累加值下限
 * @param  int_max   积分累加值上限
 * @param  threshold 误差阈值，小于该值时内环停止跟随
 * @param  outer_mode 外环模式（线性/角度）
 */
void PID_Double_Init(PID_Double_t *pid,
                     float kp_in, float ki_in, float kd_in,
                     float kp_ex, float ki_ex, float kd_ex,
                     float out_min, float out_max,
                     float int_min, float int_max,
                     float threshold,
                     PID_OuterMode_t outer_mode)
{
    PID_Init(&pid->inner, kp_in, ki_in, kd_in, out_min, out_max, int_min, int_max);
    PID_Init(&pid->outer, kp_ex, ki_ex, kd_ex, out_min, out_max, int_min, int_max);

    pid->outer_mode          = outer_mode;
    pid->threshold           = threshold;
    pid->inner_override_enable = 0;
    pid->inner_target_override = 0.0f;
}

/**
 * @brief  双环PID计算
 * @note   根据 outer_mode 自动选择线性PID或角度PID(CycleAngle)进行外环计算。
 *         内环目标值支持 override 覆盖模式。
 * @param  pid        双环PID结构体指针
 * @param  target     外环目标值
 * @param  current_in 内环当前反馈值
 * @param  current_ex 外环当前反馈值
 * @retval 内环PID输出值（已限幅）
 */
float PID_Double_Calc(PID_Double_t *pid,
                      float target, float current_in, float current_ex)
{
    // 1. 外环计算：根据模式选择线性或角度PID
    float outer_out;
    if (pid->outer_mode == PID_OUTER_MODE_ANGLE) {
        outer_out = PID_Calculate_CycleAngle(&pid->outer, current_ex, target);
    } else {
        outer_out = PID_Calculate(&pid->outer, current_ex, target);
    }
    pid->outer.Output = outer_out;

    // 2. 判断内环目标值
    float inner_target = pid->inner_override_enable
                         ? pid->inner_target_override
                         : outer_out;

    // 3. 阈值判断后执行内环
    float inner_out = 0.0f;
    if (fabsf(current_ex - target) > pid->threshold) {
        inner_out = PID_Calculate(&pid->inner, current_in, inner_target);
    }
    pid->inner.Output = inner_out;

    return inner_out;
}

//** ================================================================================ **//
//** ============================= 三环复合 PID 实现 ================================ **//
//** ================================================================================ **//

/**
 * @brief  初始化三环PID复合结构体
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
 * @param  out_min         输出下限
 * @param  out_max         输出上限
 * @param  int_min         积分累加值下限
 * @param  int_max         积分累加值上限
 * @param  max_angle_error 角度环误差阈值
 * @param  outer_mode      最外层模式（线性/角度）
 */
void PID_Triple_Init(PID_Triple_t *pid,
                     float kp_cur, float ki_cur, float kd_cur,
                     float kp_spd, float ki_spd, float kd_spd,
                     float kp_ang, float ki_ang, float kd_ang,
                     float out_min, float out_max,
                     float int_min, float int_max,
                     float max_angle_error,
                     PID_OuterMode_t outer_mode)
{
    PID_Init(&pid->current, kp_cur, ki_cur, kd_cur, out_min, out_max, int_min, int_max);
    PID_Init(&pid->speed,   kp_spd, ki_spd, kd_spd, out_min, out_max, int_min, int_max);
    PID_Init(&pid->angle,   kp_ang, ki_ang, kd_ang, out_min, out_max, int_min, int_max);

    pid->outer_mode          = outer_mode;
    pid->max_angle_error     = max_angle_error;
    pid->inner_override_enable = 0;
    pid->inner_target_override = 0.0f;
}

/**
 * @brief  三环PID计算（角度环 → 速度环 → 电流环）
 * @note   最外层根据 outer_mode 自动选择线性或角度PID，
 *         中间层与最内层始终线性PID，最内层支持 override 覆盖模式。
 * @param  pid              三环PID结构体指针
 * @param  target_angle     角度环目标值
 * @param  current_angle    角度环当前反馈值
 * @param  current_speed    速度环当前反馈值
 * @param  current_current  电流环当前反馈值
 * @retval 最内层(电流环)PID输出值（已限幅）
 */
float PID_Triple_Calc(PID_Triple_t *pid,
                      float target_angle, float current_angle,
                      float current_speed, float current_current)
{
    pid->angle.current   = current_angle;
    pid->speed.current   = current_speed;
    pid->current.current = current_current;

    // 1. 最外层(角度环)：根据模式选择线性或角度PID
    float angle_out;
    if (pid->outer_mode == PID_OUTER_MODE_ANGLE) {
        angle_out = PID_Calculate_CycleAngle(&pid->angle, current_angle, target_angle);
    } else {
        angle_out = PID_Calculate(&pid->angle, current_angle, target_angle);
    }
    pid->angle.Output = angle_out;

    // 2. 中间层(速度环)：始终线性PID
    float speed_out = 0.0f;
    if (fabsf(target_angle - current_angle) > pid->max_angle_error) {
        speed_out = PID_Calculate(&pid->speed, current_speed, angle_out);
    }
    pid->speed.Output = speed_out;

    // 3. 最内层(电流环)：支持override，始终线性PID
    float inner_target = pid->inner_override_enable
                         ? pid->inner_target_override
                         : speed_out;
    float current_out = PID_Calculate(&pid->current, current_current, inner_target);
    pid->current.Output = current_out;

    return current_out;
}

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
void PID_Double_SetInnerOverride(PID_Double_t *pid, float target)
{
    pid->inner_target_override = target;
    pid->inner_override_enable = 1;
}

/**
 * @brief  禁用内环目标值覆盖模式
 * @note   恢复内环跟随外环输出的级联模式。
 * @param  pid 双环PID结构体指针
 */
void PID_Double_DisableInnerOverride(PID_Double_t *pid)
{
    pid->inner_override_enable = 0;
}

/**
 * @brief  读取外环输出中间量
 * @param  pid 双环PID结构体指针
 * @retval 外环PID当前输出值
 */
float PID_Double_GetOuterOutput(PID_Double_t *pid)
{
    return pid->outer.Output;
}

/**
 * @brief  读取内环实际目标值
 * @param  pid 双环PID结构体指针
 * @retval 内环当前实际使用的目标值（覆盖值 or 外环输出）
 */
float PID_Double_GetInnerTarget(PID_Double_t *pid)
{
    return pid->inner_override_enable
           ? pid->inner_target_override
           : pid->outer.Output;
}

// === 三环调参辅助 ===

/**
 * @brief  启用三环最内层目标值覆盖模式
 * @note   启用后电流环使用手动设定的固定目标值，用于由内到外分步调参。
 * @param  pid    三环PID结构体指针
 * @param  target 最内层(电流环)手动目标值
 */
void PID_Triple_SetInnerOverride(PID_Triple_t *pid, float target)
{
    pid->inner_target_override = target;
    pid->inner_override_enable = 1;
}

/**
 * @brief  禁用三环最内层目标值覆盖模式
 * @note   恢复电流环跟随速度环输出的级联模式。
 * @param  pid 三环PID结构体指针
 */
void PID_Triple_DisableInnerOverride(PID_Triple_t *pid)
{
    pid->inner_override_enable = 0;
}

/**
 * @brief  读取三环最外层输出中间量
 * @param  pid 三环PID结构体指针
 * @retval 角度环PID当前输出值
 */
float PID_Triple_GetOuterOutput(PID_Triple_t *pid)
{
    return pid->angle.Output;
}

/**
 * @brief  读取三环最内层实际目标值
 * @param  pid 三环PID结构体指针
 * @retval 最内层(电流环)当前实际使用的目标值
 */
float PID_Triple_GetInnerTarget(PID_Triple_t *pid)
{
    return pid->inner_override_enable
           ? pid->inner_target_override
           : pid->speed.Output;
}

