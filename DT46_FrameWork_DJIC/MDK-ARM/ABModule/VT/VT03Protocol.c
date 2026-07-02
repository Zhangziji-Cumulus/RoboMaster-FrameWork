#include "VT03Protocol.h"


/* ======================== CRC16 查表实现 ======================== */
static const uint16_t crc16_tab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/**
 * @brief 计算 CRC16-CCITT 校验值（查表法）
 */
static uint16_t get_crc16_check_sum(const uint8_t *p_msg, uint16_t len, uint16_t crc16)
{
    while (len--)
    {
        crc16 = (crc16 >> 8) ^ crc16_tab[(crc16 ^ *p_msg++) & 0xFF];
    }
    return crc16;
}

/**
 * @brief CRC16校验（严格匹配你提供的参考代码逻辑：小端序）
 */
bool verify_crc16_check_sum(uint8_t *p_msg, uint16_t len)
{
    if ((p_msg == NULL) || (len <= 2)) return false;

    uint16_t w_expected = get_crc16_check_sum(p_msg, len - 2, 0xFFFF);
    
    // 【核心修复】：匹配参考代码的小端序校验逻辑
    // p_msg[len-2] 是 CRC 低字节，p_msg[len-1] 是 CRC 高字节
    return ((w_expected & 0xFF) == p_msg[len - 2] && 
            ((w_expected >> 8) & 0xFF) == p_msg[len - 1]);
}

/**
 * @brief 检测 WASD / shift / ctrl / q e r f g z x c v b 按键，按下则蜂鸣器赋值 B_
 * @param buzzer 蜂鸣器对象指针
 * @param rc_data VT03解析数据只读指针
 */
uint8_t VT03_KeyTest(const VT03_Data_t *rc_data)
{
    // 数据无效直接返回，不触发蜂鸣
    if (rc_data->is_valid == 0)
        return 0;

    uint8_t key_trigger = 0;

    // 你指定的全部按键
    if (rc_data->keyboard.bit.w     ||
        rc_data->keyboard.bit.s     ||
        rc_data->keyboard.bit.a     ||
        rc_data->keyboard.bit.d     ||
        rc_data->keyboard.bit.shift ||
        rc_data->keyboard.bit.ctrl  ||
        rc_data->keyboard.bit.q     ||
        rc_data->keyboard.bit.e     ||
        rc_data->keyboard.bit.r     ||
        rc_data->keyboard.bit.f     ||
        rc_data->keyboard.bit.g     ||
        rc_data->keyboard.bit.z     ||
        rc_data->keyboard.bit.x     ||
        rc_data->keyboard.bit.c     ||
        rc_data->keyboard.bit.v     ||
        rc_data->keyboard.bit.b)
    {
        key_trigger = 1;
    }

    return key_trigger;
}

/**
 * @brief 检测鼠标是否有移动或任意按键按下
 * @param rc_data VT03解析数据只读指针
 * @return 1=鼠标有动作，0=鼠标无动作或数据无效
 */
uint8_t VT03_MouseTest(const VT03_Data_t *rc_data)
{
    // 数据无效直接返回
    if (rc_data->is_valid == 0)
        return 0;

    // 检测鼠标移动（x/y/z 任一非零）或鼠标按键按下（值为1）
    if (rc_data->mouse_x != 0 ||
        rc_data->mouse_y != 0 ||
        rc_data->mouse_z != 0 ||
        rc_data->mouse_left  == 1 ||
        rc_data->mouse_right == 1 ||
        rc_data->mouse_mid   == 1)
    {
        return 1;
    }

    return 0;
}

//==================== VT03 解析回调 ====================
uint8_t VT03_ParseCallback(const uint8_t *raw_frame, void *out_data)
{
    VT03_Data_t  *data = (VT03_Data_t  *)out_data;
    data->crc_ok = 0;
    // is_valid 由 VT_UpdateValidFlag 统一管理（含防抖），回调中不清零

    // 帧头校验
    if (raw_frame[0] != 0xA9 || raw_frame[1] != 0x53)
        return 0;
    // CRC校验
    if (!verify_crc16_check_sum((uint8_t *)raw_frame, 21))
        return 0;

    data->crc_ok = 1;
    // 下方复制你完整的位域、鼠标、通道解析代码
    uint64_t packed_data = 0;
    for (int i = 0; i < 8; i++)
    {
        packed_data |= ((uint64_t)raw_frame[2 + i]) << (i * 8);
    }

    data->ch0          = (packed_data >> 0)  & 0x7FF;
    data->ch1          = (packed_data >> 11) & 0x7FF;
    data->ch2          = (packed_data >> 22) & 0x7FF;
    data->ch3          = (packed_data >> 33) & 0x7FF;
    data->sw           = (packed_data >> 44) & 0x03;
    data->pause_btn    = (packed_data >> 46) & 0x01;
    data->custom_left  = (packed_data >> 47) & 0x01;
    data->custom_right = (packed_data >> 48) & 0x01;
    data->dial         = (packed_data >> 49) & 0x7FF;
    data->trigger      = (packed_data >> 60) & 0x01;

    data->mouse_x = (int16_t)((raw_frame[11] << 8) | raw_frame[10]);
    data->mouse_y = (int16_t)((raw_frame[13] << 8) | raw_frame[12]);
    data->mouse_z = (int16_t)((raw_frame[15] << 8) | raw_frame[14]);

    data->mouse_left  = raw_frame[16] & 0x01;
    data->mouse_right = (raw_frame[16] >> 2) & 0x01;
    data->mouse_mid   = (raw_frame[16] >> 4) & 0x01;

    data->keyboard.value = (raw_frame[18] << 8) | raw_frame[17];

    return 1;
}
