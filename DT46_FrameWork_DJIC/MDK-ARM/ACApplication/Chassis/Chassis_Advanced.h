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
    /**
     * @brief 一阶低通滤波系数，控制底盘变速顺滑度和反应快慢；
     *        数值越小速度变化越平缓顺滑、延迟越高，数值越大响应越快但顺滑度下降，
     *        例：0.02代表缓慢平滑变速，0.1代表响应更快
     */
    float alpha;

    /**
     * @brief 底盘运行速度下限，底盘速度全程不会低于这个数值，保证基础移动速度，
     *        例：1.0f 意味着底盘速度始终 ≥1.0，不会继续变慢卡死
     */
    float min_spd;

    /**
     * @brief 底盘运行速度上限，底盘速度全程不会高于这个数值，防止超速失控，
     *        例：1.5f 意味着底盘速度始终 ≤1.5，不会超出设定最大速度
     */
    float max_spd;

    /**
     * @brief 一轮完整「高速-突降-回升」变速动作的最短总时长（单位：秒），控制节奏不会变得太快，
     *        例：1.5f 代表一轮动作最快1.5秒走完
     */
    float min_period;

    /**
     * @brief 一轮完整「高速-突降-回升」变速动作的最长总时长（单位：秒），控制节奏不会变得太慢，
     *        和min_period差值越大节奏越随机、越难被预判，例：2.5f 代表一轮动作最慢2.5秒走完
     */
    float max_period;

    /**
     * @brief 主慢变速波动幅度占整体速度差值(max_spd-min_spd)的最小百分比，决定慢晃动幅度下限，
     *        例：总速度差0.5，amp_min_ratio=0.2，则最小波动幅度=0.5*0.2=0.1
     */
    float amp_min_ratio;

    /**
     * @brief 主慢变速波动幅度占整体速度差值(max_spd-min_spd)的最大百分比，决定慢晃动幅度上限，
     *        例：总速度差0.5，amp_max_ratio=0.35，则最大波动幅度=0.5*0.35=0.175
     */
    float amp_max_ratio;

    /**
     * @brief 微小随机抖动强度系数，叠加细碎随机速度扰动，避免完美固定波形被自瞄算法拟合预判，
     *        例：0.05代表小幅细微抖动，不影响整体走位
     */
    float noise_ratio;

    /**
     * @brief 快速降速阶段占用整个变速周期的时间比例（正常范围0~0.5），数值越小降速越突然急促，反预判效果越强，
     *        例：0.12表示仅用12%的周期时间完成全部降速动作
     */
    float drop_ratio;

    /**
     * @brief 突降最低速度深度占本轮主波动振幅的比例，数值越大这次突降的降幅越深、快慢反差越明显，
     *        例：1.0f代表直接降到本轮波动允许的最低点，突降效果最强
     */
    float drop_depth_ratio;

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