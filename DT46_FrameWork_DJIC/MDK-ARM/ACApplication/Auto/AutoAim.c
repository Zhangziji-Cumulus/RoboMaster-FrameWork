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
    // Phase 1: 位置跟踪
    .Scale_Yaw       = 0.040f,
    .Scale_Pitch     = 0.0f,
    .Gain            = 1.0f,
    .Alpha_Still     = 0.01f,
    .DeadBand        = 1.5f,
    .Pos_DeadBand    = 0.005f,
    .Max_Jump_Deg    = 8.0f,

    // Phase 2: 匀速跟踪
    .Alpha_Ramp      = 0.40f,
    .Kff_Ramp        = 0.0f,
    .Delta_Thr_Ramp  = 1.5f,

    // Phase 3: 阶跃响应
    .Alpha_Step      = 0.90f,
    .Kff_Step        = 0.0f,
    .Delta_Thr_Step  = 3.0f,

    // Phase 4: 拐点平滑
    .Alpha_Corner    = 0.25f,
    .Kff_Corner      = 0.0f,
    .Corner_Hold_Time = 200.0f,

    // PID层速度前馈
    .PID_FF_Gain_Yaw   = 0.0f,
    .PID_FF_Gain_Pitch = 0.0f,
    .FF_Decay_K      = 5.0f,
    .FF_Max_Yaw      = 800,
    .FF_Max_Pitch    = 300,

    // 目标层速度前馈
    .TargetFF_Gain   = 0.0f,
    .FF_LPF_Alpha    = 0.5f,
    .FF_DeadZone     = 1.0f,
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

    // 初始化自适应跟随器
    memset(&AutoAim_Instance.Follower_Yaw, 0, sizeof(AutoAim_Follower_t));
    memset(&AutoAim_Instance.Follower_Pitch, 0, sizeof(AutoAim_Follower_t));
    AutoAim_Instance.Ctrl_Yaw_Last   = 0.0f;
    AutoAim_Instance.Ctrl_Pitch_Last = 0.0f;
    AutoAim_Ctrl.YawVel   = 0.0f;
    AutoAim_Ctrl.PitchVel = 0.0f;

    // 初始化帧率统计 & 异常检测
    AutoAim_Instance.MeasuredFPS   = 30.0f;
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
//** ====================== 自适应跟随滤波器 ================================ **//
//** ======================================================================== **//

/**
 * @brief   自适应跟随器核心函数（单轴）
 * @param  f         跟随器状态指针
 * @param  raw_val   当前原始指令值（视觉角度）
 * @param  timestamp 当前时间戳(ms)
 * @note   根据指令变化量自动判别工况，选择滤波系数和前馈策略：
 *         - 阶跃(STEP):  |Δcmd| > Delta_Thr_Step，弱滤波+误差前馈
 *         - 拐点(CORNER): 方向反转，中等滤波+衰减前馈
 *         - 匀速(RAMP):  有规律运动，中等滤波+速度前馈
 *         - 静止(STILL): 微小抖动，强滤波+零前馈
 */
