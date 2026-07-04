#include "RefereeCenter.h"

#if(BOARD_ID == CHASSIS_BOARD)	

/************************* 静态全局变量声明 *************************/
/* 裁判系统数据存储（仅本文件持有，外部通过 Referee_GetData() 只读访问） */
static referee_all_data_t s_ref_data;

/* 外部串口句柄声明，对应CubeMX生成的huart6 */
extern UART_HandleTypeDef REFEREE_UART;

/************************ 内部实例结构体 ************************/
typedef struct
{
    /* 环形DMA接收缓冲区：Circular模式，硬件自动绕回 */
    uint8_t  Rx_CyclicBuf[REFEREE_RX_BUF_SIZE];
    /* 上一次处理时的DMA写位置索引（由__HAL_DMA_GET_COUNTER换算） */
    uint16_t Rx_LastReadIdx;
    /* 解析锁定缓存：任务中从环形缓冲区拷贝出的新增数据段 */
    uint8_t  Rx_ParseBuf[REFEREE_RX_BUF_SIZE];
    /* 当前待解析的有效字节长度 */
    uint16_t Rx_ParseLen;
    /* 新数据标志：IDLE中断中置位，任务中消费 */
    volatile uint8_t NewDataFlag;
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
static void Referee_ProcessDelta(void);

/**********************************************************************
 * @brief  裁判系统初始化
 * @note   初始化解析状态机、双缓冲、启动DMA接收与空闲中断
 *********************************************************************/
void Referee_Init(void)
{
    /* 1. 清空数据存储，初始化协议解析状态机（注入数据指针） */
    memset(&s_ref_data, 0, sizeof(referee_all_data_t));
    Referee_Parser_Init(&g_ref_ins.Parser, &s_ref_data);

    /* 2. 环形DMA参数初始化 */
    g_ref_ins.Tx_Seq = 0;
    g_ref_ins.NewDataFlag = 0;
    g_ref_ins.Rx_LastReadIdx = 0;
    g_ref_ins.Rx_ParseLen = 0;

    /* 3. 清空接收缓冲区 */
    memset(g_ref_ins.Rx_CyclicBuf, 0, REFEREE_RX_BUF_SIZE);
    memset(g_ref_ins.Rx_ParseBuf, 0, REFEREE_RX_BUF_SIZE);

    /* 4. 启动环形DMA接收（仅一次，Circular模式硬件自动绕回） */
    if (HAL_UART_Receive_DMA(&REFEREE_UART,
                             (uint8_t *)g_ref_ins.Rx_CyclicBuf,
                             REFEREE_RX_BUF_SIZE) != HAL_OK)
    {
        /* DMA启动失败 — 可能被其他模块先占用了 */
        Error_Handler();
    }

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

        /* 仅置标志，通知任务处理新增数据（不碰DMA，Circular永不停机） */
        g_ref_ins.NewDataFlag = 1;
    }
}

/**********************************************************************
 * @brief  差值解析处理：计算环形缓冲区新增字节并喂给协议状态机
 * @note   通过__HAL_DMA_GET_COUNTER获取DMA写位置，与上次读取位置比较差值
 * @note   自动处理环形缓冲区绕回场景（分段memcpy）
 *********************************************************************/
static void Referee_ProcessDelta(void)
{
    /* 1. 计算DMA当前写位置（0 ~ REFEREE_RX_BUF_SIZE-1） */
    uint16_t dma_cnt = __HAL_DMA_GET_COUNTER(REFEREE_UART.hdmarx);
    uint16_t write_idx = REFEREE_RX_BUF_SIZE - dma_cnt;

    /* 2. 计算自上次处理以来的新字节数（处理环形绕回） */
    uint16_t new_bytes;
    if (write_idx >= g_ref_ins.Rx_LastReadIdx)
    {
        new_bytes = write_idx - g_ref_ins.Rx_LastReadIdx;
    }
    else
    {
        /* DMA写指针已绕回到缓冲区开头 */
        new_bytes = (REFEREE_RX_BUF_SIZE - g_ref_ins.Rx_LastReadIdx) + write_idx;
    }

    /* 安全检查：无新数据或数据异常则跳过 */
    if (new_bytes == 0 || new_bytes > REFEREE_RX_BUF_SIZE)
    {
        return;
    }

    /* 3. 将新增字节拷贝到解析缓存（处理环形绕回时的分段拷贝） */
    if (g_ref_ins.Rx_LastReadIdx + new_bytes <= REFEREE_RX_BUF_SIZE)
    {
        /* 无需绕回：连续的一段 */
        memcpy(g_ref_ins.Rx_ParseBuf,
               &g_ref_ins.Rx_CyclicBuf[g_ref_ins.Rx_LastReadIdx],
               new_bytes);
    }
    else
    {
        /* 需要绕回：先拷贝尾部，再拷贝头部 */
        uint16_t first_part = REFEREE_RX_BUF_SIZE - g_ref_ins.Rx_LastReadIdx;
        memcpy(g_ref_ins.Rx_ParseBuf,
               &g_ref_ins.Rx_CyclicBuf[g_ref_ins.Rx_LastReadIdx],
               first_part);
        memcpy(&g_ref_ins.Rx_ParseBuf[first_part],
               g_ref_ins.Rx_CyclicBuf,
               new_bytes - first_part);
    }

    /* 4. 更新读取位置和解析长度 */
    g_ref_ins.Rx_LastReadIdx = write_idx;
    g_ref_ins.Rx_ParseLen = new_bytes;

    /* 5. 逐字节喂给协议解析器 */
    for (uint16_t i = 0; i < new_bytes; i++)
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

        /* 检查IDLE中断标志，处理环形缓冲区新增数据 */
        if (g_ref_ins.NewDataFlag)
        {
            g_ref_ins.NewDataFlag = 0;
            Referee_ProcessDelta();
        }

        /* 无任何新数据直接延时，释放CPU */
        if (s_ref_data.update.all_flag == 0)
        {
            osDelay(1);
            continue;
        }

        osDelay(1);
    }
}

/**********************************************************************
 * @brief  获取裁判系统数据只读指针
 * @return const referee_all_data_t* 只读数据指针
 * @note   外部模块通过此函数访问裁判系统数据，禁止直接写数据
 *********************************************************************/
const referee_all_data_t* Referee_GetData(void)
{
    return &s_ref_data;
}

#endif