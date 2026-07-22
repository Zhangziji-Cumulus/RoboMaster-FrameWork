#include "AutoAim.h"
#include "CRC.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == GIMBAL_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

#if(AUTOAIM_IFOPEN)

//static 
	AutoAim_Instance_t  AutoAim_Instance;//自瞄实例
//static 
	AutoAim_Ctrl_t AutoAim_Ctrl;//自瞄控制量

//** ================================================================================ **//
//** ================================== 融合算法 ==================================== **//
//** ================================================================================ **//

/**
 * @brief   整型(int16_t)加权融合函数 —— 手动控制量与自瞄控制量的加权融合
 * @param  manual    手动控制量（操作手给定值）
 * @param  auto_val  自瞄控制量（视觉算法输出值）
 * @param  aim_valid 自瞄有效性标志（0=无效/无目标，1=有效/有目标）
 * @param  min_out   输出下限（限幅）
 * @param  max_out   输出上限（限幅）
 * @return int16_t   融合后的控制量
 * @note   当自瞄无效时完全输出手动值；
 *         当自瞄有效时按权重比 AUTOAIM_WEIGHT_AUTO / AUTOAIM_WEIGHT_MANUAL 加权融合；
 *         最终结果做限幅保护，防止溢出。
 */
int16_t AutoAim_WeightFusion_Int16(int16_t manual, int16_t auto_val, uint8_t aim_valid, int16_t min_out, int16_t max_out)
{
    int32_t output;

    if (aim_valid == 0)
    {
        output = manual;
    }
    else
    {
        output =  ((int32_t)auto_val * AUTOAIM_WEIGHT_AUTO +
                   (int32_t)manual  * AUTOAIM_WEIGHT_MANUAL) / 100;
    }

    if (output > max_out) output = max_out;
    if (output < min_out) output = min_out;

    return (int16_t)output;
}

/**
 * @brief   浮点(float)加权融合函数 —— 手动控制量与自瞄控制量的加权融合
 * @param  manual    手动控制量（操作手给定值）
 * @param  auto_val  自瞄控制量（视觉算法输出值）
 * @param  aim_valid 自瞄有效性标志（0=无效/无目标，1=有效/有目标）
 * @param  min_out   输出下限（限幅）
 * @param  max_out   输出上限（限幅）
 * @return float     融合后的控制量
 * @note   与 Int16 版本逻辑一致，但使用浮点运算避免精度损失；
 *         先乘权重再除以100，等价于加权平均。
 */
float AutoAim_WeightFusion_Float(float manual, float auto_val, uint8_t aim_valid, float min_out, float max_out)
{
    float output;

    if (aim_valid == 0)
    {
        output = manual;
    }
    else
    {
        output =  auto_val * AUTOAIM_WEIGHT_AUTO +
                  manual  * AUTOAIM_WEIGHT_MANUAL;
        output /= 100.0f;
    }

    if (output > max_out)
        output = max_out;
    if (output < min_out)
        output = min_out;

    return output;
}

/**
 * @brief   获取自瞄控制量结构体指针
 * @return const AutoAim_Ctrl_t*  自瞄控制量（Yaw/Pitch/FireOK/IsOnline）的指针
 * @note    Yaw/Pitch 为视觉给出的期望角度，FireOK 为开火允许标志，
 *         IsOnline 表示视觉上位机是否在线。
 *         外部模块通过此接口只读访问自瞄控制量，无需直接操作本文件内部变量。
 */

//** ================================================================================ **//
//** ========================== 获取自瞄控制数据（对外） ============================= **//
//** ================================================================================ **//
const AutoAim_Ctrl_t* AutoAim_Ctrl_Get_point(void)
{
    return &AutoAim_Ctrl;
}

