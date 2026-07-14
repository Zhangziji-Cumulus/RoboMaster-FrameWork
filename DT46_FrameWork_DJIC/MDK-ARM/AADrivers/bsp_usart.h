#ifndef __BSP_USART_DMA_H
#define __BSP_USART_DMA_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "usart.h"

// 单路串口DMA上下文：底层用通用字节缓冲区，不写死结构体类型
#define BSP_UART_DMA_TX_BUF_MAX_LEN 64U  // 设置足够容纳最大外部结构体

typedef struct
{
    UART_HandleTypeDef* huart;
    uint8_t tx_buf[BSP_UART_DMA_TX_BUF_MAX_LEN]; // 通用字节缓存
    uint16_t tx_len; // 当前传输长度
    bool is_running;
} BspUartDmaCtxTypeDef;

/**
 * @brief 初始化串口DMA上下文
 */
void BSP_UART_DMA_Init(BspUartDmaCtxTypeDef* ctx, UART_HandleTypeDef* huart);

/**
 * @brief 启动DMA循环发送指定长度数据
 * @param ctx: 串口DMA上下文
 * @param p_init_data: 外部任意结构体/数据指针
 * @param data_len: 数据字节长度 sizeof(你的结构体)
 */
void BSP_UART_DMA_Circular_Start(BspUartDmaCtxTypeDef* ctx, const void* p_init_data, uint16_t data_len);

/**
 * @brief 更新DMA缓冲区数据（外部传入任意结构体）
 * @param ctx: 串口DMA上下文
 * @param p_new_data: 外部任意结构体指针
 * @param data_len: 数据字节长度 sizeof(你的结构体)
 */
void BSP_UART_DMA_Update(BspUartDmaCtxTypeDef* ctx, const void* p_new_data, uint16_t data_len);

/**
 * @brief 停止DMA发送
 */
void BSP_UART_DMA_Stop(BspUartDmaCtxTypeDef* ctx);

#endif