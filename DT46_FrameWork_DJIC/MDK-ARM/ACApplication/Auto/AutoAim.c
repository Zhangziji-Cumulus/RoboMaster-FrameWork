#include "AutoAim.h"
#include "CRC.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == GIMBAL_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

#if(AUTOAIM_IFOPEN)

//static 
	AutoAim_Instance_t  AutoAim_Instance;//自瞄实例
//static 
	AutoAim_Ctrl_t AutoAim_Ctrl;//自瞄控制量

//** ================================================================================ **//
//** ================== 运行时可调参数（Debug时Watch窗口实时修改） =================== **//
//** ================================================================================ **//
AutoAim_Param_t autoaim_param = {
    .Gain            = 1.0f,
    .PID_FF_Gain_Yaw   = 50.0f,
    .PID_FF_Gain_Pitch = 8.0f,
    .FF_Decay_K      = 5.0f,
    .FF_Max_Yaw      = 800,
    .FF_Max_Pitch    = 300,
    .TargetFF_Gain   = 0.5f,
    .FF_LPF_Alpha    = 0.5f,
    .FF_DeadZone     = 1.0f,
    .EMA_Alpha_Min   = 0.40f,
    .EMA_Alpha_Max   = 0.95f,
    .EMA_Threshold   = 1.0f,
    .Max_Jump_Deg    = 5.0f,
};

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

//** ================================================================================ **//
//** ======================= 单轴自瞄+手动融合（含速度前馈） ========================= **//
//** ================================================================================ **//

/**
 * @brief   单轴自瞄+手动融合函数（含速度前馈）
 * @param  manual     手动控制量（角度步进值）
 * @param  auto_val   自瞄期望角度修正值
 * @param  auto_vel   自瞄目标角速度(°/s)
 * @param  aim_online 自瞄是否在线
 * @param  dt_sec     控制周期(秒)，用于前馈换算
 * @return float      融合后的输出值
 * @note   当自瞄离线时只输出手动量；
 *         当自瞄在线时叠加修正 Gain × auto_val 和速度前馈 auto_vel × TargetFF_Gain × dt_sec。
 *         参数来自 autoaim_param 结构体，可在 Debug 时通过 Watch 窗口实时调参。
 */
float AutoAim_FusionAxis(float manual, float auto_val, float auto_vel, uint8_t aim_online, float dt_sec)
{
    if (!aim_online)
        return manual;

    float ff = auto_vel * autoaim_param.TargetFF_Gain * dt_sec;
    return manual + autoaim_param.Gain * auto_val + ff;
}

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

    // 初始化帧率自适应 & 异常检测
    AutoAim_Instance.MeasuredFPS   = 30.0f;   // 默认30FPS
    AutoAim_Instance.LastRxTick    = 0;
    AutoAim_Instance.LastRawYaw    = 0.0f;
    AutoAim_Instance.LastRawPitch  = 0.0f;
}

//** ====================== 更新要发送的数据 ====================== **//
void AutoAim_UpdateTx(void)
{
    AutoAim_Instance.MCUData.INS_angle =  IMU_Get_point();
    AutoAim_Instance.Tx.IMU_Roll = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_ROLL];
    AutoAim_Instance.Tx.IMU_Pitch = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_PITCH];
    AutoAim_Instance.Tx.IMU_Yaw = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_YAW];
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
 * @brief   自适应EMA滤波（单轴）—— 增加 alpha_min 参数，支持帧率自适应
 * @param  raw_val      当前原始值
 * @param  prev_raw    上一次原始值指针（会被更新为当前原始值）
 * @param  prev_filter  上一次滤波值指针（会被更新为当前滤波值）
 * @param  initialized  初始化标志指针（首次调用直接赋值，不滤波）
 * @param  alpha_min    最小alpha（由帧率自适应计算传入，替代固定宏）
 * @return float        滤波后的值
 * @note   使用 autoaim_param 结构体中的参数，可在 Debug 时实时调参。
 */
static float AutoAim_EMA_Update_Single_Adaptive(float raw_val, float *prev_raw,
    float *prev_filter, uint8_t *initialized, float alpha_min)
{
    float alpha;
    float dx;
    float output;

    if (!(*initialized))
    {
        *prev_raw    = raw_val;
        *prev_filter = raw_val;
        *initialized = 1;
        return raw_val;
    }

    dx = raw_val - *prev_raw;
    if (dx < 0.0f) dx = -dx;

    // 自适应 alpha：用传入的 alpha_min（帧率自适应）替代固定宏
    alpha = alpha_min;
    if (dx < autoaim_param.EMA_Threshold)
    {
        alpha += (autoaim_param.EMA_Alpha_Max - alpha_min) * (dx / autoaim_param.EMA_Threshold);
    }
    else
    {
        alpha = autoaim_param.EMA_Alpha_Max;
    }

    output = alpha * raw_val + (1.0f - alpha) * (*prev_filter);

    *prev_raw    = raw_val;
    *prev_filter = output;

    return output;
}

/**
 * @brief   计算帧率自适应的 EMA α_min
 * @param  inst  AutoAim_Instance_t 指针（含 MeasuredFPS）
 * @return float 自适应后的 α_min
 * @note   α_min = α_base × (FPS_base / FPS_measured)
 *         30FPS → 0.25 × 2.0 = 0.50（延迟小，响应快）
 *         60FPS → 0.25 × 1.0 = 0.25（平滑适中）
 *         120FPS→ 0.25 × 0.5 = 0.125（充分利用高帧率，更平滑）
 */
