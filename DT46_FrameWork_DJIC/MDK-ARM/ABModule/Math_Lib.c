#include "Math_Lib.h"

//** #################################################################################################### **//
//** ========================================== 角度转化类 =============================================== **//
//** #################################################################################################### **//

/**
 * @brief  将角度转化为弧度
 * 
 * @param degrees 角度值（单位度）
 * @return float 返回弧度制浮点数
 */
float MyMath_Degrees_To_Radians(float degrees) {
    return degrees * MY_PI / 180.0;
}

/**
 * @brief 将弧度转化为角度
 * 
 * @param rad 弧度值
 * @return float 返回角度制浮点数
 */
float MyMath_Radians_To_Degrees(float rad) {
    return rad * (180.0 / MY_PI);
}

/**
 * @brief 规范化到 [0, 360)
 * 
 * @param angle 输入角度
 * @return double 规范化后的角度，范围 [0, 360)
 */
double MyMath_normalize_0_to_360(double angle) {
    angle = fmod(angle, 360.0);
    return angle < 0.0 ? angle + 360.0 : angle;
}

/**
 * @brief 规范化到 (-180, 180]  (与 atan2 返回值范围一致)
 * 
 * @param angle 输入角度
 * @return double 规范化后的角度，范围 (-180, 180]
 */
double MyMath_normalize_m180_to_p180(double angle) {
    angle = fmod(angle, 360.0);
    if (angle >  180.0) angle -= 360.0;
    if (angle <= -180.0) angle += 360.0;
    return angle;
}


//** #################################################################################################### **//
//** ======================================== 范围规范、限制类 =========================================== **//
//** #################################################################################################### **//

/**
 * @brief 将浮点数 “限制” 在范围[min, max]内，支持循环模式
 * 
 * @param value 待处理的浮点数
 * @param min 范围最小值
 * @param max 范围最大值
 * @param is_cycle 是否启用循环模式（1=循环，0=普通限制）
 * @return 处理后的浮点数
 */
float MyMath_Limit_Float(float value, float min, float max, int is_cycle) {
    // 计算范围区间长度
    float range = max - min;
    
    // 避免区间长度为0导致异常
    if (range <= 0) {
        return min; // 若max <= min，直接返回min
    }
    
    if (is_cycle) {
        // 循环模式：超出范围时从另一端循环
        // 先将值映射到[min, max)区间，再处理边界
        value = fmod(value - min, range);
        if (value < 0) {
            value += range; // 处理负值情况
        }
        return value + min;
    } else {
        // 普通限制模式：超出范围时钳位到边界
        if (value < min) {
            return min;
        } else if (value > max) {
            return max;
        } else {
            return value;
        }
    }
}

/**
 * @brief 将输入浮点数 “ 映射 ” 到目标范围[target_min, target_max]
 * 
 * @param input 输入的浮点数
 * @param input_min 输入值的最小值范围
 * @param input_max 输入值的最大值范围
 * @param target_min 目标范围的最小值
 * @param target_max 目标范围的最大值
 * @return 映射后的浮点数
 */
float MyMath_Map_Range_Float(float input, float input_min, float input_max, float target_min, float target_max) {
    // 处理输入范围相同的特殊情况，避免除零
    if (input_min == input_max) {
        return (target_min + target_max) / 2.0f;
    }
    
    // 计算输入值在输入范围内的比例（0.0~1.0）
    float ratio = (input - input_min) / (input_max - input_min);
    
    // 将比例映射到目标范围
    return target_min + ratio * (target_max - target_min);
}

/**
 * @brief 将整数 "限制" 在范围[min, max]内，支持循环模式
 * 
 * @param value 待处理的整数
 * @param min 范围最小值
 * @param max 范围最大值
 * @param is_cycle 是否启用循环模式（1=循环，0=普通限制）
 * @return 处理后的 int16_t 整数
 */
