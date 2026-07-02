#include "VTCenter.h"

#if(0)
#if(BOARD_ID == GIMBAL_BOARD)

/* 私有变量 */
static UART_HandleTypeDef *vt03Huart;
static uint8_t vt03RxCyclicBuf[VT03_RX_BUF_SIZE];  // DMA循环接收缓存
static uint8_t crc_fail_cnt;    // CRC连续失败计数器

static VideoTx_Ctrl_t vt03Data;
static VT03_State_t vt03State;

/* 状态计时标志 */
static uint8_t newDataFlag;
static uint32_t lastValidTick;
static uint32_t lastParseTick;
static uint32_t reconnectTick;

/* 私有函数声明 */
static void VT03_DecodeFrame(const uint8_t *frameRaw);
static void VT03_StateMachineHandle(void);
static void VT03_SafeReset(void);
static void VT03_UpdateDataValidFlag(void);
/**
 * @brief VT03串口DMA循环接收初始化
 */
void VT03_Init(UART_HandleTypeDef *huart)
{
    vt03Huart = huart;

    memset(&vt03Data, 0, sizeof(vt03Data));
    memset(vt03RxCyclicBuf, 0, sizeof(vt03RxCyclicBuf));

    // 初始状态
    vt03State = VT03_STATE_RECONNECTING;
    newDataFlag = 0;
    lastValidTick = HAL_GetTick();
    lastParseTick = HAL_GetTick();
    reconnectTick = HAL_GetTick();
    vt03Data.crc_ok = 0;

    // 开启空闲中断IDLE
    __HAL_UART_ENABLE_IT(vt03Huart, UART_IT_IDLE);

    // 启动DMA循环接收（核心改进：循环模式，永不停止）
    HAL_UART_Receive_DMA(vt03Huart, vt03RxCyclicBuf, VT03_RX_BUF_SIZE);
}

/**
 * @brief 获取只读数据指针，供外部任务读取
 */
const VideoTx_Ctrl_t *GetVT03DataPtr(void)
{
    return &vt03Data;
}

/**
 * @brief 主循环/RTOS任务定期调用，解析缓冲区、跑状态机
 */
void VT03_Process(void)
{
    uint16_t i;
    uint8_t frameFindFlag = 0;

    VT03_StateMachineHandle();

    if (newDataFlag || (HAL_GetTick() - lastParseTick > VT03_FRAME_INTERVAL))
    {
        // 滑动查找双字节帧头 0xA9 0x53
        for (i = 0; i <= VT03_RX_BUF_SIZE - RC_FRAME_SIZE; i++)
        {
            // 双字节帧头匹配，且剩余空间足够一整帧21字节
            if (vt03RxCyclicBuf[i] == 0xA9 && vt03RxCyclicBuf[i+1] == 0x53)
            {
                VT03_DecodeFrame(&vt03RxCyclicBuf[i]);
                // 只有帧头+CRC全部校验通过才判定找到帧
                if(vt03Data.crc_ok == 1)
                {
                    frameFindFlag = 1;
                    lastValidTick = HAL_GetTick();
                    memset(&vt03RxCyclicBuf[i], 0, RC_FRAME_SIZE);
                    break;
                }
            }
        }

        newDataFlag = 0;
        lastParseTick = HAL_GetTick();

        if (frameFindFlag)
        {
            vt03State = VT03_STATE_NORMAL;
        }
        else
        {
            vt03Data.crc_ok = 0;
        }
    }

    // 统一更新is_valid标志
    VT03_UpdateDataValidFlag();
}

/**
 * @brief 外部接口：判断当前数据是否有效
 */
uint8_t VT03_IsDataValid(void)
{
    return (vt03State == VT03_STATE_NORMAL) && (vt03Data.crc_ok == 1);
}

/**
 * @brief 判断VT03设备是否断电失联
 */
uint8_t VT03_IsPowerLost(void)
{
    return (vt03State == VT03_STATE_POWER_LOST);
}

/**
 * @brief 外部手动强制重置接收链路
 */
void VT03_ResetReceiver(void)
{
    VT03_SafeReset();
    vt03State = VT03_STATE_RECONNECTING;
    reconnectTick = HAL_GetTick();
}

/* --------------------------私有函数-------------------------- */

/**
 * @brief 根据链路状态+连续CRC失败次数，更新数据有效标志is_valid
 * 规则：
 * 1. 通信状态不在NORMAL，直接失效
 * 2. 连续CRC错误超过阈值，数据失效
 * 3. 状态正常且CRC连续正确，数据有效
 */
static void VT03_UpdateDataValidFlag(void)
{
    // 两种情况置无效：链路断开 或 连续多帧CRC出错
    if (vt03State != VT03_STATE_NORMAL || crc_fail_cnt >= CRC_FAIL_MAX)
    {
        vt03Data.is_valid = 0;
    }
    else
    {
        vt03Data.is_valid = 1;
    }
}

