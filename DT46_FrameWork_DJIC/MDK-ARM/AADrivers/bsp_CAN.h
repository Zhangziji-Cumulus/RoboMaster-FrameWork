#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "can.h"

/* Exported functions --------------------------------------------------------*/

/* 初始化 CAN 过滤器，接收所有 ID */
void can_filter_init(void);

/**
 * @brief 标准帧CAN发送函数
 * 
 * @param hcan CAN句柄指针
 * @param id   标准帧ID，11位有效
 * @param data 指向8字节数据缓冲区的指针
 */
void CAN_Send_STD(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t* data);

/**
 * @brief 一个发送浮点数数组的CAN发送函数，使用扩展帧，并在ID中携带帧序号
 * 
 * @param hcan CAN句柄指针
 * @param data 浮点数据数组指针
 * @param length 浮点数据个数，最大64位
 * @param ID    最大64位ID
 * @return bool发送是否成功
 */
bool CAN_SendFloatArray(CAN_HandleTypeDef* hcan, float* data, uint8_t length, uint16_t ID);

#endif /* __BSP_CAN_H__ */
