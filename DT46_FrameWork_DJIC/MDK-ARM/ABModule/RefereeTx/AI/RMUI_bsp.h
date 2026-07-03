// #ifndef __RMUI_BSP_H__
// #define __RMUI_BSP_H__

// #include <stdint.h>
// #include <string.h>

// #define FRAME_HEADER_LEN        5U
// #define CMD_ID_LEN              2U
// #define CRC16_TAIL_LEN          2U
// #define FRAME_FIXED_TOTAL       (FRAME_HEADER_LEN + CMD_ID_LEN + CRC16_TAIL_LEN)
// #define INTER_HEAD_LEN          6U
// #define INTER_USER_MAX_LEN      112U
// #define FRAME_MAX_TOTAL         127U

// #define CMD_ID_ROBOT_INTER      0x0301

// #define SUB_ID_DEL_LAYER        0x0100
// #define SUB_ID_DRAW_SINGLE      0x0101
// #define SUB_ID_DRAW_TWO         0x0102
// #define SUB_ID_DRAW_FIVE        0x0103
// #define SUB_ID_DRAW_SEVEN       0x0104
// #define SUB_ID_DRAW_TEXT        0x0110

// #define SUB_ID_ROBOT_MSG_MIN    0x0200
// #define SUB_ID_ROBOT_MSG_MAX    0x02FF

// #define INTER_SEND_MIN_DELAY_MS 33U

// #define BYTE_LIMIT_NORMAL       3720U
// #define BYTE_LIMIT_SENTRY_RADAR 5120U

// typedef enum
// {
//     COLOR_SELF = 0,
//     COLOR_YELLOW,
//     COLOR_GREEN,
//     COLOR_ORANGE,
//     COLOR_MAGENTA,
//     COLOR_PINK,
//     COLOR_CYAN,
//     COLOR_BLACK,
//     COLOR_WHITE
// } DrawColor_e;

// typedef enum
// {
//     FIG_LINE = 0,
//     FIG_RECT,
//     FIG_CIRCLE,
//     FIG_ELLIPSE,
//     FIG_ARC,
//     FIG_FLOAT_NUM,
//     FIG_INT_NUM,
//     FIG_CHAR
// } DrawType_e;

// typedef enum
// {
//     OP_NULL = 0,
//     OP_ADD,
//     OP_MODIFY,
//     OP_DELETE
// } DrawOp_e;

// typedef struct
// {
//     uint8_t name[3];
//     uint8_t op_type;
//     uint8_t fig_type;
//     uint8_t layer;
//     uint8_t color;
//     uint16_t line_width;
//     float x0, y0;
//     float x1, y1;
//     float param_a, param_b;
// } DrawFigure_t;

// typedef __packed struct
// {
//     uint8_t figure_name[3];
//     uint32_t operate_tpye : 3;
//     uint32_t figure_tpye : 3;
//     uint32_t layer : 4;
//     uint32_t color : 4;
//     uint32_t details_a : 9;
//     uint32_t details_b : 9;
//     uint32_t width : 10;
//     uint32_t start_x : 11;
//     uint32_t start_y : 11;
//     uint32_t details_c : 10;
//     uint32_t details_d : 11;
//     uint32_t details_e : 11;
// } interaction_figure_t;

// typedef __packed struct
// {
//     uint16_t sub_cmd_id;
//     uint16_t sender_id;
//     uint16_t receiver_id;
// } inter_head_t;

// typedef struct
// {
//     uint16_t sub_id;
//     uint16_t recv_id;
//     uint16_t data_len;
//     uint8_t data_buf[INTER_USER_MAX_LEN];
// } inter_queue_node_t;

// #define INTER_QUEUE_LEN 16U

// typedef struct
// {
//     inter_queue_node_t buf[INTER_QUEUE_LEN];
//     uint8_t front;
//     uint8_t rear;
// } inter_fifo_t;

// extern uint16_t g_robot_self_id;
// extern uint8_t g_is_sentry_radar;
// extern uint32_t g_send_byte_cnt;
// extern uint32_t g_send_window_tick;
// extern uint32_t g_last_send_tick;
// extern inter_fifo_t g_inter_queue;
// extern volatile uint8_t g_tx_busy;

// void InterQueue_Init(void);
// uint8_t InterQueue_Push(inter_queue_node_t *node);
// uint8_t InterQueue_Pop(inter_queue_node_t *node);
// uint8_t InterQueue_IsEmpty(void);

// uint8_t Referee_SendInterFrame(uint8_t *inter_data, uint16_t inter_len);
// void Inter_Bandwidth_Tick_Update(uint32_t tick_now);

// void Draw_SingleFigure(DrawFigure_t *fig, uint16_t target_client_id);
// uint8_t Draw_MultiFigure(DrawFigure_t fig_list[], uint8_t fig_cnt, uint16_t target_client_id);
// void Draw_DeleteLayer(uint8_t del_type, uint8_t layer, uint16_t target_client_id);
// void Draw_Text(uint8_t layer, char *str, uint16_t x, uint16_t y, uint8_t font_size, uint16_t target_client_id);

// void Inter_Schedule_Run(uint32_t tick_now);

// #endif