static void AutoAim_Follower_Update(AutoAim_Follower_t *f, float raw_val, uint32_t timestamp)
{
    float cmd_delta_raw = raw_val - f->cmd_prev;
    float cmd_delta     = cmd_delta_raw;

    // 计算帧间隔 (秒)，用于将 °/帧 转换为 °/秒
    float dt_sec = 0.0f;
    if (f->initialized && timestamp > f->tick_prev)
    {
        dt_sec = (float)(timestamp - f->tick_prev) * 0.001f;
        if (dt_sec < 0.001f) dt_sec = 0.001f;   // 防除零
        if (dt_sec > 1.0f)   dt_sec = 1.0f;     // 上限保护
    }

    // 死区处理：比较当前值与滤波输出值，而非上一帧原始值
    // 这样即使噪声交替 ±0.5°，只要不偏离滤波值超过 DeadBand，就不会穿透死区
    uint8_t in_deadband = (fabsf(raw_val - f->filtered) < autoaim_param.DeadBand);
    if (in_deadband)
        cmd_delta = 0.0f;

    // 方向反转检测
    if (f->initialized &&
        cmd_delta * f->cmd_delta_prev < 0.0f &&
        !in_deadband)
    {
        f->direction_reversed = 1;
        f->last_reversal_time = (float)timestamp;
    }

    // 拐点保持超时退出
    if (f->direction_reversed &&
        ((float)timestamp - f->last_reversal_time) > autoaim_param.Corner_Hold_Time)
    {
        f->direction_reversed = 0;
    }

    // === 工况判别 + 参数选择 ===
    float alpha;
    float ff = 0.0f;
    float abs_delta = fabsf(cmd_delta);    // 帧间变化量（用于阶跃检测）
    float gap       = fabsf(raw_val - f->filtered);  // 与滤波值的偏差（用于匀速检测）

    if (!f->initialized)
    {
        // 首次初始化：直接赋值，不滤波
        f->filtered       = raw_val;
        f->cmd_prev       = raw_val;
        f->cmd_delta_prev = 0.0f;
        f->feedforward    = 0.0f;
        f->tick_prev      = timestamp;
        f->initialized    = 1;
        f->mode           = FOLLOWER_MODE_STILL;
        return;
    }
    else if (abs_delta > autoaim_param.Delta_Thr_Step)
    {
        alpha = autoaim_param.Alpha_Step;
        ff    = gap * autoaim_param.Kff_Step;
        f->mode = FOLLOWER_MODE_STEP;
    }
    else if (f->direction_reversed)
    {
        alpha = autoaim_param.Alpha_Corner;
        ff    = (dt_sec > 0.001f) ? (f->feedforward * autoaim_param.Kff_Corner) : 0.0f;
        f->mode = FOLLOWER_MODE_CORNER;
    }
    else if (!in_deadband && gap > autoaim_param.Delta_Thr_Ramp)
    {
        alpha = autoaim_param.Alpha_Ramp;
        ff    = (dt_sec > 0.001f) ? ((cmd_delta / dt_sec) * autoaim_param.Kff_Ramp) : 0.0f;
        f->mode = FOLLOWER_MODE_RAMP;
    }
    else
    {
        alpha = autoaim_param.Alpha_Still;
        ff    = 0.0f;
        f->mode = FOLLOWER_MODE_STILL;
    }

    // 一阶低通滤波
    if (!in_deadband)
    {
        // 正常模式：用工况对应的 α 滤波
        f->filtered = alpha * raw_val + (1.0f - alpha) * f->filtered;
    }
    else
    {
        // 死区内：极弱跟踪 (α=0.02)，几乎冻结但缓慢跟随，退出死区时无跳变
        f->filtered = 0.02f * raw_val + 0.98f * f->filtered;
    }

    // 前馈输出 (°/s)
    f->feedforward = ff;

    // 更新历史状态
    f->cmd_prev       = raw_val;
    f->cmd_delta_prev = cmd_delta;
    f->tick_prev      = timestamp;
}

/**
 * @brief   双轴自适应跟随更新（Yaw + Pitch）
 * @param  timestamp 当前时间戳(ms)
 * @note   包含离线重置、数据新鲜度检测、跳变检测
 */
