#include "VTCenter.h"

#if(BOARD_ID == GIMBAL_BOARD)

// // 定义接收缓冲区，大小严格匹配协议帧长
// static uint8_t vt03_rx_buf[RC_FRAME_SIZE];

// /**
//  * @brief 初始化 VT03 串口 DMA 接收
//  */
// void VT03_DMA_Init(void)
// {
//     // 启动第一次 DMA 接收，固定接收 21 字节
//     HAL_UART_Receive_DMA(&huart6, vt03_rx_buf, RC_FRAME_SIZE);
// }

// VideoTx_Ctrl_t RX_data = {0};

// /**
//  * @brief UART 接收完成回调函数（由 HAL 库在中断中自动调用）
//  * @note  在普通模式下，每次收满 21 字节触发一次
//  */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     // 1. 确认是串口6的接收完成中断
//     if (huart->Instance == USART6) 
//     {
//         // 2. 获取解析后的数据结构指针
//         //VideoTx_Ctrl_t *rc_data = (VideoTx_Ctrl_t *)get_VideoTx_Ctl_point();
        
//         // 3. 调用你提供的协议解析函数（内部包含帧头校验和CRC校验）
//         VT_ParseFrame(vt03_rx_buf, &RX_data);
        
//         // 4. 【关键】DMA 普通模式下，接收完成后 DMA 会自动停止，
//         //    必须在这里重新启动下一次 21 字节的接收，否则将不再接收数据
//         HAL_UART_Receive_DMA(&huart6, vt03_rx_buf, RC_FRAME_SIZE);
//     }
// }

// static UBaseType_t remain_VTTask;
// __attribute__((used)) void VTTask(void *argument)
// {
//      VT03_DMA_Init(); 

//     for(;;)
//     {

//         remain_VTTask = uxTaskGetStackHighWaterMark(NULL);
//         osDelay(1);
//     }
// }



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
    VT_ParseFrame(frameRaw, &vt03Data);

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

    for(;;)
    {
        VT03_Process();

        // 栈余量监控
        remain_VTTask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(1);
    }
}

#endif /* BOARD_ID == GIMBAL_BOARD */