int16_t MyMath_Limit_Int16(int16_t value, int16_t min, int16_t max, int is_cycle) {
    // 计算范围区间长度
    int32_t range = (int32_t)max - (int32_t)min;
    
    // 避免区间长度为0导致异常
    if (range <= 0) {
        return min; // 若max <= min，直接返回min
    }
    
    if (is_cycle) {
        // 循环模式：超出范围时从另一端循环
        // 使用 int32_t 避免溢出
        int32_t val = (int32_t)value - (int32_t)min;
        
        // 手动实现取模运算，处理负数情况
        val = val % range;
        if (val < 0) {
            val += range;
        }
        
        int32_t result = val + (int32_t)min;
        
        // 确保结果在 int16_t 范围内
        if (result > INT16_MAX) return INT16_MAX;
        if (result < INT16_MIN) return INT16_MIN;
        
        return (int16_t)result;
    } else {
        // 普通限制模式：超出范围时钳位到边界
        if (value < min) {
            return min;
        } else if (value > max) {
            return max;
        } else {
            return value;
        }
    }
}

/**
 * @brief 将输入整数 "映射" 到目标范围[target_min, target_max]
 * 
 * @param input 输入的整数
 * @param input_min 输入值的最小值范围
 * @param input_max 输入值的最大值范围
 * @param target_min 目标范围的最小值
 * @param target_max 目标范围的最大值
 * @return 映射后的 int16_t 整数
 */
int16_t MyMath_Map_Range_Int16(int16_t input, int16_t input_min, int16_t input_max, int16_t target_min, int16_t target_max) {
    // 处理输入范围相同的特殊情况，避免除零
    if (input_min == input_max) {
        return (int16_t)((int32_t)target_min + (int32_t)target_max) / 2;
    }
    
    // 使用 int32_t 进行中间计算，避免溢出
    int32_t input_range = (int32_t)input_max - (int32_t)input_min;
    int32_t target_range = (int32_t)target_max - (int32_t)target_min;
    
    // 计算输入值在输入范围内的偏移量
    int32_t input_offset = (int32_t)input - (int32_t)input_min;
    
    // 执行映射计算：target_min + (input_offset * target_range / input_range)
    // 先乘后除以保持精度
    int32_t mapped_value = (int32_t)target_min + (input_offset * target_range / input_range);
    
    // 确保结果在 int16_t 范围内
    if (mapped_value > INT16_MAX) return INT16_MAX;
    if (mapped_value < INT16_MIN) return INT16_MIN;
    
    return (int16_t)mapped_value;
}

/**
 * @brief 对输入整数进行比例缩放，结果限制在[min, max]范围内
 * 
 * @param input 输入值
 * @param scale 缩放比例
 * @param min   最小值
 * @param max   最大值
 * @return int16_t 缩放并限幅后的结果
 */
int16_t MyMath_Scale_Int16(int16_t input, float scale, int16_t min, int16_t max)
{
    float result = (float)input * scale;
    
    if (result > (float)max) return max;
    if (result < (float)min) return min;
    
    return (int16_t)result;
}

//** #################################################################################################### **//
//** ============================================ 功能类 ================================================ **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ================================== 角度类 ====================================== **//
//** ================================================================================ **//

/**
 * @brief 计算最短路径角度差（控制算法必备）
 * 
 * @param target 目标角度
 * @param current 当前角度
 * @return 差值，范围 (-180, 180]，正=逆时针，负=顺时针
 */
double MyMath_angle_diff_shortest(double target, double current) {
    double diff = fmod(target - current, 360.0);
    if (diff >  180.0) diff -= 360.0;
    if (diff <= -180.0) diff += 360.0;
    return diff;
}

/**
 * @brief 传入一个0-360度的角度，返回累计值
 * 
 * @param current_angle 当前角度
 * @return float 累计角度，范围不限制
 */
float MyMath_get_accumulated_angle(float current_angle) {
    // 静态变量：保存上一次角度和累计圈数（仅初始化一次）
    static float last_angle = -1.0f;  // 初始值设为无效值，标记首次调用
    static int total_cycles = 0;      // 累计圈数（正转+，反转-）
    
    // 首次调用：初始化上一次角度（无需计算圈数）
    if (last_angle < 0.0f) {
        last_angle = current_angle;
        return current_angle;
    }
    
    // 计算当前与上一次角度的差值
    float delta = current_angle - last_angle;
    
    // 判断圈数变化（阈值180°：超过半圈的跳变视为跨圈）
    if (delta > 180.0f) {
        // 例：350°→10°，实际多转1圈，但delta= -340°，此处修正为减1圈
        total_cycles--;
    } else if (delta < -180.0f) {
        // 例：10°→350°，实际少转1圈，但delta= 340°，此处修正为加1圈
        total_cycles++;
    }
    
    // 更新上一次角度
    last_angle = current_angle;
    
    // 计算累计总角度：当前角度 + 圈数×360°
    return current_angle + total_cycles * 360.0f;
}