static void AutoAim_Follower_UpdateAll(uint32_t timestamp)
{
    // === 离线时重置 ===
    if (!AutoAim_Instance.Rx_OnlineFlag)
    {
        memset(&AutoAim_Instance.Follower_Yaw, 0, sizeof(AutoAim_Follower_t));
        memset(&AutoAim_Instance.Follower_Pitch, 0, sizeof(AutoAim_Follower_t));
        AutoAim_Instance.Ctrl_Yaw_Last   = 0.0f;
        AutoAim_Instance.Ctrl_Pitch_Last = 0.0f;
        AutoAim_Instance.LastRawYaw   = 0.0f;
        AutoAim_Instance.LastRawPitch = 0.0f;
        AutoAim_Ctrl.YawVel   = 0.0f;
        AutoAim_Ctrl.PitchVel = 0.0f;
        return;
    }

    // === 数据新鲜度检测 ===
    if (AutoAim_Instance.Rx_LastTick == AutoAim_Instance.LastRxTick)
        return;
    AutoAim_Instance.LastRxTick = AutoAim_Instance.Rx_LastTick;

    // === 帧率测量（仅统计） ===
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

    // === 异常跳变检测（Yaw/Pitch各自独立） ===
    float dyaw   = fabsf(AutoAim_Instance.Rx.Yaw   - AutoAim_Instance.LastRawYaw);
    float dpitch = fabsf(AutoAim_Instance.Rx.Pitch - AutoAim_Instance.LastRawPitch);

    uint8_t yaw_jump   = (dyaw   > autoaim_param.Max_Jump_Deg);
    uint8_t pitch_jump = (dpitch > autoaim_param.Max_Jump_Deg);

    // 未初始化的跟随器跳过跳变检测（目标首次出现在视场时允许大角度进入）
    if (!AutoAim_Instance.Follower_Yaw.initialized)
        yaw_jump = 0;
    if (!AutoAim_Instance.Follower_Pitch.initialized)
        pitch_jump = 0;

    if (!yaw_jump)
        AutoAim_Instance.LastRawYaw = AutoAim_Instance.Rx.Yaw;
    if (!pitch_jump)
        AutoAim_Instance.LastRawPitch = AutoAim_Instance.Rx.Pitch;

    // === 分别对 Yaw 和 Pitch 执行自适应跟随 ===
    if (!yaw_jump)
    {
        AutoAim_Follower_Update(&AutoAim_Instance.Follower_Yaw,
            AutoAim_Instance.Rx.Yaw, timestamp);
    }
    if (!pitch_jump)
    {
        AutoAim_Follower_Update(&AutoAim_Instance.Follower_Pitch,
            AutoAim_Instance.Rx.Pitch, timestamp);
    }
}

//** ------------------------------------------------------------ **//
//** ======================== 更新接收数据 ======================== **//
//** ------------------------------------------------------------ **//
void AutoAim_UpdateRx(void)
{
    uint32_t now_tick = HAL_GetTick();

    // 超时检测：超过 AUTOAIM_RX_TIMEOUT_MS 未收到有效帧 → 置离线
    if (now_tick - AutoAim_Instance.Rx_LastTick > AUTOAIM_RX_TIMEOUT_MS)
    {
        AutoAim_Instance.Rx_OnlineFlag = 0;
    }
    
    // 执行自适应跟随滤波（含跳变检测、工况判别、滤波+前馈输出）
    AutoAim_Follower_UpdateAll(now_tick);
    
    if(AutoAim_Instance.Rx_OnlineFlag)
    {
        // 使用跟随器输出的滤波后值 × Scale 作为角度修正量
        float new_yaw   = AutoAim_Instance.Follower_Yaw.filtered   * autoaim_param.Scale_Yaw;
        float new_pitch = AutoAim_Instance.Follower_Pitch.filtered * autoaim_param.Scale_Pitch;

        // 位置输出死区：变化太小时保持上次值，防止静止时微抖
        if (autoaim_param.Pos_DeadBand > 0.0f)
        {
            if (fabsf(new_yaw - AutoAim_Instance.Ctrl_Yaw_Last) > autoaim_param.Pos_DeadBand)
                AutoAim_Instance.Ctrl_Yaw_Last = new_yaw;
            if (fabsf(new_pitch - AutoAim_Instance.Ctrl_Pitch_Last) > autoaim_param.Pos_DeadBand)
                AutoAim_Instance.Ctrl_Pitch_Last = new_pitch;
            AutoAim_Ctrl.Yaw   = AutoAim_Instance.Ctrl_Yaw_Last;
            AutoAim_Ctrl.Pitch = AutoAim_Instance.Ctrl_Pitch_Last;
        }
        else
        {
            AutoAim_Ctrl.Yaw   = new_yaw;
            AutoAim_Ctrl.Pitch = new_pitch;
        }

        AutoAim_Ctrl.FireOK = AutoAim_Instance.Rx.Fire;
        AutoAim_Ctrl.IsOnline = 1;
        AutoAim_Ctrl.RxTick = AutoAim_Instance.Rx_LastTick;

        // 使用跟随器直接输出的前馈速度（跟随器内部已根据工况判别，无需额外滤波）
        AutoAim_Ctrl.YawVel   = AutoAim_Instance.Follower_Yaw.feedforward;
        AutoAim_Ctrl.PitchVel = AutoAim_Instance.Follower_Pitch.feedforward;
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
