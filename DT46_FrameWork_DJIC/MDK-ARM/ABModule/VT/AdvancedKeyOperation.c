#include "AdvancedKeyOperation.h"

void KeyHandle_Init(KeyHandle_t *key, uint16_t tick, uint8_t en_debounce)
{
    if (key == NULL)
        return;

    key->stable_level   = 0;
    key->debounce_cnt   = 0;
    key->debounce_tick  = tick;
    key->enable_debounce = en_debounce;

    key->last_stable_lv = 0;
    key->toggle_state   = 0;

    // 长按计时区初始化
    key->press_start_tick = 0;
    key->is_long_triggered = 0;
}

// 消抖函数：结构体指针 + 外部电平源
uint8_t KeyDebounce_Process(KeyHandle_t *key, uint8_t raw_lv)
{
    if (key == NULL)
        return 0;

    if (raw_lv != key->stable_level)
    {
        key->debounce_cnt++;
        if (key->debounce_cnt >= key->debounce_tick)
        {
            key->stable_level = raw_lv;
            key->debounce_cnt = 0;
        }
    }
    else
    {
        key->debounce_cnt = 0;
    }
    return key->stable_level;
}

// 翻转函数，接收外部电平，内部按需调用消抖
uint8_t KeyToggle_Process(KeyHandle_t *key, uint8_t raw_lv)
{
    if (key == NULL)
        return 0;

    uint8_t curr_lv;
    if (key->enable_debounce)
    {
        // 调用独立消抖函数，传入外部原始电平
        curr_lv = KeyDebounce_Process(key, raw_lv);
    }
    else
    {
        // 关闭消抖，直接使用外部电平
        curr_lv = raw_lv;
    }

    // 上升沿触发翻转
    if (curr_lv == 1 && key->last_stable_lv == 0)
    {
        key->toggle_state = !key->toggle_state;
    }
    key->last_stable_lv = curr_lv;

    return key->toggle_state;
}

KeyLongState_t KeyLongPress_Process(KeyHandle_t *key, uint8_t raw_lv, uint8_t active_lv, uint32_t long_ms)
{
    if (key == NULL)
        return KEY_LONG_IDLE;

    uint8_t stable_lv = raw_lv;
    if (key->enable_debounce)
    {
        // 双向消抖：不管是按下跳变，还是长按过程中跳变松开，都持续计数
        if (raw_lv != key->stable_level)
        {
            key->debounce_cnt++;
            // 计数达标，更新稳定电平，重置计数器
            if (key->debounce_cnt >= key->debounce_tick)
            {
                key->stable_level = raw_lv;
                key->debounce_cnt = 0;
            }
        }
        else
        {
            // 电平持续一致，计数器清零
            key->debounce_cnt = 0;
        }
        stable_lv = key->stable_level;
    }

    uint32_t now_tick = HAL_GetTick();
    KeyLongState_t ret = KEY_LONG_IDLE;

    if (stable_lv == active_lv)
    {
        // 稳定按下状态
        if (key->press_start_tick == 0)
        {
            key->press_start_tick = now_tick;
            key->is_long_triggered = 0;
        }

        uint32_t hold_time = now_tick - key->press_start_tick;
        if (hold_time >= long_ms)
        {
            if (!key->is_long_triggered)
            {
                ret = KEY_LONG_PRESS_READY;
                key->is_long_triggered = 1;
            }
            else
            {
                ret = KEY_LONG_PRESS_HOLD;
            }
        }
    }
    else
    {
        // 稳定松开状态
        if (key->press_start_tick != 0)
        {
            uint32_t hold_time = now_tick - key->press_start_tick;
            if (hold_time < long_ms)
            {
                ret = KEY_SHORT_PRESS;
            }
        }
        // 松开清空长按计时
        key->press_start_tick = 0;
        key->is_long_triggered = 0;
    }
    return ret;
}
