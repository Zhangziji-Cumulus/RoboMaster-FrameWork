#ifndef ADVANCEDKEYOPERATION_H_
#define ADVANCEDKEYOPERATION_H_

#include <stdint.h>
#include <stddef.h>

// 长按状态枚举
typedef enum
{
    KEY_LONG_IDLE        = 0,  // 空闲松开
    KEY_SHORT_PRESS,           // 松开后判定短按
    KEY_LONG_PRESS_READY,      // 刚到达长按阈值，触发一次
    KEY_LONG_PRESS_HOLD        // 持续长按保持
} KeyLongState_t;

// 整合结构体：消抖 + 翻转自锁 + 长按计时全部一体
typedef struct
{
    // 消抖模块
    uint8_t stable_level;
    uint16_t debounce_cnt;
    uint16_t debounce_tick;    // 单按键独立消抖扫描次数阈值
    uint8_t enable_debounce;   // 1启用消抖 0关闭消抖

    // 翻转自锁模块
    uint8_t last_stable_lv;
    uint8_t toggle_state;

    // 长按计时模块（新增，统一收纳）
    uint32_t press_start_tick; // 稳定按下起始HAL tick
    uint8_t  is_long_triggered;// 是否已经触发过长按就绪标志

} KeyHandle_t;

/**
 * @brief 按键初始化
 * @param key 按键结构体指针
 * @param tick 消抖阈值
 * @param en_debounce 1开启消抖 0关闭消抖
 */
void KeyHandle_Init(KeyHandle_t *key, uint16_t tick, uint8_t en_debounce);

/**
 * @brief 独立消抖处理函数
 * @param key 按键状态结构体指针
 * @param raw_lv 外部传入本次原始电平0/1
 * @return 消抖稳定电平
 */
uint8_t KeyDebounce_Process(KeyHandle_t *key, uint8_t raw_lv);

/**
 * @brief 一体化翻转处理
 * @param key 按键结构体指针
 * @param raw_lv 外部原始电平
 * @return 自锁开关状态0/1
 */
uint8_t KeyToggle_Process(KeyHandle_t *key, uint8_t raw_lv);

/**
 * @brief 长按检测处理函数
 * @param key 按键句柄（集成消抖+长按计时）
 * @param raw_lv 当前IO原始电平
 * @param active_lv 有效触发电平（0低电平按下/1高电平按下）
 * @param long_ms 长按判定阈值 单位ms
 * @return KeyLongState_t 按键长短按状态
 */
KeyLongState_t KeyLongPress_Process(KeyHandle_t *key, uint8_t raw_lv, uint8_t active_lv, uint32_t long_ms);

#endif // ADVANCEDKEYOPERATION_H_