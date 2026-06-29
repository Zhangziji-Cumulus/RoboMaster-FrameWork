#ifndef __BSP_SBUS_H__
#define __BSP_SBUS_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "A_CommonSystem.h"

/* Exported constants --------------------------------------------------------*/
#define SBUS_FRAME_LEN         25      /**< SBUS 标准帧长度 */
#define SBUS_RX_BUF_SIZE       50      /**< DMA 接收缓冲区大小，建议为帧长的2倍 */
#define SBUS_HEADER            0x0F    /**< SBUS 帧头 */
#define SBUS_FOOTER            0x00    /**< SBUS 帧尾 */

/* SBUS 超时和安全配置 */
#define SBUS_FRAME_INTERVAL     10      /**< SBUS 标准帧率 100Hz，每10ms一帧 */
#define SBUS_LOST_TIMEOUT       100     /**< 100ms未收到数据判定为信号丢失 */
#define SBUS_POWER_LOST_TIMEOUT 500     /**< 500ms未收到数据判定为接收端断电 */
#define SBUS_RECONNECT_DELAY    200     /**< 重连后稳定等待时间(ms) */

#define SBUS_CHANNEL_MIN       172     /**< 通道最小值 */
#define SBUS_CHANNEL_MID       1024    /**< 通道中立值 */
#define SBUS_CHANNEL_MAX       1811    /**< 通道最大值 */

/* SBUS 状态枚举 */
typedef enum {
    SBUS_STATE_NORMAL = 0,     /* 正常通信 */
    SBUS_STATE_SIGNAL_LOST,    /* 信号丢失（遥控器关机或超出范围） */
    SBUS_STATE_POWER_LOST,     /* 接收端断电 */
    SBUS_STATE_RECONNECTING    /* 正在重连恢复 */
} SbusState_e;

/* Exported types ------------------------------------------------------------*/ 
typedef struct {
    uint16_t channels[16];        /**< 16个标准通道，值范围 172-1811，中立值约 1024 */
    uint8_t ch17;                 /**< 数字通道 17 */
    uint8_t ch18;                 /**< 数字通道 18 */
    uint8_t frameLost;            /**< 丢帧标志（来自SBUS协议） */
    uint8_t failsafe;             /**< 失控保护标志（来自SBUS协议） */
    uint8_t newDataFlag;          /**< 中断级新数据到达标志 */
    uint8_t is_valid;             /**< 数据有效标志（应用层主要使用这个） */
    uint32_t lastParseTick;       /**< 上次解析时间戳 */
    uint32_t lastValidTick;       /**< 上次收到有效数据时间戳 */
    SbusState_e state;            /**< 当前SBUS状态 */
    uint32_t reconnectTick;       /**< 重连开始时间戳 */
} SbusData_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief 初始化 SBUS 接收
 * @param huart UART 句柄
 */
void SBUS_Init(UART_HandleTypeDef *huart);

/**
 * @brief 获取遥控器数据指针（只读）
 * @return const SbusData_t* 指向当前遥控器数据的指针
 */
const SbusData_t *get_SBUS_Data_point(void);

/**
 * @brief 处理 SBUS 数据，需在主循环中高频调用
 */
void SBUS_Process(void);

/**
 * @brief 检查遥控器数据是否有效
 * @return 1 表示数据有效，0 表示无效（任何异常情况）
 */
uint8_t SBUS_IsDataValid(void);

/**
 * @brief 检查SBUS接收端是否断电
 * @return 1 表示断电，0 表示正常
 */
uint8_t SBUS_IsPowerLost(void);

/**
 * @brief 强制重置SBUS接收
 */
void SBUS_ResetReceiver(void);

#endif /* __BSP_SBUS_H__ */

