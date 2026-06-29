#ifndef __USER_CALLBACK_H__
#define __USER_CALLBACK_H__

#include "A_CommonSystem.h"
#include "MotorCtrl_DJI.h"
#include "ZDT_CAN_Bsp.h"
#include "CMDCenter.h"
#include "bsp_CAN.h"
#include "AutoAim.h"

/* 定义一个结构体，用来把 CAN 数据传给任务 */
typedef struct {
    uint32_t ExtId;
    uint8_t Data[8];
} CAN2_RxMsg_t;



#endif /* __USER_CALLBACK_H__ */
