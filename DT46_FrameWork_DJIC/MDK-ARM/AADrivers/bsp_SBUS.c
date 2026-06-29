#include "bsp_SBUS.h"

/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef *sbusHuart;
static uint8_t sbusRxBuffer[SBUS_RX_BUF_SIZE]; /* DMA 循环接收缓冲区 */

/* Private data --------------------------------------------------------------*/
static SbusData_t sbusData;

/* Private function prototypes ----------------------------------------------*/
static void SBUS_DecodeFrame(const uint8_t *frameData);
static void SBUS_HandleStateMachine(void);
static void SBUS_SafeReset(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief 初始化 SBUS 接收
 * @param huart SBUS 对应的 UART 句柄
 */
void SBUS_Init(UART_HandleTypeDef *huart) {
    sbusHuart = huart;

    memset(&sbusData, 0, sizeof(sbusData));
    memset(sbusRxBuffer, 0, sizeof(sbusRxBuffer));
    
    /* 初始状态：数据无效，等待连接 */
    sbusData.is_valid = 0;
    sbusData.state = SBUS_STATE_RECONNECTING;
    sbusData.lastValidTick = HAL_GetTick();
    sbusData.lastParseTick = HAL_GetTick();
    sbusData.reconnectTick = HAL_GetTick();

    /* 启用 UART IDLE 中断 */
    __HAL_UART_ENABLE_IT(sbusHuart, UART_IT_IDLE);

    /* 启用 DMA 循环接收 */
    HAL_UART_Receive_DMA(sbusHuart, sbusRxBuffer, SBUS_RX_BUF_SIZE);
}

/**
 * @brief 获取当前遥控器数据的只读指针
 * @return const SbusData_t*
 */
const SbusData_t *get_SBUS_Data_point(void)
{
    return &sbusData;
}

/**
 * @brief 处理 SBUS 数据帧，需在主循环中定期调用
 */
void SBUS_Process(void) {
    uint16_t i;
    uint8_t frameFound = 0;

    /* 强制每1ms执行一次状态机 */
    SBUS_HandleStateMachine();

    /* 只要有新数据或者超过帧间隔就检查缓冲区 */
    if (sbusData.newDataFlag || (HAL_GetTick() - sbusData.lastParseTick > SBUS_FRAME_INTERVAL)) {
        /* 扫描整个缓冲区寻找完整帧 */
        for (i = 0; i <= SBUS_RX_BUF_SIZE - SBUS_FRAME_LEN; i++) {
            if (sbusRxBuffer[i] == SBUS_HEADER && sbusRxBuffer[i + 24] == SBUS_FOOTER) {
                SBUS_DecodeFrame(&sbusRxBuffer[i]);
                frameFound = 1;
                sbusData.lastValidTick = HAL_GetTick();
                
                /* 清空已解析的数据，防止重复解析 */
                memset(&sbusRxBuffer[i], 0, SBUS_FRAME_LEN);
                break;
            }
        }

        sbusData.newDataFlag = 0;
        sbusData.lastParseTick = HAL_GetTick();

        /* 修复：只要在重连状态下找到有效帧，立即设置为有效 */
        if (frameFound) {
            if (sbusData.state == SBUS_STATE_RECONNECTING) {
                sbusData.state = SBUS_STATE_NORMAL;
                sbusData.is_valid = 1;
            }
            else if (sbusData.state == SBUS_STATE_SIGNAL_LOST || sbusData.state == SBUS_STATE_POWER_LOST) {
                /* 从异常状态恢复 */
                sbusData.state = SBUS_STATE_NORMAL;
                sbusData.is_valid = 1;
            }
        }
    }
}

/**
 * @brief 检查遥控器数据是否有效
 * @return 1 表示数据有效，0 表示无效
 */
uint8_t SBUS_IsDataValid(void)
{
    return sbusData.is_valid;
}

/**
 * @brief 检查SBUS接收端是否断电
 * @return 1 表示断电，0 表示正常
 */
uint8_t SBUS_IsPowerLost(void)
{
    return (sbusData.state == SBUS_STATE_POWER_LOST);
}

/**
 * @brief 强制重置SBUS接收
 */
void SBUS_ResetReceiver(void) {
    SBUS_SafeReset();
    
    /* 进入重连状态 */
    sbusData.state = SBUS_STATE_RECONNECTING;
    sbusData.reconnectTick = HAL_GetTick();
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief 解码一帧 SBUS 数据
 * @param d 指向帧头的指针
 */
static void SBUS_DecodeFrame(const uint8_t *d) {
    sbusData.channels[0]  = (((uint16_t)d[1]  << 0)  | ((uint16_t)d[2]  << 8)) & 0x07FF;
    sbusData.channels[1]  = (((uint16_t)d[2]  >> 3)  | ((uint16_t)d[3]  << 5)) & 0x07FF;
    sbusData.channels[2]  = (((uint16_t)d[3]  >> 6)  | ((uint16_t)d[4]  << 2)  | ((uint16_t)d[5] << 10)) & 0x07FF;
    sbusData.channels[3]  = (((uint16_t)d[5]  >> 1)  | ((uint16_t)d[6]  << 7)) & 0x07FF;
    sbusData.channels[4]  = (((uint16_t)d[6]  >> 4)  | ((uint16_t)d[7]  << 4)) & 0x07FF;
    sbusData.channels[5]  = (((uint16_t)d[7]  >> 7)  | ((uint16_t)d[8]  << 1)  | ((uint16_t)d[9] << 9))  & 0x07FF;
    sbusData.channels[6]  = (((uint16_t)d[9]  >> 2)  | ((uint16_t)d[10] << 6)) & 0x07FF;
    sbusData.channels[7]  = (((uint16_t)d[10] >> 5)  | ((uint16_t)d[11] << 3)) & 0x07FF;

    sbusData.channels[8]  = (((uint16_t)d[12] << 0)  | ((uint16_t)d[13] << 8)) & 0x07FF;
    sbusData.channels[9]  = (((uint16_t)d[13] >> 3)  | ((uint16_t)d[14] << 5)) & 0x07FF;
    sbusData.channels[10] = (((uint16_t)d[14] >> 6)  | ((uint16_t)d[15] << 2)  | ((uint16_t)d[16] << 10)) & 0x07FF;
    sbusData.channels[11] = (((uint16_t)d[16] >> 1)  | ((uint16_t)d[17] << 7)) & 0x07FF;
    sbusData.channels[12] = (((uint16_t)d[17] >> 4)  | ((uint16_t)d[18] << 4)) & 0x07FF;
    sbusData.channels[13] = (((uint16_t)d[18] >> 7)  | ((uint16_t)d[19] << 1)  | ((uint16_t)d[20] << 9))  & 0x07FF;
    sbusData.channels[14] = (((uint16_t)d[20] >> 2)  | ((uint16_t)d[21] << 6)) & 0x07FF;
    sbusData.channels[15] = (((uint16_t)d[21] >> 5)  | ((uint16_t)d[22] << 3)) & 0x07FF;

    sbusData.ch17      = (d[23] >> 0) & 0x01;
    sbusData.ch18      = (d[23] >> 1) & 0x01;
    sbusData.frameLost = (d[23] >> 2) & 0x01;
    sbusData.failsafe  = (d[23] >> 3) & 0x01;
}

/**
 * @brief SBUS 状态机处理（强制每1ms执行一次）
 */
static void SBUS_HandleStateMachine(void) {
    uint32_t currentTick = HAL_GetTick();
    uint32_t timeSinceLastValid = currentTick - sbusData.lastValidTick;

    switch (sbusData.state) {
        case SBUS_STATE_NORMAL:
            /* 同时检查协议自带标志和超时 */
            if (sbusData.failsafe || sbusData.frameLost) {
                sbusData.is_valid = 0;
                sbusData.state = SBUS_STATE_SIGNAL_LOST;
            }
            else if (timeSinceLastValid > SBUS_POWER_LOST_TIMEOUT) {
                /* 500ms未收到数据，判定为接收端断电 */
                sbusData.is_valid = 0;
                sbusData.state = SBUS_STATE_POWER_LOST;
            }
            else if (timeSinceLastValid > SBUS_LOST_TIMEOUT) {
                /* 100ms未收到数据，判定为信号丢失 */
                sbusData.is_valid = 0;
                sbusData.state = SBUS_STATE_SIGNAL_LOST;
            }
            break;

        case SBUS_STATE_SIGNAL_LOST:
            if (timeSinceLastValid > SBUS_POWER_LOST_TIMEOUT) {
                /* 信号丢失时间过长，升级为断电状态 */
                sbusData.state = SBUS_STATE_POWER_LOST;
            }
            break;

        case SBUS_STATE_POWER_LOST:
            /* 断电状态下，每秒执行一次安全重置 */
            static uint32_t lastResetTick = 0;
            if (currentTick - lastResetTick > 1000) {
                SBUS_ResetReceiver();
                lastResetTick = currentTick;
            }
            break;

        case SBUS_STATE_RECONNECTING:
            if (timeSinceLastValid > SBUS_POWER_LOST_TIMEOUT) {
                /* 重连超时，回到断电状态 */
                sbusData.state = SBUS_STATE_POWER_LOST;
            }
            break;
    }
}

/**
 * @brief 安全重置SBUS接收（不会导致系统卡死）
 * @note 移除了危险的UART/DMA反初始化操作
 */
static void SBUS_SafeReset(void) {
    /* 1. 停止DMA接收 */
    HAL_UART_AbortReceive(sbusHuart);
    
    /* 2. 清空UART接收FIFO和DR寄存器 */
    __HAL_UART_CLEAR_FLAG(sbusHuart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
    (void)sbusHuart->Instance->DR;
    
    /* 3. 清空DMA缓冲区 */
    memset(sbusRxBuffer, 0, sizeof(sbusRxBuffer));
    sbusData.newDataFlag = 0;
    sbusData.failsafe = 0;
    sbusData.frameLost = 0;
    
    /* 4. 重新启用DMA接收 */
    HAL_UART_Receive_DMA(sbusHuart, sbusRxBuffer, SBUS_RX_BUF_SIZE);
}

/**
 * @brief SBUS UART IDLE 中断处理函数
 * @note 需在 USARTx_IRQHandler 中调用(目前写在USART3_IRQHandler)
 */
void SBUS_UART_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(sbusHuart, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(sbusHuart);
        sbusData.newDataFlag = 1;
    }
    
    /* 清除所有可能的错误标志，防止UART卡死 */
    if (__HAL_UART_GET_FLAG(sbusHuart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE) != RESET) {
        __HAL_UART_CLEAR_FLAG(sbusHuart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
        (void)sbusHuart->Instance->DR;
    }
}

static UBaseType_t remain_SBUSTask;
__attribute__((used)) void SBUSTask(void *argument)
{
  /* USER CODE BEGIN SBUSTask */
  /* Infinite loop */
  for(;;)
  {
    SBUS_Process();
		//=============================检测剩余栈=================================//
	  remain_SBUSTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
  /* USER CODE END SBUSTask */
}


