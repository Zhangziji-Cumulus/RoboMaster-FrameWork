#ifndef VTCENTER_H_
#define VTCENTER_H_

#include "VT03Protocol.h"
#include "A_CommonSystem.h"
#include "Dual_Board_Transmit.h"
#include "sound_effects_task.h"
#include <string.h>

#if(0)
#if(BOARD_ID == GIMBAL_BOARD)
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

#endif
#endif

//最大示例数
#define VT_MAX_INST 2

//==================== 通信状态枚举 ====================
typedef enum
{
    VT_STATE_NORMAL,
    VT_STATE_SIGNAL_LOST,
    VT_STATE_POWER_LOST,
    VT_STATE_RECONNECTING
} VT_State_t;

//==================== 图传通信配置（帧长、超时、帧头等） ====================
typedef struct
{
    uint16_t frame_len;          // 单帧总字节长度
    uint16_t rx_buf_len;         // DMA循环缓存大小
    uint8_t  head0;              // 帧头字节1
    uint8_t  head1;              // 帧头字节2

    uint16_t scan_interval_ms;   // 缓冲区定时扫描间隔
    uint16_t signal_lost_ms;     // 信号丢失超时
    uint16_t power_lost_ms;      // 断电失联超时
    uint8_t  crc_fail_max;       // CRC连续失败判定阈值

    uint8_t  crc_ok_offset;      // crc_ok字段在user_data结构体中的字节偏移
    uint8_t  is_valid_offset;    // is_valid字段在user_data结构体中的字节偏移
} VT_Config_t;

//==================== 解析函数指针：多协议多态核心 ====================
// 入参：原始帧缓存 / 输出数据结构体通用void*
// 返回：1=校验解析成功  0=帧头/CRC失败
typedef uint8_t (*VT_ParseCallback)(const uint8_t *raw_frame, void *out_data);

//==================== VT通用实例（一套底层驱动挂载任意图传） ====================
typedef struct
{
    UART_HandleTypeDef *huart;
    VT_Config_t cfg;
    VT_ParseCallback parse_cb;  // 绑定当前型号专属解析函数

    uint8_t *rx_cyclic_buf;     // DMA循环接收缓存
    void    *user_data;         // 指向对应型号的数据结构体
    VT_State_t state;

    uint8_t crc_fail_cnt;
    uint8_t new_data_flag;

    uint32_t last_valid_tick;
    uint32_t last_parse_tick;
    uint32_t reconnect_tick;

    uint16_t scan_offset;

} VT_Instance_t;

//==================== 对外API ====================
/**
 * @brief 初始化图传实例
 * @param inst VT实例
 * @param huart 串口句柄
 * @param cfg 通信参数配置
 * @param parse_cb 协议解析回调
 * @param rx_buf DMA接收缓存数组
 * @param data_buf 型号专属数据结构体地址
 */
void VT_Init(VT_Instance_t *inst,
             UART_HandleTypeDef *huart,
             const VT_Config_t *cfg,
             VT_ParseCallback parse_cb,
             uint8_t *rx_buf,
             void *data_buf);

// 给每个串口绑定实例，中断通过huart查找对应实例
//注册
void VT_RegisterInstance(UART_HandleTypeDef *huart, VT_Instance_t *inst);
//获取
VT_Instance_t *VT_GetInstanceByUart(UART_HandleTypeDef *huart);

// 周期处理：放在RTOS任务循环
void VT_Process(VT_Instance_t *inst);

// 手动重置串口DMA接收链路
void VT_ResetReceiver(VT_Instance_t *inst);

// 串口中断IDLE处理，放入USARTx_IRQHandler
void VT_UART_IRQHandler(VT_Instance_t *inst);

// 读取数据安全宏
#define VT_GET_DATA(type, inst) ((type *)(inst)->user_data)

// 状态判断接口
uint8_t VT_IsDataValid(const VT_Instance_t *inst);
uint8_t VT_IsPowerLost(const VT_Instance_t *inst);

#endif // VTCENTER_H_