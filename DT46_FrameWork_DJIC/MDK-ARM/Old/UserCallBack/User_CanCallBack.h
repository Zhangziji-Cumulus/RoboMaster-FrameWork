#ifndef __USER_CANCALLBACK_H
#define __USER_CANCALLBACK_H

#include "main.h"
#include "can.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"
#include "queue.h"

#include "Dual_board_Transmit.h"
#include "DJI_Motor_CAN.h"


/* 定义一个结构体，用来把 CAN 数据传给任务 */
typedef struct {
    uint32_t ExtId;
    uint8_t Data[8];
} CAN2_RxMsg_t;



#endif
