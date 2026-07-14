#include "Chassis_Advanced.h"
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stddef.h>

GyroSmoothRand_t gyro_rand;
static uint32_t lfsr_rand = 0;

static uint32_t LFSR_Next(void)
{
    lfsr_rand ^= lfsr_rand >> 13;
    lfsr_rand ^= lfsr_rand << 17;
    lfsr_rand ^= lfsr_rand >> 5;
    return lfsr_rand;
}

static float GetRandFloat(float min, float max)
{
    uint32_t r = LFSR_Next();
    float f = (float)r / (float)UINT32_MAX;
    return min + f * (max - min);
}

void GyroSmoothRand_Init(void)
{
    lfsr_rand = xTaskGetTickCount() ^ (uint32_t)&gyro_rand;

    // ===================== 反自瞄突降波形 默认参数 =====================
    gyro_rand.param.alpha          = 0.02f;
    gyro_rand.param.min_spd        = 1.0f;
    gyro_rand.param.max_spd        = 1.5f;
    gyro_rand.param.min_period     = 1.5f;
    gyro_rand.param.max_period     = 2.5f;
    gyro_rand.param.amp_min_ratio  = 0.20f;
    gyro_rand.param.amp_max_ratio  = 0.35f;
    gyro_rand.param.noise_ratio    = 0.05f;
    gyro_rand.param.drop_ratio     = 0.12f;    // 12%周期完成陡降，突降很急促
    gyro_rand.param.drop_depth_ratio = 1.0f;   // 降到本轮波动最大谷底
    // =================================================================

    gyro_rand.current_speed = (gyro_rand.param.min_spd + gyro_rand.param.max_spd) * 0.5f;
    gyro_rand.phase         = 0.0f;
    gyro_rand.tick          = 0;

    float rand_period = GetRandFloat(gyro_rand.param.min_period, gyro_rand.param.max_period);
    gyro_rand.period = rand_period;
    gyro_rand.period_tick = (uint32_t)(rand_period / DT_SEC);

    gyro_rand.base_amp = (gyro_rand.param.max_spd - gyro_rand.param.min_spd)
                       * GetRandFloat(gyro_rand.param.amp_min_ratio, gyro_rand.param.amp_max_ratio);
}

float GyroSmoothRand_Run(void)
{
    GyroSmoothRand_Param_t *p = &gyro_rand.param;
    gyro_rand.tick++;

    // 周期重置
    if(gyro_rand.tick >= gyro_rand.period_tick)
    {
        gyro_rand.tick = 0;
        float new_period = GetRandFloat(p->min_period, p->max_period);
        gyro_rand.period = new_period;
        gyro_rand.period_tick = (uint32_t)(new_period / DT_SEC);
        gyro_rand.base_amp = (p->max_spd - p->min_spd)
                           * GetRandFloat(p->amp_min_ratio, p->amp_max_ratio);
        gyro_rand.phase = 0.0f; // phase 归一化：0 ~ 1
    }

    // 归一化相位 0~1
    float phase_norm = (float)gyro_rand.tick / (float)gyro_rand.period_tick;
    float wave = 0.0f;

    // ========== 非对称波形逻辑：高速 -> 快速突降 -> 缓慢回升 ==========
    if (phase_norm < p->drop_ratio)
    {
        // 阶段1：快速陡降
        float t = phase_norm / p->drop_ratio;
        wave = 1.0f - t * p->drop_depth_ratio;
    }
    else
    {
        // 阶段2：缓慢回升回到高速
        float t = (phase_norm - p->drop_ratio) / (1.0f - p->drop_ratio);
        // 缓升余弦曲线，恢复到高位速度
        wave = -cosf(t * M_PI) * 0.5f + 0.5f;
    }
    wave = (wave - 0.5f) * 2.0f * gyro_rand.base_amp;
    // ================================================================

    // 微小随机扰动
    float small_rand = (p->max_spd - p->min_spd) * p->noise_ratio * GetRandFloat(-0.5f, 0.5f);
    float mid_spd = (p->min_spd + p->max_spd) / 2.0f;
    float target_spd = mid_spd + wave + small_rand;

    // 硬限幅
    if(target_spd < p->min_spd) target_spd = p->min_spd;
    if(target_spd > p->max_spd) target_spd = p->max_spd;

    // 一阶滤波顺滑
    gyro_rand.current_speed = p->alpha * target_spd + (1 - p->alpha) * gyro_rand.current_speed;

    return gyro_rand.current_speed;
}