#ifndef PLAEYERUIEXTEND_H_
#define PLAEYERUIEXTEND_H_

#include "cmsis_os2.h"   // 核心，定义osMutexId_t等类型
#include "freertos.h"    // CubeMX自动生成的RTOS配置头

// #define SEND_MESSAGE(message, len) do{                \
//   osMutexAcquire(uart6_tx_mtx, osWaitForever);        \
//   HAL_UART_Transmit_DMA(&huart6, message, len);       \
//   while(HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX); \
//   osMutexRelease(uart6_tx_mtx);                       \
// }while(0)

// #define SEND_MESSAGE(message, len) do{ \
// HAL_UART_Transmit_DMA(&huart6, message, len); \
// /* 等待DMA传输完成 */ \
// while(HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX); \
// }while(0)

extern osMutexId_t uart6_tx_mtx;

extern void PlayerUIExtend_Init(void);

#endif // PLAEYERUIEXTEND_H_