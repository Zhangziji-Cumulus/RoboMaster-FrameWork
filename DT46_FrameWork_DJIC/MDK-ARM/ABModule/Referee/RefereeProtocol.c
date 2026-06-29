#include "RefereeProtocol.h"

#include <string.h>

#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/************************* CRC表 *************************/
const uint8_t CRC8_INIT = 0xFF;
const uint8_t CRC8_TAB[256] =
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

const uint16_t CRC16_INIT = 0xFFFF;
const uint16_t CRC16_TAB[256] =
{
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

/************************* 全局数据 *************************/
referee_all_data_t g_ref_data = {0};


/************************* CRC8函数 *************************/
uint8_t Get_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint8_t ucCRC8)
{
    uint8_t ucIndex;
    while (dwLength--)
    {
        ucIndex = ucCRC8 ^ (*pchMessage++);
        ucCRC8 = CRC8_TAB[ucIndex];
    }
    return ucCRC8;
}

uint32_t Verify_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint8_t ucExpected;
    if ((pchMessage == NULL) || (dwLength <= 2))
        return 0;
    ucExpected = Get_CRC8_Check_Sum(pchMessage, dwLength - 1, CRC8_INIT);
    return (ucExpected == pchMessage[dwLength - 1]);
}

void Append_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint8_t ucCRC;
    if ((pchMessage == NULL) || (dwLength <= 2))
        return;
    ucCRC = Get_CRC8_Check_Sum(pchMessage, dwLength - 1, CRC8_INIT);
    pchMessage[dwLength - 1] = ucCRC;
}

/************************* CRC16函数 *************************/
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC)
{
    uint8_t chData;
    if (pchMessage == NULL)
        return 0xFFFF;
    while (dwLength--)
    {
        chData = *pchMessage++;
        wCRC = ((uint16_t)(wCRC >> 8)) ^ CRC16_TAB[((uint16_t)(wCRC ^ (uint16_t)chData)) & 0x00ff];
    }
    return wCRC;
}

uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint16_t wExpected;
    if ((pchMessage == NULL) || (dwLength <= 2))
        return 0;
    wExpected = Get_CRC16_Check_Sum(pchMessage, dwLength - 2, CRC16_INIT);
    return (((wExpected & 0xff) == pchMessage[dwLength - 2]) && (((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]));
}

void Append_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint16_t wCRC;
    if ((pchMessage == NULL) || (dwLength <= 2))
        return;
    wCRC = Get_CRC16_Check_Sum(pchMessage, dwLength - 2, CRC16_INIT);
    pchMessage[dwLength - 2] = (uint8_t)(wCRC & 0x00ff);
    pchMessage[dwLength - 1] = (uint8_t)((wCRC >> 8) & 0x00ff);
}

/************************* 解析器初始化 *************************/
void Referee_Parser_Init(referee_parser_t *parser)
{
    if (parser == NULL)
        return;
    parser->state = WAIT_SOF;
    parser->data_cnt = 0;
    memset(&parser->frame_buf, 0, sizeof(referee_full_frame_t));
    memset(&g_ref_data, 0, sizeof(referee_all_data_t));
}

/************************* 单字节解析状态机 *************************/
void Referee_Parser_Byte(referee_parser_t *parser, uint8_t byte)
{
    if (parser == NULL)
        return;

    switch (parser->state)
    {
    case WAIT_SOF:
        if (byte == FRAME_SOF)
        {
            parser->frame_buf.header.sof = byte;
            parser->data_cnt = 1;
            parser->state = WAIT_HEADER_REST;
        }
        break;

    case WAIT_HEADER_REST:
        ((uint8_t *)&parser->frame_buf.header)[parser->data_cnt] = byte;
        parser->data_cnt++;

        if (parser->data_cnt >= FRAME_HEADER_LEN)
        {
            // 校验帧头CRC8
            if (Verify_CRC8_Check_Sum((uint8_t *)&parser->frame_buf.header, FRAME_HEADER_LEN))
            {
                parser->data_cnt = 0;
                parser->state = WAIT_DATA_AND_CRC16;
            }
            else
            {
                // CRC错误，重置状态机
                parser->state = WAIT_SOF;
                parser->data_cnt = 0;
            }
        }
        break;

    case WAIT_DATA_AND_CRC16:
    {
        uint16_t total_len = parser->frame_buf.header.data_len + FRAME_CMD_ID_LEN + FRAME_CRC16_LEN;
        ((uint8_t *)&parser->frame_buf.cmd_id)[parser->data_cnt] = byte;
        parser->data_cnt++;

        if (parser->data_cnt >= total_len)
        {
            // 整包接收完成，校验CRC16
            uint8_t *p_start = (uint8_t *)&parser->frame_buf.header;
            uint32_t pack_len = FRAME_HEADER_LEN + FRAME_CMD_ID_LEN + parser->frame_buf.header.data_len + FRAME_CRC16_LEN;

            if (Verify_CRC16_Check_Sum(p_start, pack_len))
            {
                // 校验通过，进入回调
                Referee_Frame_Callback(&parser->frame_buf);
            }
            // 重置状态机等待下一帧
            parser->state = WAIT_SOF;
            parser->data_cnt = 0;
        }
        break;
    }

    default:
        parser->state = WAIT_SOF;
        parser->data_cnt = 0;
        break;
    }
}

