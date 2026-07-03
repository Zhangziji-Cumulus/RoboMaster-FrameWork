#include "RefereeTx_Bsp.h"
#include "CRC.h"
#include "usart.h"

/*==================== 全局变量 ====================*/
uint16_t g_robot_self_id = 0;
uint8_t g_is_sentry_radar = 0;
uint32_t g_send_byte_cnt = 0;
uint32_t g_send_window_tick = 0;
uint32_t g_last_send_tick = 0;
inter_fifo_t g_inter_queue;
static uint8_t g_frame_seq = 0;

/*==================== 队列操作 ====================*/
void InterQueue_Init(void)
{
    g_inter_queue.front = 0;
    g_inter_queue.rear = 0;
    memset(g_inter_queue.buf, 0, sizeof(g_inter_queue.buf));
}

uint8_t InterQueue_IsEmpty(void)
{
    return (g_inter_queue.front == g_inter_queue.rear);
}

uint8_t InterQueue_Push(inter_queue_node_t *node)
{
    uint8_t next = (g_inter_queue.rear + 1) % INTER_QUEUE_LEN;
    if (next == g_inter_queue.front)
        return 1; // full
    memcpy(&g_inter_queue.buf[g_inter_queue.rear], node, sizeof(inter_queue_node_t));
    g_inter_queue.rear = next;
    return 0;
}

uint8_t InterQueue_Pop(inter_queue_node_t *node)
{
    if (InterQueue_IsEmpty())
        return 1;
    memcpy(node, &g_inter_queue.buf[g_inter_queue.front], sizeof(inter_queue_node_t));
    g_inter_queue.front = (g_inter_queue.front + 1) % INTER_QUEUE_LEN;
    return 0;
}

/*==================== 带宽统计 ====================*/
void Inter_Bandwidth_Tick_Update(uint32_t tick_now)
{
    if ((tick_now - g_send_window_tick) >= 1000)
    {
        g_send_byte_cnt = 0;
        g_send_window_tick = tick_now;
    }
}

static uint8_t Inter_Check_Bandwidth(uint16_t pkt_len)
{
    uint32_t limit = g_is_sentry_radar ? BYTE_LIMIT_SENTRY_RADAR : BYTE_LIMIT_NORMAL;
    if ((g_send_byte_cnt + pkt_len) > limit)
        return 1; // over bandwidth
    return 0;
}

/*==================== 底层串口发送函数 ====================*/
void UART_Send_Data(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit_DMA(&huart6, data, len);
}

/*==================== 组包发送 ====================*/
uint8_t Referee_SendInterFrame(uint8_t *inter_data, uint16_t inter_len)
{
    if ((FRAME_FIXED_TOTAL + inter_len) > FRAME_MAX_TOTAL)
        return 1;
    uint8_t tx_buf[FRAME_MAX_TOTAL] = {0};
    uint16_t offset = 0;
    // 1. 帧头 5Byte
    tx_buf[offset++] = 0xA5; // SOF
    tx_buf[offset++] = inter_len & 0xFF;
    tx_buf[offset++] = (inter_len >> 8) & 0xFF;
    tx_buf[offset++] = g_frame_seq++;
    tx_buf[offset++] = Get_CRC8_Check_Sum(tx_buf, 4, CRC8_INIT);
    // 2. CMD_ID 0x0301
    tx_buf[offset++] = CMD_ID_ROBOT_INTER & 0xFF;
    tx_buf[offset++] = (CMD_ID_ROBOT_INTER >> 8) & 0xFF;
    // 3. 交互数据段
    memcpy(&tx_buf[offset], inter_data, inter_len);
    offset += inter_len;
    // 4. CRC16
    uint16_t crc16 = Get_CRC16_Check_Sum(tx_buf, offset, CRC16_INIT);
    tx_buf[offset++] = crc16 & 0xFF;
    tx_buf[offset++] = (crc16 >> 8) & 0xFF;
    // 带宽计数
    g_send_byte_cnt += offset;
    UART_Send_Data(tx_buf, offset);
    return 0;
}