/**
 * @brief 知道当前编码器值和减速比求输出轴角度(放到1ms里计算),输出角度范围（-180°，180°）
 * 
 * @param current_angle 
 * @param gear_ratio 
 * @return double 
 */
// 功能：电机单圈角度(-180~180 或 0~360) → 经过减速比 → 输出轴角度(-180~180)
// 1ms 中断安全调用，无跳变、无飞 angle、多圈累计正确
double MyMath_cal_output_angle(double current_angle, uint16_t gear_ratio)
{
    static double last_angle  = 0.0;   // 上一次电机角度（全用double，不丢精度）
    static int    total_cycles = 0;    // 电机累计圈数（正转+，反转-）
    static int    first_run    = 1;    // 首次运行标记

    // 首次调用，直接记录并返回
    if (first_run)
    {
        first_run = 0;
        last_angle = current_angle;
        return MyMath_normalize_m180_to_p180(current_angle / gear_ratio);
    }

    // 计算角度差
    double delta = current_angle - last_angle;

    // ========== 跨圈判断（标准正确写法）==========
    if (delta > 180.0)       // 350° → 10° 跳变，反转过零
    {
        total_cycles--;
    }
    else if (delta < -180.0) // 10° → 350° 跳变，正转过零
    {
        total_cycles++;
    }

    // 更新上一次角度
    last_angle = current_angle;

    // ========== 电机总绝对角度 ==========
    double motor_total_angle = current_angle + total_cycles * 360.0;

    // ========== 输出轴角度 = 电机角度 / 减速比 ==========
    double output_angle = motor_total_angle / gear_ratio;

    // ========== 归一到 -180 ~ 180（唯一正确输出）==========
    return MyMath_normalize_m180_to_p180(output_angle);
}

//** ================================================================================ **//
//** ============================ 轮子转速和线速度 =================================== **//
//** ================================================================================ **//

//输入值达到32767时会溢出，从32767到-32767
float RPM_Protect(float rpm)
{
	if(rpm >= 32767.0f)
	{
		rpm = 32767.0f;
	}
	else if(rpm <= -32767.0f)
	{
		rpm = -32767.0f;
	}
		
	return rpm;
}

/**
 * @brief 线速度(m/s) → 电机目标转速(rpm)
 * @param speed_mps     目标线速度，单位：m/s
 * @param radius_m      轮子半径，单位：m
 * @param gear_ratio    减速比（电机转速/轮子转速），如 10:1 则填 10.0
 * @return              电机目标转速，单位：rpm
 */
float calc_motor_rpm_from_speed(float speed_mps, float radius_m, float gear_ratio)
{
    if (radius_m <= 0.0f || gear_ratio <= 0.0f) return 0.0f;
    
    // 轮子转速 × 减速比 = 电机转速
    float wheel_rpm = (speed_mps * 60.0f) / (2.0f * MY_PI * radius_m);
    float motor_rpm = wheel_rpm * gear_ratio;
    
    return RPM_Protect(motor_rpm); // ?? 限幅必须基于电机最大允许转速
}

/**
 * @brief 电机转速(rpm) → 实际线速度(m/s)
 * @param radius_m      轮子半径，单位：m
 * @param motor_rpm     电机实际转速（来自编码器），单位：rpm
 * @param gear_ratio    减速比
 * @return              线速度，单位：m/s
 */
float calc_speed_from_motor_rpm(float radius_m, float motor_rpm, float gear_ratio)
{
    if (radius_m <= 0.0f || gear_ratio <= 0.0f) return 0.0f;
    
    float wheel_rpm = motor_rpm / gear_ratio;
    float circumference = 2.0f * MY_PI * radius_m;
    
    return (circumference * wheel_rpm) / 60.0f;
}