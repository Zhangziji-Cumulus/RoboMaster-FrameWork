#ifndef __HOTRC_HT10A_H
#define __HOTRC_HT10A_H

#include "stm32f4xx_hal.h" // 根据你的芯片型号更改，比如 stm32f1xx_hal.h

#include "main.h"
#include "usart.h"
#include "string.h"
#include <stdbool.h>
#include "main.h"
#include <stdint.h>

#define HOTRC_MID_VEL 992.0f //接受到的数据的正中值
#define HOTRC_RANGE 600

#define HOTRC_SWITCH_UP    1
#define HOTRC_SWITCH_MID   2
#define HOTRC_SWITCH_DOWN  3

typedef struct{
	
	//摇杆
	struct{
		short LX;// 左摇杆 X 轴
		short LY;// 左摇杆 Y 轴
		short RX;// 右摇杆 X 轴
		short RY;// 右摇杆 Y 轴
	}Stick;
	
	struct{
		short S2_L;// 两段拨杆1
		short S2_R;// 两段拨杆2
		short S3_L;// 三段拨杆1
		short S3_R;// 三段拨杆2
	}Switch;
	
	struct{
		short KL;
		short KR;
	}Knob;

	struct{
    uint8_t ch17;                        // 数字通道1 (0或1)
    uint8_t ch18;                        // 数字通道2 (0或1)
    uint8_t frameLost;                   // 丢帧标志 (1表示丢帧)
    uint8_t failsafe;                    // 故障安全标志 (1表示触发)
	}Flag;

}HOTRC_Ctl_t;

// ================= 配置宏定义 =================
#define SBUS_FRAME_LEN         25      // SBUS 标准帧长：25字节
#define SBUS_RX_BUF_SIZE       50      // DMA 接收缓冲区大小 (建议是帧长的2倍，防止溢出)
#define SBUS_HEADER            0x0F    // SBUS 帧头
#define SBUS_FOOTER            0x00    // SBUS 帧尾

// ================= 数据结构体 =================
typedef struct {
    // 16个标准通道 (值范围: 172 - 1811, 中立值约 1024)
    uint16_t channels[16];
    
    // 2个数字通道 (0或1)
    uint8_t ch17;
    uint8_t ch18;
    
    // 状态标志
    uint8_t frameLost;  // 丢帧标志
    uint8_t failsafe;   // 失控保护标志
    
    // 内部逻辑变量
    uint8_t  newDataFlag;   // 中断级：有新数据到达标志
    uint32_t lastParseTick; // 应用级：上次解析时间戳 (用于超时轮询)
} SbusData_t;

// ================= 全局变量声明 =================
extern SbusData_t sbusData;

// ================= 函数声明 =================
void SBUS_Init(UART_HandleTypeDef *huart);
void SBUS_Process(void); // 这个函数必须在 main 循环中高频调用
uint8_t SBUS_IsRemoteLost(void);//检测遥控器掉线
	
#endif /* __SBUS_H */
