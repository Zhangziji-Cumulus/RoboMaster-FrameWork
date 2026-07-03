//
// Created by bismarckkk on 2024/2/17.
//

#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include <stdio.h>
#include "ui_types.h"

#include "cmsis_os2.h"   // 核心，定义osMutexId_t等类型
#include "freertos.h"    // CubeMX自动生成的RTOS配置头
#include "usart.h"

extern int ui_self_id;

void print_message(const uint8_t* message, int length);

// User Code Begin
#define SEND_MESSAGE(message, len) HAL_UART_Transmit_DMA(&huart6, message, len); osDelay(10);
// User Code End

void ui_proc_1_frame(ui_1_frame_t *msg);
void ui_proc_2_frame(ui_2_frame_t *msg);
void ui_proc_5_frame(ui_5_frame_t *msg);
void ui_proc_7_frame(ui_7_frame_t *msg);
void ui_proc_string_frame(ui_string_frame_t *msg);

#endif //UI_INTERFACE_H
