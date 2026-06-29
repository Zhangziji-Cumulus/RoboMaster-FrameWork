#ifndef __HERO_MATH_H
#define __HERO_MATH_H

#define MY_PI 3.14159265358979323846

//输入值达到32767时会溢出，从32767到-32767
float RPM_Protect(float rpm);

/**
 * @brief 计算轮子直线运动速度
 * @param radius 轮子半径，单位：m
 * @param rpm    轮子转速，单位：转/分钟
 * @return       线速度，单位：m/s
 */
float calc_wheel_speed(float radius, float rpm);

/**
 * @brief 由线速度和轮子半径求转速
 * @param speed   线速度，单位：m/s
 * @param radius  轮子半径，单位：m
 * @return        转速，单位：转/分钟 (rpm)
 */
float calc_wheel_rpm(float speed, float radius);


#endif
