#include "VTCenter_Instance.h"

#if(BOARD_ID == GIMBAL_BOARD)

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
            return;
        }
    }
    /* 实例注册表已满，不应发生 */
    Error_Handler();
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
            // 5秒才自动重置一次，降低串口启停频率
            if (tick_now - inst->reconnect_tick > 5000U)
            {
                VT_ResetReceiver(inst);
                inst->reconnect_tick = tick_now;
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

#endif