/************************* 帧回调弱定义 *************************/
__weak void Referee_Frame_Callback(referee_full_frame_t *frame)
{
    if (frame == NULL)
        return;

    referee_all_data_t *all = &g_ref_data;

    switch (frame->cmd_id)
    {
    /***************** 基础比赛数据 *****************/
    case CMD_GAME_STATUS:
        memcpy(&all->_game_status, frame->data, MIN(frame->header.data_len, sizeof(all->_game_status)));
        all->p_game_status = &all->_game_status;
        all->update.bit.game_status_upd = 1;
        break;

    case CMD_GAME_RESULT:
        memcpy(&all->_game_result, frame->data, MIN(frame->header.data_len, sizeof(all->_game_result)));
        all->p_game_result = &all->_game_result;
        all->update.bit.game_result_upd = 1;
        break;

    case CMD_ROBOT_HP_DATA:
        memset(&all->_ally_hp, 0, sizeof(all->_ally_hp));
        memcpy(&all->_ally_hp, frame->data, MIN(frame->header.data_len, sizeof(all->_ally_hp)));
        all->p_ally_hp = &all->_ally_hp;
        all->update.bit.ally_hp_upd = 1;
        break;

    case CMD_FIELD_EVENT:
        memcpy(&all->_field_event, frame->data, sizeof(all->_field_event));
        all->p_field_event = &all->_field_event;
        all->update.bit.field_event_upd = 1;
        break;

    case CMD_REF_WARNING:
        memcpy(&all->_ref_warning, frame->data, MIN(frame->header.data_len, sizeof(all->_ref_warning)));
        all->p_ref_warning = &all->_ref_warning;
        all->update.bit.ref_warn_upd = 1;
        break;

    case CMD_DART_INFO:
        memcpy(&all->_dart_info, frame->data, MIN(frame->header.data_len, sizeof(all->_dart_info)));
        all->p_dart_info = &all->_dart_info;
        all->update.bit.dart_info_upd = 1;
        break;

    /***************** 本机机器人数据 *****************/
    case CMD_ROBOT_STATUS:
        memset(&all->_robot_status, 0, sizeof(all->_robot_status));
        memcpy(&all->_robot_status, frame->data, MIN(frame->header.data_len, sizeof(all->_robot_status)));
        all->p_robot_status = &all->_robot_status;
        all->update.bit.robot_status_upd = 1;
        break;

    case CMD_POWER_HEAT_DATA:
        memcpy(&all->_power_heat, frame->data, MIN(frame->header.data_len, sizeof(all->_power_heat)));
        all->p_power_heat = &all->_power_heat;
        all->update.bit.power_heat_upd = 1;
        break;

    case CMD_ROBOT_POS:
        memcpy(&all->_robot_pos, frame->data, MIN(frame->header.data_len, sizeof(all->_robot_pos)));
        all->p_robot_pos = &all->_robot_pos;
        all->update.bit.robot_pos_upd = 1;
        break;

    case CMD_ROBOT_BUFF:
        memset(&all->_buff, 0, sizeof(all->_buff));
        memcpy(&all->_buff, frame->data, MIN(frame->header.data_len, sizeof(all->_buff)));
        all->p_buff = &all->_buff;
        all->update.bit.buff_upd = 1;
        break;

    case CMD_DAMAGE_DATA:
        memcpy(all->_damage, frame->data, frame->header.data_len);
        all->p_damage_buf = all->_damage;
        all->update.bit.damage_upd = 1;
        break;

    case CMD_SHOOT_DATA:
        memcpy(&all->_shoot_data, frame->data, MIN(frame->header.data_len, sizeof(all->_shoot_data)));
        all->p_shoot_data = &all->_shoot_data;
        all->update.bit.shoot_upd = 1;
        break;

    case CMD_PROJECTILE_ALLOW:
        memcpy(&all->_bullet_data, frame->data, MIN(frame->header.data_len, sizeof(all->_bullet_data)));
        all->p_bullet_data = &all->_bullet_data;
        all->update.bit.bullet_upd = 1;
        break;

    case CMD_RFID_STATUS:
        memcpy(all->_rfid, frame->data, frame->header.data_len);
        all->p_rfid_buf = all->_rfid;
        all->update.bit.rfid_upd = 1;
        break;

    case CMD_DART_CLIENT_CMD:
        memcpy(&all->_dart_client_cmd, frame->data, MIN(frame->header.data_len, sizeof(all->_dart_client_cmd)));
        all->p_dart_client_cmd = &all->_dart_client_cmd;
        all->update.bit.dart_client_upd = 1;
        break;

    case CMD_GROUND_ROBOT_POS:
        memcpy(&all->_ground_pos, frame->data, MIN(frame->header.data_len, sizeof(all->_ground_pos)));
        all->p_ally_ground_pos = &all->_ground_pos;
        all->update.bit.ally_ground_upd = 1;
        break;

    case CMD_RADAR_MARK_PROG:
        memcpy(&all->_radar_mark, frame->data, sizeof(uint16_t));
        all->p_radar_mark_bit = &all->_radar_mark;
        all->update.bit.radar_mark_upd = 1;
        break;

    case CMD_SENTRY_AUTO_INFO:
        memset(&all->_sentry_sync, 0, sizeof(all->_sentry_sync));
        memcpy(&all->_sentry_sync, frame->data, MIN(frame->header.data_len, sizeof(all->_sentry_sync)));
        all->p_sentry_sync = &all->_sentry_sync;
        all->update.bit.sentry_sync_upd = 1;
        break;

    case CMD_RADAR_AUTO_INFO:
        memcpy(&all->_radar_sync, frame->data, MIN(frame->header.data_len, sizeof(all->_radar_sync)));
        all->p_radar_sync = &all->_radar_sync;
        all->update.bit.radar_sync_upd = 1;
        break;

    /***************** 交互数据 *****************/
    case CMD_ROBOT_INTERACT:
    {
        memcpy(&all->_robot_inter, frame->data, MIN(frame->header.data_len, sizeof(all->_robot_inter)));
        all->p_robot_inter = &all->_robot_inter;
        all->update.bit.robot_inter_upd = 1;
        uint16_t sub_cmd = all->p_robot_inter->data_cmd_id;
        switch (sub_cmd)
        {
        case SUB_CMD_SENTRY_AUTO_CMD:
            memcpy(&all->_sentry_auto_cmd, all->p_robot_inter->user_data, sizeof(sentry_cmd_t));
            all->p_sentry_auto_cmd = &all->_sentry_auto_cmd;
            break;
        case SUB_CMD_RADAR_AUTO_CMD:
            memcpy(&all->_radar_cmd, all->p_robot_inter->user_data, sizeof(radar_cmd_t));
            all->p_radar_auto_cmd = &all->_radar_cmd;
            break;
        default:
            break;
        }
        break;
    }

    case CMD_MAP_CLICK_CMD:
        memcpy(&all->_map_click, frame->data, MIN(frame->header.data_len, sizeof(all->_map_click)));
        all->p_map_click = &all->_map_click;
        all->update.bit.map_click_upd = 1;
        break;

    case CMD_CUSTOM_CTRL_CLIENT:
        memcpy(&all->_mouse_key, frame->data, MIN(frame->header.data_len, sizeof(all->_mouse_key)));
        all->p_mouse_key = &all->_mouse_key;
        all->update.bit.mouse_key_upd = 1;
        break;

    case CMD_SENTRY_PATH_DATA:
        memcpy(&all->_sentry_path, frame->data, MIN(frame->header.data_len, sizeof(all->_sentry_path)));
        all->p_sentry_path = &all->_sentry_path;
        all->update.bit.sentry_path_upd = 1;
        break;

    default:
        break;
    }
}