//** ------------------------------------------------------------ **//
//** ======================== 初始化通信 ======================== **//
//** ------------------------------------------------------------ **//
void AutoAim_Init(void)
{
    __HAL_UART_ENABLE_IT(&AUTO_USART_HANDLE, UART_IT_IDLE);

    AutoAim_Instance.Rx_ActiveBuf = 0;
    // DMA循环模式绑定第一个缓冲，硬件自动循环切换
    HAL_UART_Receive_DMA(&AUTO_USART_HANDLE, (uint8_t *)&AutoAim_Instance.Rx_Buf[AutoAim_Instance.Rx_ActiveBuf], sizeof(AutoAim_Rx_t));

    AutoAim_Instance.Tx_Done = 1;
    AutoAim_Instance.Tx.Frame_head = AUTO_USART_HEADER;
    AutoAim_Instance.Tx.Enemy_Color = AUTOAIM_ENEMY_COLOR;

    // 初始化自适应EMA滤波器
    memset(&AutoAim_Instance.EMA, 0, sizeof(AutoAim_EMA_t));

    // 初始化速度前馈追踪值
    AutoAim_Instance.Ctrl_Yaw_Prev   = 0.0f;
    AutoAim_Instance.Ctrl_Pitch_Prev = 0.0f;
    AutoAim_Instance.Ctrl_Tick_Prev  = HAL_GetTick();
    AutoAim_Ctrl.YawVel   = 0.0f;
    AutoAim_Ctrl.PitchVel = 0.0f;
}

//** ====================== 更新要发送的数据 ====================== **//
void AutoAim_UpdateTx(void)
{
    AutoAim_Instance.MCUData.INS_angle =  IMU_Get_point();
    AutoAim_Instance.Tx.IMU_Roll = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_ROLL];
    AutoAim_Instance.Tx.IMU_Pitch = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_PITCH];
    AutoAim_Instance.Tx.IMU_Yaw = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_YAW];

    //AutoAim_Instance.Tx.Match = 999;
}

//** ===================== 普通模式发送函数 ====================== **//
void AutoAim_SendData(void)
{
    HAL_UART_Transmit_DMA(&AUTO_USART_HANDLE, (uint8_t*)&AutoAim_Instance.Tx, sizeof(AutoAim_Tx_t));
}

//** ------------------------------------------------------------ **//
//** ======================== 接收解析函数 ======================= **//
//** ------------------------------------------------------------ **//

void AutoAim_ReceiveProcess(void)
{
    AutoAim_Rx_t *rx_buf = &AutoAim_Instance.Rx_ParseBuf;

    // 帧头校验
    if(rx_buf->Frame_head != AUTO_USART_HEADER)
    {
        AutoAim_Instance.Rx_OnlineFlag = 0;
        return;
    }

    // CRC16 校验：防止误匹配 / 数据损坏
    if(Verify_CRC16_Check_Sum((uint8_t *)rx_buf, sizeof(AutoAim_Rx_t)) == 0)
    {
        AutoAim_Instance.Rx_OnlineFlag = 0;
        return;
    }

    AutoAim_Instance.Rx = *rx_buf;
    AutoAim_Instance.Rx_OnlineFlag = 1;
    AutoAim_Instance.Rx_LastTick = HAL_GetTick();
}

//** ======================================================================== **//
//** ====================== 自适应EMA滤波器 ================================ **//
//** ======================================================================== **//

/**
 * @brief   自适应EMA滤波（单轴）—— 根据变化率动态调节平滑系数
 * @param  raw_val      当前原始值
 * @param  prev_raw    上一次原始值指针（会被更新为当前原始值）
 * @param  prev_filter  上一次滤波值指针（会被更新为当前滤波值）
 * @param  initialized  初始化标志指针（首次调用直接赋值，不滤波）
 * @return float        滤波后的值
 * @note   算法原理：
 *         1. 计算原始值的变化率 dx = |raw_val - *prev_raw|
 *         2. 自适应 alpha = ALPHA_MIN + (ALPHA_MAX - ALPHA_MIN) * saturate(dx / THRESHOLD)
 *         3. 标准一阶 EMA：filtered = alpha * raw + (1-alpha) * prev_filter
 *         4. 变化率大 → alpha 大 → 响应快（跟随目标快速移动）
 *         5. 变化率小 → alpha 小 → 平滑强（抑制抖动噪声）
 *         首次调用时直接输出原始值并初始化历史数据。
 */
