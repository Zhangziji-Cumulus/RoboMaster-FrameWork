#include "RefereeCenter.h"

#if(BOARD_ID == CHASSIS_BOARD)	

/* 外部串口句柄声明，对应CubeMX生成的huart6 */
extern UART_HandleTypeDef REFEREE_UART;

/************************ 内部实例结构体 ************************/
typedef struct
{
    /* 双接收缓冲：DMA乒乓写入，读写分离无冲突 */
    uint8_t  Rx_Buf[2][REFEREE_RX_BUF_SIZE];
    /* 当前DMA正在写入的缓冲区索引 0/1 */
    uint8_t  Rx_ActiveBuf;
    /* 解析锁定缓存：空闲中断时拷贝出的完整数据段 */
    uint8_t  Rx_ParseBuf[REFEREE_RX_BUF_SIZE];
    /* 当前待解析的有效字节长度 */
    uint16_t Rx_ParseLen;
    /* 协议解析器句柄（状态机+帧缓存） */
    referee_parser_t Parser;
    /* 发送包序号，单调自增 */
    uint8_t  Tx_Seq;
} referee_instance_t;

/* 裁判系统单例 */
static referee_instance_t g_ref_ins;

/* 任务剩余栈空间统计（保留你指定的变量） */
static UBaseType_t remain_RefereeTask;

/************************ 内部静态函数声明 ************************/
static void Referee_ReceiveProcess(void);

/**********************************************************************
 * @brief  裁判系统初始化
 * @note   初始化解析状态机、双缓冲、启动DMA接收与空闲中断
 *********************************************************************/
void Referee_Init(void)
{
    /* 1. 初始化协议解析状态机 */
    Referee_Parser_Init(&g_ref_ins.Parser);

    /* 2. 双缓冲索引初始化 */
    g_ref_ins.Rx_ActiveBuf = 0;
    g_ref_ins.Tx_Seq = 0;
    g_ref_ins.Rx_ParseLen = 0;

    /* 3. 清空接收缓冲区 */
    memset(g_ref_ins.Rx_Buf[0], 0, REFEREE_RX_BUF_SIZE);
    memset(g_ref_ins.Rx_Buf[1], 0, REFEREE_RX_BUF_SIZE);
    memset(g_ref_ins.Rx_ParseBuf, 0, REFEREE_RX_BUF_SIZE);

    /* 4. 启动首次DMA接收 */
    HAL_UART_Receive_DMA(&REFEREE_UART,
                        (uint8_t *)g_ref_ins.Rx_Buf[g_ref_ins.Rx_ActiveBuf],
                        REFEREE_RX_BUF_SIZE);

    /* 5. 开启串口空闲中断 */
    __HAL_UART_ENABLE_IT(&REFEREE_UART, UART_IT_IDLE);
}

/**********************************************************************
 * @brief  串口空闲中断处理函数
 * @note   逻辑完全对齐自瞄接收架构：拷贝缓存→切换缓冲→重启DMA→解析数据
 * @note   需放入 USART6_IRQHandler 中调用
 *********************************************************************/
void Referee_UART_IRQHandler(void)
{
    /* 先执行HAL库通用串口中断处理 */
    HAL_UART_IRQHandler(&REFEREE_UART);

    /* 检测空闲中断标志 */
    if (__HAL_UART_GET_FLAG(&REFEREE_UART, UART_FLAG_IDLE) != RESET)
    {
        /* 清除空闲中断标志 */
        __HAL_UART_CLEAR_IDLEFLAG(&REFEREE_UART);

        /* 清除硬件错误标志（ORE/NE/FE/PE），防止错误累积锁死UART */
        if (__HAL_UART_GET_FLAG(&REFEREE_UART, UART_FLAG_ORE | UART_FLAG_NE |
                                                UART_FLAG_FE | UART_FLAG_PE) != RESET)
        {
            __HAL_UART_CLEAR_FLAG(&REFEREE_UART, UART_FLAG_ORE | UART_FLAG_NE |
                                                  UART_FLAG_FE | UART_FLAG_PE);
            (void)REFEREE_UART.Instance->DR;  /* 读DR完成硬件清除 */
        }

        /* 计算本次实际接收字节数 = 缓冲区总长 - DMA剩余传输计数 */
        uint16_t recv_len = REFEREE_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(REFEREE_UART.hdmarx);

        /* 1. 将DMA写完的整段数据拷贝到解析缓存，避免被硬件覆写 */
        memcpy(g_ref_ins.Rx_ParseBuf,
               g_ref_ins.Rx_Buf[g_ref_ins.Rx_ActiveBuf],
               recv_len);
        g_ref_ins.Rx_ParseLen = recv_len;

        /* 2. 切换DMA目标缓冲区（乒乓切换） */
        g_ref_ins.Rx_ActiveBuf ^= 1U;

        /* 2.5 复位UART状态机，允许重新配置DMA */
        HAL_UART_AbortReceive(&REFEREE_UART);

        /* 3. 重启DMA接收，硬件写入新缓冲区 */
        HAL_UART_Receive_DMA(&REFEREE_UART,
                            (uint8_t *)g_ref_ins.Rx_Buf[g_ref_ins.Rx_ActiveBuf],
                            REFEREE_RX_BUF_SIZE);

        /* 4. 解析锁定好的一整段数据 */
        Referee_ReceiveProcess();
    }
}

/**********************************************************************
 * @brief  字节流解析处理
 * @note   将缓存中的所有字节逐个喂给协议状态机，拼帧校验通过自动触发回调
 *********************************************************************/
static void Referee_ReceiveProcess(void)
{
    for (uint16_t i = 0; i < g_ref_ins.Rx_ParseLen; i++)
    {
        Referee_Parser_Byte(&g_ref_ins.Parser, g_ref_ins.Rx_ParseBuf[i]);
    }
}

/**********************************************************************
 * @brief  FreeRTOS 裁判系统业务任务
 * @note   中断仅做拼包+置标志，所有业务逻辑统一在任务中处理
 * @note   保留你指定的栈检测结构与osDelay(1)
 *********************************************************************/
__attribute__((used)) void RefereeTask(void *argument)
{
    Referee_Init();
    for (;;)
    {
        /* ===================== 检测剩余栈 ===================== */
        remain_RefereeTask = uxTaskGetStackHighWaterMark(NULL);

        /* 无任何新数据直接延时，释放CPU */
        if (g_ref_data.update.all_flag == 0)
        {
            osDelay(1);
            continue;
        }

        osDelay(1);
    }
}

#endif