/************************* 打包发送函数 *************************/
/**
 * @brief  通用帧打包函数
 * @param  cmd_id    命令码
 * @param  data      数据段指针
 * @param  data_len  数据段长度
 * @param  seq       包序号
 * @param  out_buf   输出打包完成的整包缓冲区
 * @return 整包总字节长度
 */
uint16_t Referee_Pack_Frame(uint16_t cmd_id, uint8_t *data, uint16_t data_len,
                            uint8_t seq, uint8_t *out_buf)
{
    if (out_buf == NULL || data_len > FRAME_MAX_DATA_LEN)
        return 0;

    uint16_t offset = 0;

    // 1. 填充帧头前4字节
    out_buf[offset++] = FRAME_SOF;
    out_buf[offset++] = (uint8_t)(data_len & 0xFF);
    out_buf[offset++] = (uint8_t)((data_len >> 8) & 0xFF);
    out_buf[offset++] = seq;

    // 2. 计算并填充帧头CRC8
    Append_CRC8_Check_Sum(out_buf, FRAME_HEADER_LEN);
    offset = FRAME_HEADER_LEN;

    // 3. 填充命令码
    out_buf[offset++] = (uint8_t)(cmd_id & 0xFF);
    out_buf[offset++] = (uint8_t)((cmd_id >> 8) & 0xFF);

    // 4. 填充数据段
    if (data != NULL && data_len > 0)
    {
        memcpy(&out_buf[offset], data, data_len);
        offset += data_len;
    }

    // 5. 计算并填充整包CRC16
    uint16_t total_len = offset + FRAME_CRC16_LEN;
    Append_CRC16_Check_Sum(out_buf, total_len);

    return total_len;
}