static float AutoAim_EMA_Update_Single(float raw_val, float *prev_raw, float *prev_filter, uint8_t *initialized)
{
    float alpha;
    float dx;
    float output;

    if (!(*initialized))
    {
        // 首次：直接赋值，不滤波
        *prev_raw    = raw_val;
        *prev_filter = raw_val;
        *initialized = 1;
        return raw_val;
    }

    // 计算变化率
    dx = raw_val - *prev_raw;
    if (dx < 0.0f) dx = -dx;

    // 自适应 alpha：变化率越大 alpha 越大（响应越快）
    alpha = AUTOAIM_EMA_ALPHA_MIN;
    if (dx < AUTOAIM_EMA_THRESHOLD)
    {
        // 线性插值：alpha = alpha_min + (alpha_max - alpha_min) * (dx / threshold)
        alpha += (AUTOAIM_EMA_ALPHA_MAX - AUTOAIM_EMA_ALPHA_MIN) * (dx / AUTOAIM_EMA_THRESHOLD);
    }
    else
    {
        // 超过阈值，使用最大 alpha（最快响应）
        alpha = AUTOAIM_EMA_ALPHA_MAX;
    }

    // 一阶 EMA
    output = alpha * raw_val + (1.0f - alpha) * (*prev_filter);

    // 更新历史数据
    *prev_raw    = raw_val;
    *prev_filter = output;

    return output;
}

/**
 * @brief   自适应EMA滤波 —— 对 Rx 原始 Yaw/Pitch 进行滤波
 * @note    在 AutoAim_UpdateRx() 内部调用。
 *          当离线时重置滤波器 Initialized 标志，确保重连后快速跟踪。
 */
static void AutoAim_EMA_Update(void)
{
    AutoAim_EMA_t *ema = &AutoAim_Instance.EMA;

    // 离线时重置滤波器（重连后首次直接输出，不滤波）
    if (!AutoAim_Instance.Rx_OnlineFlag)
    {
        ema->Initialized = 0;
        return;
    }

    // 对 Yaw 和 Pitch 分别进行自适应 EMA 滤波（在原始域，未乘 0.1）
    ema->Yaw   = AutoAim_EMA_Update_Single(AutoAim_Instance.Rx.Yaw,
                                            &ema->Yaw_PrevRaw,
                                            &ema->Yaw,
                                            &ema->Initialized);
    ema->Pitch = AutoAim_EMA_Update_Single(AutoAim_Instance.Rx.Pitch,
                                            &ema->Pitch_PrevRaw,
                                            &ema->Pitch,
                                            &ema->Initialized);
}

//** ------------------------------------------------------------ **//
//** ======================== 更新接收数据 ======================== **//
//** ------------------------------------------------------------ **//
void AutoAim_UpdateRx(void)
{
    // 超时检测：超过 AUTOAIM_RX_TIMEOUT_MS 未收到有效帧 → 置离线
    if (HAL_GetTick() - AutoAim_Instance.Rx_LastTick > AUTOAIM_RX_TIMEOUT_MS)
    {
        AutoAim_Instance.Rx_OnlineFlag = 0;
    }
    
    // 先执行自适应EMA滤波
    AutoAim_EMA_Update();
    
    if(AutoAim_Instance.Rx_OnlineFlag)
    {
        // 注意：此处使用滤波后的值（ema->Yaw / ema->Pitch）而非原始 Rx.Yaw / Rx.Pitch
        AutoAim_Ctrl.Yaw   = (AutoAim_Instance.EMA.Yaw   * 0.1f);
        AutoAim_Ctrl.Pitch = (AutoAim_Instance.EMA.Pitch * 0.1f);
        AutoAim_Ctrl.FireOK = AutoAim_Instance.Rx.Fire;
        AutoAim_Ctrl.IsOnline = 1;
        AutoAim_Ctrl.RxTick = AutoAim_Instance.Rx_LastTick;

        // === 计算目标角速度（用于速度前馈） ===
        uint32_t now_tick = HAL_GetTick();
        uint32_t dt_ms = now_tick - AutoAim_Instance.Ctrl_Tick_Prev;

        if (dt_ms >= 1)  // 防止除零
        {
            float dt_sec = (float)dt_ms * 0.001f;

            // 原始角速度 (°/s)
            float raw_vel_yaw   = (AutoAim_Ctrl.Yaw   - AutoAim_Instance.Ctrl_Yaw_Prev)   / dt_sec;
            float raw_vel_pitch = (AutoAim_Ctrl.Pitch - AutoAim_Instance.Ctrl_Pitch_Prev) / dt_sec;

            // 一阶低通滤波，抑制速度噪声
            AutoAim_Ctrl.YawVel   = AUTOAIM_FF_LPF_ALPHA * raw_vel_yaw
                                  + (1.0f - AUTOAIM_FF_LPF_ALPHA) * AutoAim_Ctrl.YawVel;
            AutoAim_Ctrl.PitchVel = AUTOAIM_FF_LPF_ALPHA * raw_vel_pitch
                                  + (1.0f - AUTOAIM_FF_LPF_ALPHA) * AutoAim_Ctrl.PitchVel;

            // 速度死区：微小速度不给前馈，防止静止时漂移
            if (fabsf(AutoAim_Ctrl.YawVel) < AUTOAIM_FF_DEADZONE)
                AutoAim_Ctrl.YawVel = 0.0f;
            if (fabsf(AutoAim_Ctrl.PitchVel) < AUTOAIM_FF_DEADZONE)
                AutoAim_Ctrl.PitchVel = 0.0f;
        }

        // 更新前馈追踪值
        AutoAim_Instance.Ctrl_Yaw_Prev   = AutoAim_Ctrl.Yaw;
        AutoAim_Instance.Ctrl_Pitch_Prev = AutoAim_Ctrl.Pitch;
        AutoAim_Instance.Ctrl_Tick_Prev  = now_tick;
    }
    else
    {
        AutoAim_Ctrl.Yaw = 0.0f;
        AutoAim_Ctrl.Pitch = 0.0f;
        AutoAim_Ctrl.FireOK = 0;
        AutoAim_Ctrl.IsOnline = 0;
        AutoAim_Ctrl.YawVel = 0.0f;
        AutoAim_Ctrl.PitchVel = 0.0f;
    }
}

