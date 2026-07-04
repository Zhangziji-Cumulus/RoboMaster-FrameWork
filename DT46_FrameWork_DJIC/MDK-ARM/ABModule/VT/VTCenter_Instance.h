#ifndef VTCENTER_INSTANCE_H_
#define VTCENTER_INSTANCE_H_

#include "VT03Protocol.h"
#include "A_CommonSystem.h"
#include <string.h>

#if(BOARD_ID == GIMBAL_BOARD)

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

#endif

#endif // VTCENTER_INSTANCE_H_