#ifndef REFEREE_CENTER_H
#define REFEREE_CENTER_H

#include "A_CommonSystem.h"
#include "stdint.h"
#include "stdbool.h"
#include "RefereeProtocol.h"

/************************ 硬件配置宏 ************************/
#define REFEREE_UART        huart6      // 裁判系统绑定串口
#define REFEREE_RX_BUF_SIZE 256U       // 单缓冲区大小，大于协议最大单包127字节

/************************ 对外接口函数 ************************/
void Referee_UART_IRQHandler(void);    // 串口空闲中断处理（放入对应串口中断中）


#endif // REFEREE_CENTER_H