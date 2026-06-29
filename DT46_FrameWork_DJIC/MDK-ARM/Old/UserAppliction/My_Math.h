#ifndef ___MY_MATH_H
#define ___MY_MATH_H

#include "math.h"
#include <stdint.h>
#define MY_PI 3.14159265358979323846

float MyMath_Limit_Float(float value, float min, float max, int is_cycle);
float MyMath_Map_Range(float input, float input_min, float input_max, float target_min, float target_max);
float MyMath_Degrees_To_Radians(float degrees);
float MyMath_Radians_To_Degrees(float rad);

double MyMath_normalize_0_to_360(double angle);//规范化到 [0, 360)
double MyMath_normalize_m180_to_p180(double angle);//规范化到 (-180, 180]  (与 atan2 返回值范围一致)
double MyMath_angle_diff_shortest(double target, double current);//计算最短路径角度差（控制算法必备）
float  MyMath_get_accumulated_angle(float current_angle);/* 传入一个0-360度的角度，返回累计值 */

double MyMath_cal_output_angle(double CurentAngle,uint16_t gear_ratio);
#endif 