/*==================== 机器人交互发送API ====================*/
uint8_t Robot_SendInterMsg(uint16_t sub_cmd_id, uint16_t recv_id, uint8_t *data, uint16_t len)
{
    // 长度校验
    if (len > INTER_USER_MAX_LEN)
        return 1;
    // 子ID合法
    if (sub_cmd_id < SUB_ID_ROBOT_MSG_MIN || sub_cmd_id > SUB_ID_ROBOT_MSG_MAX)
        return 3;
    inter_queue_node_t node = {0};
    node.sub_id = sub_cmd_id;
    node.recv_id = recv_id;
    node.data_len = INTER_HEAD_LEN + len;
    inter_head_t *head = (inter_head_t *)node.data_buf;
    head->sub_cmd_id = sub_cmd_id;
    head->sender_id = g_robot_self_id;
    head->receiver_id = recv_id;
    memcpy(node.data_buf + INTER_HEAD_LEN, data, len);
    if (InterQueue_Push(&node) != 0)
        return 2;
    return 0;
}

/*==================== 绘图内部转换工具 ====================*/
static void Figure_To_Proto(DrawFigure_t *src, interaction_figure_t *dst)
{
    memset(dst, 0, sizeof(interaction_figure_t));
    memcpy(dst->figure_name, src->name, 3);
    dst->operate_tpye = src->op_type;
    dst->figure_tpye = src->fig_type;
    dst->layer = src->layer;
    dst->color = src->color;
    dst->width = src->line_width;
    dst->start_x = (uint32_t)(src->x0);
    dst->start_y = (uint32_t)(src->y0);
    dst->details_a = (uint32_t)(src->param_a);
    dst->details_b = (uint32_t)(src->param_b);
    dst->details_c = (uint32_t)(src->x1);
    dst->details_d = (uint32_t)(src->y1);
}

/*==================== 绘图对外API ====================*/
void Draw_SingleFigure(DrawFigure_t *fig, uint16_t target_client_id)
{
    uint8_t send_buf[INTER_HEAD_LEN + 15] = {0};
    inter_head_t *head = (inter_head_t *)send_buf;
    head->sub_cmd_id = SUB_ID_DRAW_SINGLE;
    head->sender_id = g_robot_self_id;
    head->receiver_id = target_client_id;
    interaction_figure_t *proto_fig = (interaction_figure_t *)(send_buf + INTER_HEAD_LEN);
    Figure_To_Proto(fig, proto_fig);
    inter_queue_node_t node = {0};
    node.sub_id = SUB_ID_DRAW_SINGLE;
    node.recv_id = target_client_id;
    node.data_len = INTER_HEAD_LEN + 15;
    memcpy(node.data_buf, send_buf, node.data_len);
    InterQueue_Push(&node);
}

uint8_t Draw_MultiFigure(DrawFigure_t fig_list[], uint8_t fig_cnt, uint16_t target_client_id)
{
    uint16_t sub_id, data_len;
    if (fig_cnt <= 2)
    {
        sub_id = SUB_ID_DRAW_TWO;
        data_len = INTER_HEAD_LEN + 30;
    }
    else if (fig_cnt <=5)
    {
        sub_id = SUB_ID_DRAW_FIVE;
        data_len = INTER_HEAD_LEN + 75;
    }
    else if (fig_cnt <=7)
    {
        sub_id = SUB_ID_DRAW_SEVEN;
        data_len = INTER_HEAD_LEN + 105;
    }
    else
    {
        // 超过7个分多包
        uint8_t i;
        for(i=0;i<fig_cnt;i+=7)
        {
            uint8_t slice = (fig_cnt - i) >7 ?7:(fig_cnt-i);
            Draw_MultiFigure(&fig_list[i], slice, target_client_id);
        }
        return 0;
    }
    uint8_t send_buf[INTER_HEAD_LEN + 105] = {0};
    inter_head_t *head = (inter_head_t *)send_buf;
    head->sub_cmd_id = sub_id;
    head->sender_id = g_robot_self_id;
    head->receiver_id = target_client_id;
    uint8_t offset = INTER_HEAD_LEN;
    for(uint8_t i=0;i<fig_cnt;i++)
    {
        interaction_figure_t *f = (interaction_figure_t *)(send_buf + offset);
        Figure_To_Proto(&fig_list[i], f);
        offset += 15;
    }
    inter_queue_node_t node = {0};
    node.sub_id = sub_id;
    node.recv_id = target_client_id;
    node.data_len = data_len;
    memcpy(node.data_buf, send_buf, node.data_len);
    InterQueue_Push(&node);
    return 0;
}