/**
 * @brief  打包机器人交互数据包(0x0301)
 */
uint16_t Referee_Pack_Interaction(uint16_t sub_cmd, uint16_t sender_id,
                                  uint16_t receiver_id, uint8_t *user_data,
                                  uint8_t data_len, uint8_t seq, uint8_t *out_buf)
{
    if (out_buf == NULL || data_len > 112)
        return 0;

    uint8_t data_buf[118];
    uint16_t offset = 0;

    // 子命令ID
    data_buf[offset++] = (uint8_t)(sub_cmd & 0xFF);
    data_buf[offset++] = (uint8_t)((sub_cmd >> 8) & 0xFF);
    // 发送者ID
    data_buf[offset++] = (uint8_t)(sender_id & 0xFF);
    data_buf[offset++] = (uint8_t)((sender_id >> 8) & 0xFF);
    // 接收者ID
    data_buf[offset++] = (uint8_t)(receiver_id & 0xFF);
    data_buf[offset++] = (uint8_t)((receiver_id >> 8) & 0xFF);
    // 用户数据
    if (user_data != NULL && data_len > 0)
    {
        memcpy(&data_buf[offset], user_data, data_len);
        offset += data_len;
    }

    return Referee_Pack_Frame(CMD_ROBOT_INTERACT, data_buf, offset, seq, out_buf);
}

/**
 * @brief  打包哨兵自主决策指令(0x0301子命令0x0120)
 */
uint16_t Referee_Pack_Sentry_Cmd(uint32_t cmd_val, uint16_t sender_id,
                                 uint8_t seq, uint8_t *out_buf)
{
    sentry_cmd_t cmd;
    cmd.sentry_cmd = cmd_val;
    return Referee_Pack_Interaction(SUB_CMD_SENTRY_AUTO_CMD, sender_id,
                                    CLIENT_JUDGE_SERVER, (uint8_t *)&cmd,
                                    sizeof(sentry_cmd_t), seq, out_buf);
}

/**
 * @brief  打包雷达自主决策指令(0x0301子命令0x0121)
 * @param  password 6字节密钥ASCII码数组
 */
uint16_t Referee_Pack_Radar_Cmd(uint8_t cmd_val, uint8_t *password,
                                uint16_t sender_id, uint8_t seq, uint8_t *out_buf)
{
    radar_cmd_t cmd;
    cmd.radar_cmd = cmd_val;
    cmd.password_cmd = 1; // 默认更新密钥
    if (password != NULL)
    {
        cmd.password_1 = password[0];
        cmd.password_2 = password[1];
        cmd.password_3 = password[2];
        cmd.password_4 = password[3];
        cmd.password_5 = password[4];
        cmd.password_6 = password[5];
    }
    return Referee_Pack_Interaction(SUB_CMD_RADAR_AUTO_CMD, sender_id,
                                    CLIENT_JUDGE_SERVER, (uint8_t *)&cmd,
                                    sizeof(radar_cmd_t), seq, out_buf);
}

/**
 * @brief  打包自定义控制器数据(0x0302)
 */
uint16_t Referee_Pack_Custom_Ctrl(uint8_t *data, uint8_t seq, uint8_t *out_buf)
{
    return Referee_Pack_Frame(CMD_CUSTOM_CTRL_ROBOT, data, 30, seq, out_buf);
}

/**
 * @brief  打包自定义控制器键鼠模拟数据(0x0306) 
 */
uint16_t Referee_Pack_Mouse_Key(custom_client_data_t *mk_data,
                                uint8_t seq, uint8_t *out_buf)
{
    return Referee_Pack_Frame(CMD_CUSTOM_CTRL_CLIENT, (uint8_t *)mk_data,
                              sizeof(custom_client_data_t), seq, out_buf);
}