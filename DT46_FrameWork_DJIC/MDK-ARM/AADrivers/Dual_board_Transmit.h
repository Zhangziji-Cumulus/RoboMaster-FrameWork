#ifndef __DUAL_BOARD_TRANSMIT_H__
#define __DUAL_BOARD_TRANSMIT_H__

#include "A_CommonSystem.h"

#if(BOARD_MODE == BOARD_MODE_DUAL)

//** #################################################################################################### **//
//** ============================================ 宏定义 ================================================ **//
//** #################################################################################################### **//


#define CHASSIS_BOARD   1
#define GIMBAL_BOARD    2

#define BOARD_ID    GIMBAL_BOARD

#define TX_BASE_ID ((BOARD_ID == 1) ? 0x100 : 0x200)  /* 发送基ID */
#define RX_BASE_ID ((BOARD_ID == 1) ? 0x200 : 0x100)  /* 接收基ID */

//** #################################################################################################### **//
//** ========================================== 枚举、结构体 ============================================= **//
//** #################################################################################################### **//



//** #################################################################################################### **//
//** ========================================= 对外函数声明 ============================================= **//
//** #################################################################################################### **//

/**
 * @brief  【示例】用户自定义结构体（根据实际需求修改）
 */
typedef struct {
    float pitch_angle;
    float yaw_angle;
    uint8_t fire_mode;
    uint8_t reserved[3];
} Example_Struct_t;

#define EXAMPLE_MSG_ID  0x0001  // 消息ID（扩展帧的高24位）


// ==================== 简化版双板通信接口 ====================

/**
 * @brief  【发送】发送结构体数据
 * @param  hcan CAN句柄
 * @param  extId 扩展帧ID（用于标识数据类型）
 * @param  pData 指向结构体的指针
 * @param  dataSize 结构体大小
 * @retval true=成功, false=失败
 */
bool DualBoard_SendStruct(
    CAN_HandleTypeDef *hcan,
    uint32_t extId,
    const void *pData,
    uint16_t dataSize
);

/**
 * @brief  【接收】解析CAN接收到的结构体数据（带ID过滤）
 * @param  pHeader CAN接收帧头
 * @param  RxData 接收到的8字节数据
 * @param  expectedId 期望的消息ID（用于过滤，必须与发送端一致）
 * @param  pOutStruct 输出结构体指针（自动填充）
 * @param  outStructSize 输出结构体大小
 * @retval true=成功解析并填充, false=不是目标数据或解析失败
 */
bool DualBoard_ParseStruct(
    CAN_RxHeaderTypeDef *pHeader,
    const uint8_t *RxData,
    uint32_t expectedId,
    void *pOutStruct,
    uint16_t outStructSize
);

#endif /* BOARD_MODE == BOARD_MODE_DUAL */

#endif /* __DUAL_BOARD_TRANSMIT_H__ */
