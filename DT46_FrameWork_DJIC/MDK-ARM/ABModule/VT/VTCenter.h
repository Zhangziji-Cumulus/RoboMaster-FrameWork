#ifndef VTCENTER_H_
#define VTCENTER_H_

#include "VT03Protocol.h"
#include "A_CommonSystem.h"
#include "Dual_Board_Transmit.h"
#include <string.h>


// 协议参数
#define RC_FRAME_SIZE        21U    // 单帧总长度
#define VT03_RX_BUF_SIZE     64U    // DMA循环缓冲区，大于2帧防止溢出
#define VT03_FRAME_INTERVAL  10U    // 帧解析扫描间隔ms
#define VT03_LOST_TIMEOUT    100U   // 100ms无新帧判定信号丢失
#define VT03_POWER_TIMEOUT   500U   // 500ms无帧判定设备断电
#define CRC_FAIL_MAX 3U         // 连续3帧CRC错误，判定数据失效


// 帧头尾（根据你实际协议修改）
#define VT03_FRAME_HEAD     0xAAU
#define VT03_FRAME_TAIL     0x55U


// VT03串口接收链路状态枚举
typedef enum
{
    VT03_STATE_NORMAL,        // 正常通信：持续收到校验合法的完整数据帧，通道数据可信
    VT03_STATE_SIGNAL_LOST,   // 信号丢失：短时未收到有效帧，判定为干扰/临时断连，设备未断电
    VT03_STATE_POWER_LOST,    // 设备断电失联：长时间无合法帧，判定VT03外设掉电、接线脱落
    VT03_STATE_RECONNECTING   // 重连中：上电初始化/手动复位后，等待首次有效帧恢复通信
} VT03_State_t;

// 全局对外接口
void VT03_Init(UART_HandleTypeDef *huart);
void VT03_Process(void);
const VideoTx_Ctrl_t *GetVT03DataPtr(void);
uint8_t VT03_IsDataValid(void);
uint8_t VT03_IsPowerLost(void);
void VT03_ResetReceiver(void);
void VT03_UART_IRQHandler(void);



#endif // VTCENTER_H_