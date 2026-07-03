#include "RefereeProtocol.h"

#include <string.h>

#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/************************* 全局数据 *************************/
referee_all_data_t g_ref_data = {0};


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