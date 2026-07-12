#include "Chassis_Advanced.h"

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

static GyroSmoothRand_t gyro_rand = {0};
const float dt = 0.001f;    // 1ms

// 初始化函数，上电调用一次
void GyroSmoothRand_Init(float min_speed, float max_speed)
{
    srand((unsigned int)time(NULL));
    gyro_rand.min_spd = min_speed;
    gyro_rand.max_spd = max_speed;
    gyro_rand.alpha = 0.03f; // 滤波系数，越小越丝滑、响应越慢
    gyro_rand.current_speed = (min_speed + max_speed) * 0.5f;
    gyro_rand.phase = 0.0f;
    gyro_rand.tick = 0;
    // 初始随机周期：0.5 ~ 3 秒
    float rand_period = 0.5f + 2.5f * ((float)rand() / RAND_MAX);
    gyro_rand.period = rand_period;
    gyro_rand.period_tick = (uint32_t)(rand_period / dt);
    // 随机振幅
    gyro_rand.base_amp = (max_speed - min_speed) * (0.3f + 0.4f * ((float)rand() / RAND_MAX));
}

// 每1ms调用一次，返回平滑随机变速
float GyroSmoothRand_Run(void)
{
    gyro_rand.tick++;

    // 到达周期终点：重新随机周期和振幅
    if(gyro_rand.tick >= gyro_rand.period_tick)
    {
        gyro_rand.tick = 0;
        // 新随机周期：0.5 ~ 3.0 秒
        float new_period = 0.5f + 2.5f * ((float)rand() / RAND_MAX);
        gyro_rand.period = new_period;
        gyro_rand.period_tick = (uint32_t)(new_period / dt);
        // 新随机振幅
        gyro_rand.base_amp = (gyro_rand.max_spd - gyro_rand.min_spd) * (0.3f + 0.4f * ((float)rand() / RAND_MAX));
        // 重置相位
        gyro_rand.phase = 0.0f;
    }

    // 更新相位
    float omega = 2.0f * (float)M_PI / gyro_rand.period;
    gyro_rand.phase += omega * dt;

    // 主慢变正弦 + 小幅随机扰动（增加自然忽快忽慢）
    float main_wave = sinf(gyro_rand.phase) * gyro_rand.base_amp;
    // 叠加微小随机噪声（低频慢扰动）
    float small_rand = (0.1f * (gyro_rand.max_spd - gyro_rand.min_spd)) * ((float)rand()/RAND_MAX - 0.5f);
    float target_spd = (gyro_rand.min_spd + gyro_rand.max_spd)/2.0f + main_wave + small_rand;

    // 硬限幅保证不超出范围
    if(target_spd < gyro_rand.min_spd) target_spd = gyro_rand.min_spd;
    if(target_spd > gyro_rand.max_spd) target_spd = gyro_rand.max_spd;

    // 一阶低通滤波丝滑过渡
    gyro_rand.current_speed = gyro_rand.alpha * target_spd + (1 - gyro_rand.alpha) * gyro_rand.current_speed;

    return gyro_rand.current_speed;
}