/**
 * @brief   帧头丢失重同步：在接收缓冲区内逐字节搜索帧头
 * @note    当 DMA 收到的数据帧头不匹配（由于串口噪声或字节错位导致），
 *          遍历整个 Rx_ParseBuf 寻找 AUTO_USART_HEADER。
 *          找到后将后续数据整体前移，使帧头对齐到 buffer[0]。
 *          如果从头到尾都找不到帧头，则保持原有数据不变，
 *          后续的 ReceiveProcess 会因帧头错误而丢弃。
 */
static void AutoAim_ByteSearchResync(void)
{
    uint8_t *p = (uint8_t *)&AutoAim_Instance.Rx_ParseBuf;
    uint32_t i;

    for (i = 1; i < sizeof(AutoAim_Rx_t); i++)
    {
        if (p[i] == AUTO_USART_HEADER)
        {
            // 从偏移 i 处开始，将剩余数据前移对齐
            memmove(p, &p[i], sizeof(AutoAim_Rx_t) - i);
            // 末尾清零（清除残留数据）
            memset(&p[sizeof(AutoAim_Rx_t) - i], 0, i);
            break;
        }
    }
    // 未找到帧头 → 不操作，后续 ReceiveProcess 会检测到帧头错误
}

//** ------------------------------------------------------------ **//
//** ======================== 中断接收函数 ======================= **//
//** ------------------------------------------------------------ **//
void AutoAim_UART_IRQHandler(void)
{
    HAL_UART_IRQHandler(&AUTO_USART_HANDLE);

    if (__HAL_UART_GET_FLAG(&AUTO_USART_HANDLE, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&AUTO_USART_HANDLE);

        // 1. 把当前DMA写完的整帧拷贝到解析缓存（DMA此时在另一个缓冲继续存数据，不会被覆写）
        memcpy(&AutoAim_Instance.Rx_ParseBuf, &AutoAim_Instance.Rx_Buf[AutoAim_Instance.Rx_ActiveBuf], sizeof(AutoAim_Rx_t));

        // 2. 切换DMA目标缓冲，硬件自动循环切换
        AutoAim_Instance.Rx_ActiveBuf ^= 1U;
        HAL_UART_Receive_DMA(&AUTO_USART_HANDLE, (uint8_t *)&AutoAim_Instance.Rx_Buf[AutoAim_Instance.Rx_ActiveBuf], sizeof(AutoAim_Rx_t));

        // 3. 若帧头不匹配，尝试逐字节搜索重同步（解决失步后无法恢复的问题）
        if (AutoAim_Instance.Rx_ParseBuf.Frame_head != AUTO_USART_HEADER)
        {
            AutoAim_ByteSearchResync();
        }

        // 4. 解析数据（含帧头 + CRC 双重校验）
        AutoAim_ReceiveProcess();
    }
}

#endif

#endif
