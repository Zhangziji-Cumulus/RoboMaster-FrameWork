#ifndef CHASSIS_ADVANCE_
#define CHASSIS_ADVANCE_

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define M_PI 3.14159265358979323846

// 全局静态变量，保持状态（1ms中断调用）
typedef struct
{
    float current_speed;      // 当前平滑输出速度
    float phase;              // 波形相位
    float period;             // 当前随机周期 (秒)
    float base_amp;           // 当前随机振幅
    float alpha;              // 一阶滤波系数
    float min_spd;
    float max_spd;
    uint32_t tick;            // 计时tick(1ms)
    uint32_t period_tick;     // 当前周期总tick数
} GyroSmoothRand_t;

void GyroSmoothRand_Init(float min_speed, float max_speed);
float GyroSmoothRand_Run(void);

#endif // CHASSIS_ADVANCE_