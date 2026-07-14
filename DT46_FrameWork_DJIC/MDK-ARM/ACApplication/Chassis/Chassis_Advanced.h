#ifndef __CHASSIS_ADVANCED_H
#define __CHASSIS_ADVANCED_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DT_SEC 0.001f          // 调用周期：1ms = 0.001秒
#define M_PI 3.14159265358979323846f

/**
 * @brief 可在线调试参数结构体
 * @note  可在IDE调试监视窗口实时修改参数，运行时即时生效
 */
typedef struct
{
    float alpha;              // 一阶滤波系数
    float min_spd;            // 整体速度下限
    float max_spd;            // 整体速度上限
    float min_period;         // 周期下限(秒)
    float max_period;         // 周期上限(秒)
    float amp_min_ratio;      // 波动振幅最小占比
    float amp_max_ratio;      // 波动振幅最大占比
    float noise_ratio;        // 细微扰动强度系数
    // 新增非对称波形参数
    float drop_ratio;         // 降速阶段占整个周期的比例 (0~0.5)，越小=降速越急促
    float drop_depth_ratio;   // 最低谷深度占基础波动振幅的比例
} GyroSmoothRand_Param_t;

/**
 * @brief 主结构体
 * @param param         可调试参数区（在线调参区域）
 * @param current_speed 最终滤波平滑输出速度
 * @param phase         波形相位(0~1归一化)
 * @param period        当前周期(秒)
 * @param base_amp      当前主波形振幅
 * @param tick          本地1ms计时计数器
 * @param period_tick   当前周期总tick计数
 * @note  非param成员为自动运行状态变量，不建议手动修改！
 */
typedef struct
{
    GyroSmoothRand_Param_t param;

    // ========== 自动运行状态变量（请勿手动修改） ==========
    float current_speed;
    float phase;
    float period;
    float base_amp;
    uint32_t tick;
    uint32_t period_tick;
    // ======================================================
} GyroSmoothRand_t;

// 全局主结构体实例
extern GyroSmoothRand_t gyro_rand;

// 函数声明
void GyroSmoothRand_Init(void);
float GyroSmoothRand_Run(void);

#ifdef __cplusplus
}
#endif

#endif