#include "Printf.h"
#include <stdarg.h>

#define PRINTF_USART  PRINTF_USART_HANDLE

/* 打印缓冲区 & DMA 忙标志 */
static char  printf_buf[PRINTF_BUFFER_SIZE];
static volatile uint8_t printf_tx_busy = 0;

/* ================================================================= */
/*  如果上一次DMA还没发完，本次数据直接丢弃（不阻塞调用者）           */
/* ================================================================= */
static void Printf_Send(const char *str, uint16_t len)
{
    if (printf_tx_busy)
        return;                     // 上一次还没发完，丢弃本次

    printf_tx_busy = 1;
    HAL_UART_Transmit_DMA(&PRINTF_USART, (uint8_t *)str, len);
}

/* ================================================================= */
/*  DMA 发送完成回调                                                 */
/* ================================================================= */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == PRINTF_USART.Instance)
    {
        printf_tx_busy = 0;
    }
}

/* ================================================================= */
/*  初始化                                                           */
/* ================================================================= */
void Printf_Init(void)
{
    printf_tx_busy = 0;
}

/* ================================================================= */
/*  格式化打印 —— 用法同 printf()                                     */
/*  示例：                                                           */
/*    Printf("Hello\r\n");                                          */
/*    Printf("yaw = %.2f, pitch = %.2f\r\n", yaw, pitch);           */
/*    Printf("speed = %d rpm\r\n", (int)speed_rpm);                  */
/* ================================================================= */
void Printf(const char *fmt, ...)
{
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsnprintf(printf_buf, sizeof(printf_buf), fmt, args);
    va_end(args);

    if (len > 0)
    {
        if (len >= PRINTF_BUFFER_SIZE)
            len = PRINTF_BUFFER_SIZE - 1;

        Printf_Send(printf_buf, (uint16_t)len);
    }
}