static float AutoAim_CalcAdaptiveAlpha(AutoAim_Instance_t *inst)
{
    float alpha_min;

    alpha_min = AUTOAIM_EMA_ALPHA_BASE * (AUTOAIM_FPS_BASE / inst->MeasuredFPS);

    if (alpha_min < AUTOAIM_EMA_ALPHA_ABSOLUTE_MIN)
        alpha_min = AUTOAIM_EMA_ALPHA_ABSOLUTE_MIN;
    if (alpha_min > AUTOAIM_EMA_ALPHA_ABSOLUTE_MAX)
        alpha_min = AUTOAIM_EMA_ALPHA_ABSOLUTE_MAX;

    return alpha_min;
}

/**
 * @brief   自适应EMA滤波 —— 重写版
 * @note    新增功能：
 *          1. 数据新鲜度检测：RxTick没变则跳过滤波，维持上次输出
 *          2. 帧率实时测量：只在数据更新时计算帧率
 *          3. 异常跳变检测：|Δ| > Max_Jump_Deg 丢弃该帧
 *          4. 帧率自适应 α_min：根据实测帧率自动调整
 *          5. 离线时重置滤波器
 */
static void AutoAim_EMA_Update(void)
{
    AutoAim_EMA_t *ema = &AutoAim_Instance.EMA;

    // 离线时重置滤波器（重连后首次直接输出，不滤波）
    if (!AutoAim_Instance.Rx_OnlineFlag)
    {
        ema->Initialized = 0;
        AutoAim_Instance.LastRawYaw   = 0.0f;
        AutoAim_Instance.LastRawPitch = 0.0f;
        return;
    }

    // === 数据新鲜度检测 ===
    // 如果 RxTick 没变（同一帧重复接收），跳过滤波，维持上次输出
    if (AutoAim_Instance.Rx_LastTick == AutoAim_Instance.LastRxTick)
        return;
    AutoAim_Instance.LastRxTick = AutoAim_Instance.Rx_LastTick;

    // === 帧率测量（只在数据更新时测量） ===
    static uint32_t prev_fps_tick = 0;
    if (prev_fps_tick != 0)
    {
        float dt_ms = (float)(AutoAim_Instance.Rx_LastTick - prev_fps_tick);
        if (dt_ms >= 1.0f && dt_ms <= 500.0f)
        {
            float instant_fps = 1000.0f / dt_ms;
            AutoAim_Instance.MeasuredFPS = AUTOAIM_FPS_LPF_ALPHA * instant_fps
                                         + (1.0f - AUTOAIM_FPS_LPF_ALPHA) * AutoAim_Instance.MeasuredFPS;
        }
    }
    prev_fps_tick = AutoAim_Instance.Rx_LastTick;

    // === 异常跳变检测 ===
    float dyaw   = fabsf(AutoAim_Instance.Rx.Yaw   - AutoAim_Instance.LastRawYaw);
    float dpitch = fabsf(AutoAim_Instance.Rx.Pitch - AutoAim_Instance.LastRawPitch);

    if (dyaw > autoaim_param.Max_Jump_Deg || dpitch > autoaim_param.Max_Jump_Deg)
    {
        // 异常跳变 → 丢弃这一帧，保持上次输出
        return;
    }
    AutoAim_Instance.LastRawYaw   = AutoAim_Instance.Rx.Yaw;
    AutoAim_Instance.LastRawPitch = AutoAim_Instance.Rx.Pitch;

    // === 计算帧率自适应的 α_min ===
    float adaptive_alpha_min = AutoAim_CalcAdaptiveAlpha(&AutoAim_Instance);

    // === 对 Yaw 和 Pitch 分别滤波（使用自适应 α_min） ===
    ema->Yaw   = AutoAim_EMA_Update_Single_Adaptive(AutoAim_Instance.Rx.Yaw,
                    &ema->Yaw_PrevRaw, &ema->Yaw, &ema->Initialized,
                    adaptive_alpha_min);
    ema->Pitch = AutoAim_EMA_Update_Single_Adaptive(AutoAim_Instance.Rx.Pitch,
                    &ema->Pitch_PrevRaw, &ema->Pitch, &ema->Initialized,
                    adaptive_alpha_min);
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

            // 一阶低通滤波，抑制速度噪声（参数来自 autoaim_param，Debug可实时调）
            AutoAim_Ctrl.YawVel   = autoaim_param.FF_LPF_Alpha * raw_vel_yaw
                                  + (1.0f - autoaim_param.FF_LPF_Alpha) * AutoAim_Ctrl.YawVel;
            AutoAim_Ctrl.PitchVel = autoaim_param.FF_LPF_Alpha * raw_vel_pitch
                                  + (1.0f - autoaim_param.FF_LPF_Alpha) * AutoAim_Ctrl.PitchVel;

            // 速度死区：微小速度不给前馈，防止静止时漂移
            if (fabsf(AutoAim_Ctrl.YawVel) < autoaim_param.FF_DeadZone)
                AutoAim_Ctrl.YawVel = 0.0f;
            if (fabsf(AutoAim_Ctrl.PitchVel) < autoaim_param.FF_DeadZone)
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