/**
 * @brief 解析原始21字节VT03协议帧（移植原有VT_ParseFrame完整逻辑）
 * @param frameRaw 指向缓冲区中一帧完整21字节数据起始地址
 */
static void VT03_DecodeFrame(const uint8_t *frameRaw)
{
    VT03_ParseFrame(frameRaw, &vt03Data);

    if (vt03Data.crc_ok == 1)
    {
        crc_fail_cnt = 0; // 校验正常，清零失败计数
    }
    else
    {
        if(crc_fail_cnt < CRC_FAIL_MAX)
        {
            crc_fail_cnt++; // 校验失败，计数累加
        }
    }
}

/**
 * @brief 通信状态机：超时判定丢信号/断电、自动重连
 */
static void VT03_StateMachineHandle(void)
{
    uint32_t tickNow = HAL_GetTick();
    uint32_t timeNoFrame = tickNow - lastValidTick;

    switch (vt03State)
    {
        case VT03_STATE_NORMAL:
            if (timeNoFrame > VT03_POWER_TIMEOUT)
            {
                vt03State = VT03_STATE_POWER_LOST;
                vt03Data.crc_ok = 0;
            }
            else if (timeNoFrame > VT03_LOST_TIMEOUT)
            {
                vt03State = VT03_STATE_SIGNAL_LOST;
                vt03Data.crc_ok = 0;
            }
            break;

        case VT03_STATE_SIGNAL_LOST:
            if (timeNoFrame > VT03_POWER_TIMEOUT)
            {
                vt03State = VT03_STATE_POWER_LOST;
            }
            break;

        case VT03_STATE_POWER_LOST:
            // 断电后每秒自动重置一次DMA，尝试恢复通信
            static uint32_t lastAutoReset = 0;
            if (tickNow - lastAutoReset > 1000U)
            {
                VT03_ResetReceiver();
                lastAutoReset = tickNow;
            }
            break;

        case VT03_STATE_RECONNECTING:
            if (timeNoFrame > VT03_POWER_TIMEOUT)
            {
                vt03State = VT03_STATE_POWER_LOST;
            }
            break;
    }
}

/**
 * @brief 安全重置DMA+串口，清溢出错误，防止串口卡死
 */
