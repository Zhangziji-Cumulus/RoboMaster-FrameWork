#ifndef PRINTF_H_
#define PRINTF_H_

#include "A_CommonSystem.h"

/*
 * 使用说明：
 *   1. 在 FreeRTOSConfig.h 中确认 configENABLE_FPU = 1（否则 printf 浮点格式化会卡死）
 *   2. 在 CubeMX 中为对应串口使能 DMA TX（普通模式即可）
 *   3. 在需要打印的地方 #include "Printf.h"，然后调用 Printf()
 */

/* ========== 用户配置 ========== */
#define PRINTF_USART_HANDLE     huart1       // 打印用的串口句柄（可选 huart1 / huart3 / huart6）
#define PRINTF_BUFFER_SIZE      256          // 单次最大打印长度（字节）

/* ========== API ========== */
void Printf_Init(void);
void Printf(const char *fmt, ...);

#endif // PRINTF_H_
