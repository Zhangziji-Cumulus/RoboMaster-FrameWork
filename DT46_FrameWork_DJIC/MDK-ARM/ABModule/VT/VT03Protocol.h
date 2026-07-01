#ifndef __VT03_H__
#define __VT03_H__

#include "main.h"

#include <stdint.h>
#include <stdbool.h>

// 串口接收缓冲区大小（单帧21字节，定义32字节避免溢出）
#define RC_FRAME_SIZE 21

//宏定义接收到的数据范围

//遥感通道最小值、最大值、中间值
#define VTX_CHANNEL_MIN 364
#define VTX_CHANNEL_MID 1024
#define VTX_CHANNEL_MAX 1684

//鼠标移动速度最小值、最大值、未移动为0
#define VTX_MOUSE_VELMIN -32768
#define VTX_MOUSE_STOP 0
#define VTX_MOUSE_MAX 32767

/*
 * @brief 图传遥控器原始帧打包结构体（直接映射21字节接收缓冲）
 * @note  __packed 确保无填充对齐，位域顺序匹配小端协议
 *        帧结构：帧头(2B) + 通道/按键(8B) + 鼠标(6B) + 鼠标按键(1B) + 键盘(2B) + CRC16(2B) = 21B
 */
typedef __packed struct
{
    /* 帧头 */
    uint8_t sof_1;         // 0xA9
    uint8_t sof_2;         // 0x53

    /* 摇杆 + 开关 + 拨轮（紧凑位域，共61bit，填满8字节） */
    uint64_t ch_0     : 11;
    uint64_t ch_1     : 11;
    uint64_t ch_2     : 11;
    uint64_t ch_3     : 11;
    uint64_t mode_sw  :  2;
    uint64_t pause    :  1;
    uint64_t fn_1     :  1;
    uint64_t fn_2     :  1;
    uint64_t wheel    : 11;
    uint64_t trigger  :  1;

    /* 鼠标位移 */
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;

    /* 鼠标按键（各2bit，低有效位 = 按键状态） */
    uint8_t mouse_left   : 2;
    uint8_t mouse_right  : 2;
    uint8_t mouse_middle : 2;

    /* 键盘位图 + CRC */
    uint16_t key;
    uint16_t crc16;

} remote_data_t;

// 遥控器解析后的完整数据
typedef struct
{
    // 摇杆通道（原始值：364~1684，中间值1024）
    uint16_t ch0;   // 右摇杆水平
    uint16_t ch1;   // 右摇杆竖直
    uint16_t ch2;   // 左摇杆竖直
    uint16_t ch3;   // 左摇杆水平
    uint16_t dial;  // 拨轮
    
    // 开关/按键
    uint8_t sw;         // 挡位开关 0:C,1:N,2:S
    uint8_t pause_btn;  // 暂停键 未按下为0，按下为1
    uint8_t custom_left;// 自定义左键 未按下为0，按下为1
    uint8_t custom_right;// 自定义右键 未按下为0，按下为1
    uint8_t trigger;    // 扳机键，未按下为0，按下为1
    
    // 鼠标数据（有符号16位）
    //最小值-32768，最大值32767，未移动为0
    int16_t mouse_x;//鼠标左右移动的速度，负为左移动
    int16_t mouse_y;//鼠标前后移动的速度，负为向后移动
    int16_t mouse_z;//鼠标滚轮的滚动速度，负值为向后滚动
    
    // 鼠标按键，最小值0，最大值1，未按下为1，按下为0
    uint8_t mouse_left;
    uint8_t mouse_right;
    uint8_t mouse_mid;
    
    // 键盘按键（16位位图）
    //最小值0，最大值65545，每个位代表一个键是否按下，1表示按下，0表示未按下
    union
    {
        uint16_t value;
        struct
        {
            uint16_t w      : 1;
            uint16_t s      : 1;
            uint16_t a      : 1;
            uint16_t d      : 1;
            uint16_t shift  : 1;
            uint16_t ctrl   : 1;
            uint16_t q      : 1;
            uint16_t e      : 1;
            uint16_t r      : 1;
            uint16_t f      : 1;
            uint16_t g      : 1;
            uint16_t z      : 1;
            uint16_t x      : 1;
            uint16_t c      : 1;
            uint16_t v      : 1;
            uint16_t b      : 1;
        } bit;
    } keyboard;

    uint8_t is_valid;  // 数据是否有效（CRC校验通过）

} VideoTx_Ctrl_t;

void VT_ParseFrame(const uint8_t *raw_frame, VideoTx_Ctrl_t *rc_data);

bool verify_crc16_check_sum(uint8_t *p_msg, uint16_t len);

const VideoTx_Ctrl_t* get_VideoTx_Ctl_point();

#endif /* __VT03_H__ */
