#include "HERO_Math.h"

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
 * @brief 计算轮子直线运动速度
 * @param radius 轮子半径，单位：m
 * @param rpm    轮子转速，单位：转/分钟
 * @return       线速度，单位：m/s
 */
float calc_wheel_speed(float radius, float rpm)
{
    // 周长
    float circumference = 2 * MY_PI * radius;
    // 每分钟行驶距离
    float dist_per_min = circumference * rpm;
    // 转换为每秒速度
    float speed = dist_per_min / 60.0f;

    return speed;
}

/**
 * @brief 由线速度和轮子半径求转速
 * @param speed   线速度，单位：m/s
 * @param radius  轮子半径，单位：m
 * @return        转速，单位：转/分钟 (rpm)
 */
float calc_wheel_rpm(float speed, float radius)
{
    if (radius <= 0.0f)
    {
        return 0.0f; // 防止除0
    }

    float rpm = (speed * 60.0f) / (2 * MY_PI * radius);
		
		rpm = RPM_Protect(rpm);
		
    return rpm;
}
