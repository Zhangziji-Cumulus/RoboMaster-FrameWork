#ifndef MATH_LIB_H_
#define MATH_LIB_H_

/* system includes */
#include "math.h"
#include <stdint.h>
#include <float.h>
#include <stdint.h>

/* define */
#define MY_PI 3.14159265358979323846

/* extern function declarations */

//** #################################################################################################### **//
//** ========================================== 角度转化类 =============================================== **//
//** #################################################################################################### **//

/* @brief  将角度转化为弧度 */
float MyMath_Degrees_To_Radians(float degrees);

/* @brief 将弧度转化为角度 */
float MyMath_Radians_To_Degrees(float rad);

/* @brief 规范化到 [0, 360) */
double MyMath_normalize_0_to_360(double angle);

/* @brief 规范化到 (-180, 180]  (与 atan2 返回值范围一致) */
double MyMath_normalize_m180_to_p180(double angle);


//** #################################################################################################### **//
//** ======================================== 范围规范、限制类 =========================================== **//
//** #################################################################################################### **//

/* @brief 将浮点数 "限制" 在范围[min, max]内，支持循环模式 */
float MyMath_Limit_Float(float value, float min, float max, int is_cycle);

/* @brief 将输入浮点数 " 映射 " 到目标范围[target_min, target_max] */
float MyMath_Map_Range_Float(float input, float input_min, float input_max, float target_min, float target_max);

/* @brief 将整数 "限制" 在范围[min, max]内，支持循环模式 */
int16_t MyMath_Limit_Int16(int16_t value, int16_t min, int16_t max, int is_cycle);

/* @brief 将输入整数 "映射" 到目标范围[target_min, target_max] */
int16_t MyMath_Map_Range_Int16(int16_t input, int16_t input_min, int16_t input_max, int16_t target_min, int16_t target_max);

//** #################################################################################################### **//
//** ============================================ 功能类 ================================================ **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ================================== 角度类 ====================================== **//
//** ================================================================================ **//

/* @brief 计算最短路径角度差（控制算法必备）*/
double MyMath_angle_diff_shortest(double target, double current);

/* @brief 传入一个0-360度的角度，返回累计值 */
float MyMath_get_accumulated_angle(float current_angle);

/* @brief 知道当前编码器值和减速比求输出轴角度(放到1ms里计算),输出角度范围（-180°，180°）*/
double MyMath_cal_output_angle(double current_angle,uint16_t gear_ratio);

//** ================================================================================ **//
//** ============================ 轮子转速和线速度 =================================== **//
//** ================================================================================ **//

/* 输入值达到32767时会溢出，从32767到-32767 */
float RPM_Protect(float rpm);
/* 线速度(m/s) → 电机目标转速(rpm)*/
float calc_motor_rpm_from_speed(float speed_mps, float radius_m, float gear_ratio);
/* 电机转速(rpm) → 实际线速度(m/s) */
float calc_speed_from_motor_rpm(float radius_m, float motor_rpm, float gear_ratio);

#endif // MATH_LIB_H_