void Draw_DeleteLayer(uint8_t del_type, uint8_t layer, uint16_t target_client_id)
{
    uint8_t send_buf[INTER_HEAD_LEN + 2] = {0};
    inter_head_t *head = (inter_head_t *)send_buf;
    head->sub_cmd_id = SUB_ID_DEL_LAYER;
    head->sender_id = g_robot_self_id;
    head->receiver_id = target_client_id;
    send_buf[INTER_HEAD_LEN] = del_type;
    send_buf[INTER_HEAD_LEN + 1] = layer;
    inter_queue_node_t node = {0};
    node.sub_id = SUB_ID_DEL_LAYER;
    node.recv_id = target_client_id;
    node.data_len = INTER_HEAD_LEN + 2;
    memcpy(node.data_buf, send_buf, node.data_len);
    InterQueue_Push(&node);
}

void Draw_Text(uint8_t layer, char *str, uint16_t x, uint16_t y, uint8_t font_size, uint16_t target_client_id)
{
    uint8_t send_buf[INTER_HEAD_LEN + 45] = {0};
    inter_head_t *head = (inter_head_t *)send_buf;
    head->sub_cmd_id = SUB_ID_DRAW_TEXT;
    head->sender_id = g_robot_self_id;
    head->receiver_id = target_client_id;
    // 简化文本填充，可根据协议完善字符结构体
    send_buf[INTER_HEAD_LEN] = font_size;
    send_buf[INTER_HEAD_LEN + 1] = x & 0xFF;
    send_buf[INTER_HEAD_LEN + 2] = (x>>8)&0xFF;
    send_buf[INTER_HEAD_LEN + 3] = y & 0xFF;
    send_buf[INTER_HEAD_LEN + 4] = (y>>8)&0xFF;
    uint16_t str_len = strlen(str);
    if(str_len >30) str_len =30;
    memcpy(send_buf + INTER_HEAD_LEN + 15, str, str_len);
    inter_queue_node_t node = {0};
    node.sub_id = SUB_ID_DRAW_TEXT;
    node.recv_id = target_client_id;
    node.data_len = INTER_HEAD_LEN + 45;
    memcpy(node.data_buf, send_buf, node.data_len);
    InterQueue_Push(&node);
}

/*==================== 调度主循环 33ms周期调用 ====================*/
void Inter_Schedule_Run(uint32_t tick_now)
{
    Inter_Bandwidth_Tick_Update(tick_now);
    // 频率限制
    if ((tick_now - g_last_send_tick) < INTER_SEND_MIN_DELAY_MS)
        return;
    if (InterQueue_IsEmpty())
        return;
    inter_queue_node_t node;
    if (InterQueue_Pop(&node) != 0)
        return;
    // 带宽判断
    if (Inter_Check_Bandwidth(node.data_len))
    {
        // 放回队列延后发送
        InterQueue_Push(&node);
        return;
    }
    Referee_SendInterFrame(node.data_buf, node.data_len);
    g_last_send_tick = tick_now;
}