static void VT03_SafeReset(void)
{
    // 停止DMA
    HAL_UART_AbortReceive(vt03Huart);
    // 等待DMA停止完成
    while(HAL_UART_GetState(vt03Huart) == HAL_UART_STATE_BUSY_RX);

    // 清空所有错误标志
    __HAL_UART_CLEAR_FLAG(vt03Huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
    (void)vt03Huart->Instance->DR;

    // 清空缓存
    memset(vt03RxCyclicBuf, 0, sizeof(vt03RxCyclicBuf));
    newDataFlag = 0;
    vt03Data.crc_ok = 0;

    // 重新开启IDLE中断 + 循环DMA
    __HAL_UART_ENABLE_IT(vt03Huart, UART_IT_IDLE);
    HAL_UART_Receive_DMA(vt03Huart, vt03RxCyclicBuf, VT03_RX_BUF_SIZE);
}

/**
 * @brief 串口中断服务函数，放到USART6_IRQHandler中调用
 */
void VT03_UART_IRQHandler(void)
{
    // 空闲中断：上位机一帧发送完毕，置解析标志
    if (__HAL_UART_GET_FLAG(vt03Huart, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(vt03Huart);
        newDataFlag = 1;
    }

    // 清除硬件错误标志，避免串口锁死不再接收
    if (__HAL_UART_GET_FLAG(vt03Huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(vt03Huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
        (void)vt03Huart->Instance->DR;
    }
}



static UBaseType_t remain_VTTask;
__attribute__((used)) void VTTask(void *argument)
{
    // 初始化串口6，huart6为你CubeMX生成的句柄
    VT03_Init(&huart6);
    buzzer_t *buzzer = get_buzzer_effect_point();

    for(;;)
    {
        VT03_Process();

        if(VT03_KeyTest(&vt03Data))
        {
            buzzer->sound_effect = B_;
        }

        // 栈余量监控
        remain_VTTask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(1);
    }
}

#endif /* BOARD_ID == GIMBAL_BOARD */
#endif

//==================== 私有函数声明 ====================
static uint8_t VT_DecodeFrame(VT_Instance_t *inst, const uint8_t *frame_raw);
static void VT_StateMachineHandle(VT_Instance_t *inst);
static void VT_SafeReset(VT_Instance_t *inst);
static void VT_UpdateValidFlag(VT_Instance_t *inst);
static VT_Instance_t *vt_inst_map[VT_MAX_INST] = {0};

void VT_RegisterInstance(UART_HandleTypeDef *huart, VT_Instance_t *inst)
{
    for(uint8_t i=0; i<VT_MAX_INST; i++)
    {
        if(vt_inst_map[i] == 0)
        {
            vt_inst_map[i] = inst;
            break;
        }
    }
}

VT_Instance_t *VT_GetInstanceByUart(UART_HandleTypeDef *huart)
{
    for(uint8_t i=0; i<VT_MAX_INST; i++)
    {
        if(vt_inst_map[i] && vt_inst_map[i]->huart == huart)
        {
            return vt_inst_map[i];
        }
    }
    return NULL;
}

//==================== 对外初始化 ====================
void VT_Init(VT_Instance_t *inst,
             UART_HandleTypeDef *huart,
             const VT_Config_t *cfg,
             VT_ParseCallback parse_cb,
             uint8_t *rx_buf,
             void *data_buf)
{
    memset(inst, 0, sizeof(VT_Instance_t));
    inst->huart     = huart;
    inst->cfg       = *cfg;
    inst->parse_cb  = parse_cb;
    inst->rx_cyclic_buf = rx_buf;
    inst->user_data = data_buf;

    memset(inst->rx_cyclic_buf, 0, inst->cfg.rx_buf_len);
    inst->state = VT_STATE_RECONNECTING;
    inst->new_data_flag = 0;
    inst->last_valid_tick = HAL_GetTick();
    inst->last_parse_tick = HAL_GetTick();
    inst->reconnect_tick = HAL_GetTick();
    inst->crc_fail_cnt = 0;
    inst->scan_offset = 0;

    VT_RegisterInstance(huart,inst);

    // 开启空闲中断
    __HAL_UART_ENABLE_IT(inst->huart, UART_IT_IDLE);
    // 启动循环DMA接收
    HAL_UART_Receive_DMA(inst->huart, inst->rx_cyclic_buf, inst->cfg.rx_buf_len);
}

//==================== 周期处理入口 ====================
void VT_Process(VT_Instance_t *inst)
{
    uint16_t i;
    uint8_t frame_find_flag = 0;
    const VT_Config_t *cfg = &inst->cfg;

    VT_StateMachineHandle(inst);

    if (inst->new_data_flag || (HAL_GetTick() - inst->last_parse_tick > cfg->scan_interval_ms))
    {
        // 从上次扫描位置开始，环形扫描一圈（最多 rx_buf_len 次，防止死循环）
        for (uint16_t cnt = 0; cnt < cfg->rx_buf_len; cnt++)
        {
            i = inst->scan_offset + cnt;
            if (i >= cfg->rx_buf_len)
                i -= cfg->rx_buf_len;  // 环形回卷

            // 剩余空间不足一帧，跳过
            if(i + cfg->frame_len > cfg->rx_buf_len)
                continue;

            if (inst->rx_cyclic_buf[i] == cfg->head0 && inst->rx_cyclic_buf[i+1] == cfg->head1)
            {
                if (VT_DecodeFrame(inst, &inst->rx_cyclic_buf[i]) == 1)
                {
                    frame_find_flag = 1;
                    inst->last_valid_tick = HAL_GetTick();
                    memset(&inst->rx_cyclic_buf[i], 0, cfg->frame_len);
                    inst->scan_offset = (i + cfg->frame_len) % cfg->rx_buf_len;
                    break;
                }
            }
        }

        // 未找到有效帧时不改变 scan_offset，下次从同一位置继续扫描
        inst->new_data_flag = 0;
        inst->last_parse_tick = HAL_GetTick();
        if (frame_find_flag)
        {
            inst->state = VT_STATE_NORMAL;
        }
    }
    VT_UpdateValidFlag(inst);
}

//==================== 帧解析调度（调用对应型号回调） ====================
static uint8_t VT_DecodeFrame(VT_Instance_t *inst, const uint8_t *frame_raw)
{
    uint8_t crc_ok = inst->parse_cb(frame_raw, inst->user_data);

    // 更新CRC连续失败计数
    if (crc_ok == 1)
    {
        inst->crc_fail_cnt = 0;
    }
    else
    {
        if (inst->crc_fail_cnt < inst->cfg.crc_fail_max)
        {
            inst->crc_fail_cnt++;
        }
    }

    return crc_ok;
}

//==================== 通信状态机 ====================
static void VT_StateMachineHandle(VT_Instance_t *inst)
{
    uint32_t tick_now = HAL_GetTick();
    uint32_t time_no_frame = tick_now - inst->last_valid_tick;
    const VT_Config_t *cfg = &inst->cfg;

    switch (inst->state)
    {
        case VT_STATE_NORMAL:
            if (time_no_frame > cfg->power_lost_ms)
            {
                inst->state = VT_STATE_POWER_LOST;
            }
            else if (time_no_frame > cfg->signal_lost_ms)
            {
                inst->state = VT_STATE_SIGNAL_LOST;
            }
            break;
        case VT_STATE_SIGNAL_LOST:
            if (time_no_frame > cfg->power_lost_ms)
            {
                inst->state = VT_STATE_POWER_LOST;
            }
            break;
        case VT_STATE_POWER_LOST:
            static uint32_t auto_reset_tick = 0;
            // 5秒才自动重置一次，降低串口启停频率
            if (tick_now - auto_reset_tick > 5000U)
            {
                VT_ResetReceiver(inst);
                auto_reset_tick = tick_now;
            }
            break;
        case VT_STATE_RECONNECTING:
            if (time_no_frame > cfg->power_lost_ms)
            {
                inst->state = VT_STATE_POWER_LOST;
            }
            break;
    }
}

//==================== 更新数据有效标志 ====================
static void VT_UpdateValidFlag(VT_Instance_t *inst)
{
    const VT_Config_t *cfg = &inst->cfg;
    uint8_t *crc_ok_ptr   = (uint8_t *)inst->user_data + cfg->crc_ok_offset;
    uint8_t *is_valid_ptr = (uint8_t *)inst->user_data + cfg->is_valid_offset;

    if (inst->state != VT_STATE_NORMAL || inst->crc_fail_cnt >= cfg->crc_fail_max)
    {
        // 链路异常 或 连续CRC失败达阈值 → 强制无效
        // crc_ok不受影响，它独立反映最近一帧的CRC校验结果
        *is_valid_ptr = 0;
    }
    else if (*crc_ok_ptr == 1)
    {
        // CRC校验通过且链路正常 → 数据有效
        *is_valid_ptr = 1;
    }
    /* else: CRC校验失败但连续次数未达阈值 → 保持当前is_valid不变（防抖） */
}

//==================== 安全重置串口DMA，清除硬件错误 ====================
static void VT_SafeReset(VT_Instance_t *inst)
{
    UART_HandleTypeDef *huart = inst->huart;
    // 停止DMA，移除while死循环阻塞
    HAL_UART_AbortReceive(huart);

    // 清溢出、帧错误、奇偶错误
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
    (void)huart->Instance->DR;

    memset(inst->rx_cyclic_buf, 0, inst->cfg.rx_buf_len);
    inst->new_data_flag = 0;
    inst->crc_fail_cnt = 0;

    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    HAL_UART_Receive_DMA(huart, inst->rx_cyclic_buf, inst->cfg.rx_buf_len);
}

void VT_ResetReceiver(VT_Instance_t *inst)
{
    VT_SafeReset(inst);
    inst->state = VT_STATE_RECONNECTING;
    inst->last_valid_tick = HAL_GetTick();
    inst->reconnect_tick = inst->last_valid_tick;
}

//==================== 串口中断处理 ====================
void VT_UART_IRQHandler(VT_Instance_t *inst)
{
    UART_HandleTypeDef *huart = inst->huart;
    // IDLE空闲中断
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        inst->new_data_flag = 1;
    }
    // 硬件错误清除
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
        (void)huart->Instance->DR;
    }
}

//==================== 状态查询接口 ====================
uint8_t VT_IsDataValid(const VT_Instance_t *inst)
{
    uint8_t *is_valid_ptr = (uint8_t *)inst->user_data + inst->cfg.is_valid_offset;
    return *is_valid_ptr;
}

uint8_t VT_IsPowerLost(const VT_Instance_t *inst)
{
    return (inst->state == VT_STATE_POWER_LOST);
}



//========= VT03 实例资源 =========
static uint8_t vt03_rx_buf[64];
static VideoTx_Ctrl_t vt03_data;
static VT_Instance_t vt03_inst;
const VT_Config_t VT03_CFG = {
    .frame_len = 21,
    .rx_buf_len = 64,
    .head0 = 0xA9,
    .head1 = 0x53,
    .scan_interval_ms = 10,
    .signal_lost_ms = 100,
    .power_lost_ms = 500,
    .crc_fail_max = 3,
    .crc_ok_offset   = offsetof(VT03_Data_t, crc_ok),
    .is_valid_offset = offsetof(VT03_Data_t, is_valid)
};

static UBaseType_t remain_VTTask;
__attribute__((used)) void VTTask(void *argument)
{
    // 初始化串口6，huart6为你CubeMX生成的句柄
    buzzer_t *buzzer = get_buzzer_effect_point();
    VT_Init(&vt03_inst, &huart6, &VT03_CFG, VT03_ParseCallback, vt03_rx_buf, &vt03_data);

    for(;;)
    {

        VT_Process(&vt03_inst);
        VT03_Data_t *rc = VT_GET_DATA(VT03_Data_t, &vt03_inst);

        // 栈余量监控
        remain_VTTask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(1);
    }
}

