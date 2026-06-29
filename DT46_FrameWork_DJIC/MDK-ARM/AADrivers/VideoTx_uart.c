#include "VideoTx_uart.h"

static VideoTx_Ctrl_t VideoTx_Ctrl;

/**
 * @brief 获取图传遥控器数据的指针
 * 
 * @return const VideoTx_Ctrl_t* 
 */
const VideoTx_Ctrl_t* get_VideoTx_Ctl_point(void)
{
    return &VideoTx_Ctrl;
}


// // CRC-16/CCITT-FALSE 多项式 0x1021，初始值 0xFFFF，无反转无XOR
// uint16_t RC_CRC16_Calc(const uint8_t *data, uint16_t len)
// {
//     uint16_t crc = 0xFFFF;
//     for (uint16_t i = 0; i < len; i++)
//     {
//         crc ^= (uint16_t)data[i] << 8;
//         for (uint8_t j = 0; j < 8; j++)
//         {
//             if (crc & 0x8000)
//                 crc = (crc << 1) ^ 0x1021;
//             else
//                 crc <<= 1;
//         }
//     }
//     return crc;
// }

// // 解析单帧21字节的遥控器数据
// void RC_ParseFrame(const uint8_t *raw_frame, VideoTx_Ctrl_t *rc_data)
// {
//     // 先清标志位
//     rc_data->is_valid = 0;

//     // 1. 帧头校验
//     if (raw_frame[0] != 0xA9 || raw_frame[1] != 0x53)
//         return;

//     // 2. CRC校验（校验前19字节，最后2字节是CRC值）
//     uint16_t calc_crc = RC_CRC16_Calc(raw_frame, RC_FRAME_SIZE - 2);
//     uint16_t recv_crc = (raw_frame[19] << 8) | raw_frame[20];
//     if (calc_crc != recv_crc)
//         return;

//     // CRC通过，标记数据有效
//     rc_data->is_valid = 1;

//     // --------------------------
//     // 3. 解析摇杆通道（11位无符号）
//     // --------------------------
//     // 通道0（偏移16位=第2字节，长度11位）
//     rc_data->ch0 = (raw_frame[2] | (raw_frame[3] << 8)) & 0x7FF;
//     // 通道1（偏移27位=第3字节第3位开始，长度11位）
//     rc_data->ch1 = ((raw_frame[3] >> 3) | (raw_frame[4] << 5) | (raw_frame[5] << 13)) & 0x7FF;
//     // 通道2（偏移38位=第4字节第6位开始，长度11位）
//     rc_data->ch2 = ((raw_frame[4] >> 6) | (raw_frame[5] << 2) | (raw_frame[6] << 10)) & 0x7FF;
//     // 通道3（偏移49位=第6字节第1位开始，长度11位）
//     rc_data->ch3 = ((raw_frame[6] >> 1) | (raw_frame[7] << 7)) & 0x7FF;

//     // --------------------------
//     // 4. 解析开关/按键
//     // --------------------------
//     uint8_t byte7 = raw_frame[7];
//     uint8_t byte8 = raw_frame[8];
//     uint8_t byte9 = raw_frame[9];

//     // 挡位开关（偏移60位=第7字节第4位，长度2位）
//     rc_data->sw = (byte7 >> 4) & 0x03;
//     // 暂停按键（偏移62位=第7字节第6位，长度1位）
//     rc_data->pause_btn = (byte7 >> 6) & 0x01;
//     // 自定义左键（偏移63位=第7字节第7位，长度1位）
//     rc_data->custom_left = (byte7 >> 7) & 0x01;
//     // 自定义右键（偏移64位=第8字节第0位，长度1位）
//     rc_data->custom_right = byte8 & 0x01;
//     // 拨轮（偏移65位=第8字节第1位，长度11位）
//     rc_data->dial = ((byte8 >> 1) | (byte9 << 7)) & 0x7FF;
//     // 扳机键（偏移76位=第9字节第4位，长度1位）
//     rc_data->trigger = (byte9 >> 4) & 0x01;

//     // --------------------------
//     // 5. 解析鼠标数据（16位有符号）
//     // --------------------------
//     rc_data->mouse_x = (int16_t)(raw_frame[10] | (raw_frame[11] << 8));
//     rc_data->mouse_y = (int16_t)(raw_frame[12] | (raw_frame[13] << 8));
//     rc_data->mouse_z = (int16_t)(raw_frame[14] | (raw_frame[15] << 8));

//     // 鼠标按键（偏移128/130/132位，各2位，取第0位即可）
//     rc_data->mouse_left = raw_frame[16] & 0x01;
//     rc_data->mouse_right = (raw_frame[16] >> 2) & 0x01;
//     rc_data->mouse_mid = (raw_frame[16] >> 4) & 0x01;

//     // --------------------------
//     // 6. 解析键盘按键（16位位图）
//     // --------------------------
//     rc_data->keyboard.value = raw_frame[17] | (raw_frame[18] << 8);
// }
