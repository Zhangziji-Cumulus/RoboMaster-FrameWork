#ifndef PLAYERUI_H_
#define PLAYERUI_H_

#include "A_CommonSystem.h"
#include "ui.h"
#include "PlayerUIExtend.h"

// #define SEND_MESSAGE(message, len) do{                \
//   osMutexAcquire(uart6_tx_mtx, osWaitForever);        \
//   HAL_UART_Transmit_DMA(&huart6, message, len);       \
//   while(HAL_UART_GetState(&huart6) == HAL_UART_STATE_BUSY_TX); \
//   osMutexRelease(uart6_tx_mtx);                       \
// }while(0)

#endif // PLAYERUI_H_