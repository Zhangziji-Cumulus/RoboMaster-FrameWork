#include "bsp_usart.h"

void BSP_UART_DMA_Init(BspUartDmaCtxTypeDef* ctx, UART_HandleTypeDef* huart)
{
    if (ctx == NULL || huart == NULL) return;
    memset(ctx, 0, sizeof(BspUartDmaCtxTypeDef));
    ctx->huart = huart;
    ctx->tx_len = 0;
    ctx->is_running = false;
}

void BSP_UART_DMA_Circular_Start(BspUartDmaCtxTypeDef* ctx, const void* p_init_data, uint16_t data_len)
{
    if (ctx == NULL || ctx->huart == NULL || p_init_data == NULL || ctx->huart->hdmatx == NULL) return;
    if (data_len > BSP_UART_DMA_TX_BUF_MAX_LEN) data_len = BSP_UART_DMA_TX_BUF_MAX_LEN;

    BSP_UART_DMA_Stop(ctx);
    ctx->tx_len = data_len;
    memcpy(ctx->tx_buf, p_init_data, data_len);

    // CubeMX预先配置为DMA Circular模式
    HAL_UART_Transmit_DMA(ctx->huart, ctx->tx_buf, ctx->tx_len);
    ctx->is_running = true;
}

void BSP_UART_DMA_Update(BspUartDmaCtxTypeDef* ctx, const void* p_new_data, uint16_t data_len)
{
    if (ctx == NULL || p_new_data == NULL || !ctx->is_running || data_len > BSP_UART_DMA_TX_BUF_MAX_LEN) return;
    if (data_len != ctx->tx_len) return; // 长度必须保持一致（DMA循环模式长度固定）
    memcpy(ctx->tx_buf, p_new_data, data_len);
}

void BSP_UART_DMA_Stop(BspUartDmaCtxTypeDef* ctx)
{
    if (ctx == NULL || ctx->huart == NULL) return;
    HAL_UART_DMAStop(ctx->huart);
    ctx->is_running = false;
}