#include "My_Math.h"
#include <float.h>
#include <stdint.h>

/**
 * @brief 将浮点数限制在范围[min, max]内，支持循环模式
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
 * 将输入浮点数映射到目标范围[target_min, target_max]
 * @param input 输入的浮点数
 * @param input_min 输入值的最小值范围
 * @param input_max 输入值的最大值范围
 * @param target_min 目标范围的最小值
 * @param target_max 目标范围的最大值
 * @return 映射后的浮点数
 */
float MyMath_Map_Range(float input, float input_min, float input_max, float target_min, float target_max) {
    // 处理输入范围相同的特殊情况，避免除零
    if (input_min == input_max) {
        return (target_min + target_max) / 2.0f;
    }
    
    // 计算输入值在输入范围内的比例（0.0~1.0）
    float ratio = (input - input_min) / (input_max - input_min);
    
    // 将比例映射到目标范围
    return target_min + ratio * (target_max - target_min);
}

/* 将角度转化为弧度 */
float MyMath_Degrees_To_Radians(float degrees) {
    return degrees * MY_PI / 180.0;
}
// 弧度转角度
float MyMath_Radians_To_Degrees(float rad) {
    return rad * (180.0 / MY_PI);
}

/* 传入一个0-360度的角度，返回累计值 */
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
 * @brief 规范化到 [0, 360)
 */
double MyMath_normalize_0_to_360(double angle) {
    angle = fmod(angle, 360.0);
    return angle < 0.0 ? angle + 360.0 : angle;
}

/**
 * @brief 规范化到 (-180, 180]  (与 atan2 返回值范围一致)
 */
double MyMath_normalize_m180_to_p180(double angle) {
    angle = fmod(angle, 360.0);
    if (angle >  180.0) angle -= 360.0;
    if (angle <= -180.0) angle += 360.0;
    return angle;
}

/**
 * @brief 计算最短路径角度差（控制算法必备）
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

//知道当前编码器值和减速比求输出轴角度(放到1ms里计算),输出角度范围（-180°，180°）
double MyMath_cal_output_angle(double current_angle,uint16_t gear_ratio)
{
		static double Last_Angle = 0.0;
		static double changeAngle = 0.0;
		static double add_angle = 0.0;
	
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
    add_angle = current_angle + total_cycles * 360.0f;
	
		if(add_angle >= 360 * gear_ratio || add_angle <= -360 * gear_ratio)
		{
			total_cycles = 0;
		}
		
		add_angle	= MyMath_normalize_m180_to_p180(add_angle / gear_ratio);
	
	return add_angle;
}
