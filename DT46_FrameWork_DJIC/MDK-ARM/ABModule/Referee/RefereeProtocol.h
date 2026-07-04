/*
 *   基于机甲大师高校系列赛通信协议V2.0.0（20260626）编写
 *
 *
 */

#ifndef REFEREEPROTOCOL_H_
#define REFEREEPROTOCOL_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************* 硬件配置常量 *************************/
#define REFEREE_UART_BAUD_NORMAL    115200U   // 常规链路波特率
#define REFEREE_UART_BAUD_AIR       921600U   // 图传链路波特率
#define UART_DATA_BIT                8U
#define UART_STOP_BIT                1U
#define UART_PARITY_NONE             0U

/************************* 帧基础常量 *************************/
#define FRAME_SOF                    0xA5U     // 帧起始字节
#define FRAME_HEADER_LEN             5U        // 帧头总长度
#define FRAME_CMD_ID_LEN             2U        // 命令码长度
#define FRAME_CRC16_LEN              2U        // CRC16长度
#define FRAME_MIN_LEN                (FRAME_HEADER_LEN + FRAME_CMD_ID_LEN + FRAME_CRC16_LEN)
#define FRAME_MAX_DATA_LEN           118U      // 数据段最大长度
#define FRAME_MAX_PACK_LEN           127U      // 整包最大长度

/************************* 机器人ID定义 *************************/
// 红方
#define ROBOT_RED_HERO               1U
#define ROBOT_RED_ENGINEER           2U
#define ROBOT_RED_INFANTRY_3         3U
#define ROBOT_RED_INFANTRY_4         4U
#define ROBOT_RED_INFANTRY_5         5U
#define ROBOT_RED_AERIAL             6U
#define ROBOT_RED_SENTRY             7U
#define ROBOT_RED_DART               8U
#define ROBOT_RED_RADAR              9U
#define ROBOT_RED_OUTPOST            10U
#define ROBOT_RED_BASE               11U

// 蓝方
#define ROBOT_BLUE_HERO              101U
#define ROBOT_BLUE_ENGINEER          102U
#define ROBOT_BLUE_INFANTRY_3        103U
#define ROBOT_BLUE_INFANTRY_4        104U
#define ROBOT_BLUE_INFANTRY_5        105U
#define ROBOT_BLUE_AERIAL            106U
#define ROBOT_BLUE_SENTRY            107U
#define ROBOT_BLUE_DART              108U
#define ROBOT_BLUE_RADAR             109U
#define ROBOT_BLUE_OUTPOST           110U
#define ROBOT_BLUE_BASE              111U

/************************* 选手端ID *************************/
#define CLIENT_RED_HERO              0x0101U
#define CLIENT_RED_ENGINEER          0x0102U
#define CLIENT_RED_INFANTRY_3        0x0103U
#define CLIENT_RED_INFANTRY_4        0x0104U
#define CLIENT_RED_INFANTRY_5        0x0105U
#define CLIENT_RED_AERIAL            0x0106U

#define CLIENT_BLUE_HERO             0x0165U
#define CLIENT_BLUE_ENGINEER         0x0166U
#define CLIENT_BLUE_INFANTRY_3       0x0167U
#define CLIENT_BLUE_INFANTRY_4       0x0168U
#define CLIENT_BLUE_INFANTRY_5       0x0169U
#define CLIENT_BLUE_AERIAL           0x016AU

#define CLIENT_JUDGE_SERVER          0x8080U  // 裁判服务器ID

/************************* 命令码 CMD_ID *************************/
// ========== 上位机下发（接收解析） ==========
#define CMD_GAME_STATUS              0x0001U  // 比赛状态 1Hz
#define CMD_GAME_RESULT              0x0002U  // 比赛结果 触发
#define CMD_ROBOT_HP_DATA            0x0003U  // 全队血量 3Hz
#define CMD_FIELD_EVENT              0x0101U  // 场地事件 1Hz
#define CMD_REF_WARNING              0x0104U  // 裁判警告 触发
#define CMD_DART_INFO                0x0105U  // 飞镖状态 1Hz
#define CMD_ROBOT_STATUS             0x0201U  // 本机性能 10Hz
#define CMD_POWER_HEAT_DATA          0x0202U  // 能量热量 10Hz
#define CMD_ROBOT_POS                0x0203U  // 本机坐标 1Hz
#define CMD_ROBOT_BUFF               0x0204U  // 增益状态 3Hz
#define CMD_DAMAGE_DATA              0x0206U  // 受伤害 触发
#define CMD_SHOOT_DATA               0x0207U  // 射击数据 触发
#define CMD_PROJECTILE_ALLOW         0x0208U  // 剩余弹量 10Hz
#define CMD_RFID_STATUS              0x0209U  // RFID状态 3Hz
#define CMD_DART_CLIENT_CMD          0x020AU  // 飞镖选手指令 3Hz
#define CMD_GROUND_ROBOT_POS         0x020BU  // 地面机器人坐标 1Hz(哨兵)
#define CMD_RADAR_MARK_PROG          0x020CU  // 雷达标记进度 1Hz
#define CMD_SENTRY_AUTO_INFO         0x020DU  // 哨兵自主同步 1Hz
#define CMD_RADAR_AUTO_INFO          0x020EU  // 雷达自主同步 1Hz

// ========== 交互/图传链路（双向） ==========
#define CMD_ROBOT_INTERACT           0x0301U  // 机器人交互 30Hz上限
#define CMD_CUSTOM_CTRL_ROBOT        0x0302U  // 自定义控制器→机器人
#define CMD_MAP_CLICK_CMD            0x0303U  // 小地图点击指令
#define CMD_RADAR_MAP_DATA           0x0305U  // 雷达地图数据
#define CMD_CUSTOM_CTRL_CLIENT       0x0306U  // 键鼠模拟选手端
#define CMD_SENTRY_PATH_DATA         0x0307U  // 哨兵路径数据
#define CMD_ROBOT_TO_CLIENT_MSG      0x0308U  // 机器人→选手端文字
#define CMD_ROBOT_TO_CUSTOM_CTRL     0x0309U  // 机器人→自定义控制器
#define CMD_ROBOT_TO_CUSTOM_CLIENT   0x0310U  // 机器人→客户端大数据
#define CMD_CUSTOM_CLIENT_TO_ROBOT   0x0311U  // 客户端→机器人指令

// ========== 雷达无线链路（信号发射源发送） ==========
#define CMD_RADAR_ENEMY_POS          0x0A01U  // 敌方坐标 10Hz
#define CMD_RADAR_ENEMY_HP           0x0A02U  // 敌方血量 10Hz
#define CMD_RADAR_ENEMY_BULLET       0x0A03U  // 敌方弹量 10Hz
#define CMD_RADAR_GLOBAL_STATUS      0x0A04U  // 敌方全局状态 10Hz
#define CMD_RADAR_ENEMY_BUFF         0x0A05U  // 敌方增益 10Hz
#define CMD_RADAR_ENEMY_KEY          0x0A06U  // 敌方密钥 10Hz

/************************* 0x0301子命令ID *************************/
#define SUB_CMD_ROBOT_COMM           0x0200U  // 机器人自定义通信
#define SUB_CMD_MAP_DEL_LAYER        0x0100U  // 删除图层
#define SUB_CMD_MAP_DRAW_1           0x0101U  // 绘制1个图形
#define SUB_CMD_MAP_DRAW_2           0x0102U  // 绘制2个图形
#define SUB_CMD_MAP_DRAW_5           0x0103U  // 绘制5个图形
#define SUB_CMD_MAP_DRAW_7           0x0104U  // 绘制7个图形
#define SUB_CMD_MAP_DRAW_TEXT        0x0110U  // 绘制字符
#define SUB_CMD_SENTRY_AUTO_CMD      0x0120U  // 哨兵自主指令
#define SUB_CMD_RADAR_AUTO_CMD       0x0121U  // 雷达自主指令

/************************* 编译器字节对齐配置 *************************/
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#pragma pack(1)
#define PACKED
#elif defined(__GNUC__)
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

// ========== 帧头结构体 ==========
// 表1-3/表1-4 帧头格式 页4
typedef struct
{
    uint8_t  sof;               // 帧起始标志 固定0xA5
    uint16_t data_len;          // 数据段字节长度
    uint8_t  seq;               // 包序号自增
    uint8_t  crc8;              // 帧头CRC8校验
} PACKED referee_frame_header_t;

// ========== 完整帧缓存结构体 ==========
// 表1-2 通信协议格式 frame_header+cmd_id+data+CRC16 页4
typedef struct
{
    referee_frame_header_t header;
    uint16_t cmd_id;
    uint8_t  data[FRAME_MAX_DATA_LEN];
    uint16_t crc16;
} PACKED referee_full_frame_t;

/************************* 各命令码数据结构体 *************************/
// 0x0001 比赛状态
// 通信协议V2.0.0 表1-6 页9
typedef struct
{
    uint8_t  game_type : 4;     // bit0-3 比赛类型（1=超级对抗赛 2=高校单项赛 3=ICRA 4=联盟3V3 5=联盟步兵对抗）
    uint8_t  game_progress : 4; // bit4-7 当前比赛阶段（0=未开始 1=准备 2=自检 3=倒计时 4=比赛中 5=结算中）
    uint16_t stage_remain_time; // 阶段剩余时间(秒)
    uint64_t sync_time_stamp;   // UNIX时间戳（连接NTP后生效）
} PACKED game_status_t;

// 0x0002 比赛结果
// 通信协议V2.0.0 表1-7 页9
typedef struct
{
    uint8_t winner;             // 0平局 1红胜 2蓝胜
} PACKED game_result_t;

// 0x0003 全队血量（V2.0.0修订：新增伤害差、对方前哨站/基地血量）
// 通信协议V2.0.0 表1-8 页9-10
typedef struct
{
    uint16_t ally_1_robot_HP;   // 己方1号英雄血量（未上场/罚下为0）
    uint16_t ally_2_robot_HP;   // 己方2号工程血量
    uint16_t ally_3_robot_HP;   // 己方3号步兵血量
    uint16_t ally_4_robot_HP;   // 己方4号步兵血量
    int16_t  damage_difference; // 己方总伤害与对方总伤害之差
    uint16_t ally_7_robot_HP;   // 己方7号哨兵血量
    uint16_t ally_outpost_HP;   // 己方前哨站血量
    uint16_t ally_base_HP;      // 己方基地血量
    uint16_t enemy_outpost_HP;  // 对方前哨站血量
    uint16_t enemy_base_HP;     // 对方基地血量
} PACKED game_robot_HP_t;

// 0x0104 裁判警告
// 通信协议V2.0.0 表1-10 页12
typedef struct
{
    uint8_t level;              // 己方最后一次受罚等级（1=双方黄牌 2=黄牌 3=红牌 4=判负）
    uint8_t offending_robot_id; // 违规机器人ID（判负/双方黄牌时为0）
    uint8_t count;              // 该机器人对应等级的累计违规次数
} PACKED referee_warning_t;

// 0x0105 飞镖信息
// 通信协议V2.0.0 表1-11 页12
typedef struct
{
    uint8_t  dart_remaining_time; // 己方飞镖发射剩余时间(秒)
    uint16_t dart_info;           // bit0-2:最近击中目标 bit3-5:累计击中次数 bit6-8:当前选定目标
} PACKED dart_info_t;

// 0x0201 机器人性能状态（V2.0.0修订：新增bullet_speed_limit）
// 通信协议V2.0.0 表1-12 页13
typedef struct
{
    uint8_t  robot_id;                      // 本机器人ID
    uint8_t  robot_level;                   // 机器人等级
    uint16_t current_HP;                    // 当前血量
    uint16_t maximum_HP;                    // 血量上限
    uint16_t shooter_barrel_cooling_value;  // 射击热量每秒冷却值
    uint16_t shooter_barrel_heat_limit;     // 射击热量上限
    uint16_t chassis_power_limit;           // 底盘功率上限
    //float    bullet_speed_limit;            // 射击初速度上限
    uint8_t  power_management_gimbal_output  : 1;  // bit0 gimble口 0=无输出 1=24V
    uint8_t  power_management_chassis_output : 1;  // bit1 chassis口
    uint8_t  power_management_shooter_output : 1;  // bit2 shooter口
} PACKED robot_status_t;

// 0x0202 缓冲能量与热量
// 通信协议V2.0.0 表1-13 页14
typedef struct
{
    uint16_t reserved1;                     // 保留位
    uint16_t reserved2;                     // 保留位
    float    reserved3;                     // 保留位
    uint16_t buffer_energy;                 // 缓冲能量(J)
    uint16_t shooter_17mm_barrel_heat;      // 17mm发射机构射击热量
    uint16_t shooter_42mm_barrel_heat;      // 42mm发射机构射击热量
} PACKED power_heat_data_t;

// 0x0203 机器人坐标
// 通信协议V2.0.0 表1-14 页14
typedef struct
{
    float x;                                // 本机x坐标(m)
    float y;                                // 本机y坐标(m)
    float angle;                            // 测速模块朝向(度,正北为0)
} PACKED robot_pos_t;

// 0x0204 增益状态
// 通信协议V2.0.0 表1-15 页15
typedef struct
{
    uint8_t  recovery_buff;         // 回血增益(百分比,如10表示每秒恢复血量上限10%)
    uint16_t cooling_buff;          // 射击热量冷却增益(直接值,如5表示热量冷却增加5/s)
    uint8_t  defence_buff;          // 防御增益(百分比,如50表示50%防御)
    uint8_t  vulnerability_buff;    // 负防御增益(百分比,如30表示-30%防御)
    uint16_t attack_buff;           // 攻击增益(百分比,如50表示50%攻击)
    uint8_t  remaining_energy;      // bit0-6剩余能量比例标识(bit0=125% bit1=100% bit2=50% bit3=30% bit4=15% bit5=5% bit6=1%)
} PACKED robot_buff_t;

// 0x0207 射击数据
// 通信协议V2.0.0 表1-17 页16
typedef struct
{
    uint8_t bullet_type;            // 弹丸类型(bit1=17mm bit2=42mm)
    uint8_t shooter_number;         // 发射机构ID(1=17mm 3=42mm)
    uint8_t launching_frequency;    // 弹丸射速(Hz)
    float   initial_speed;          // 弹丸初速度(m/s)
} PACKED shoot_data_t;

// 0x0208 允许发弹量
// 通信协议V2.0.0 表1-18 页17
typedef struct
{
    uint16_t projectile_allowance_17mm;  // 机器人自身17mm弹丸允许发弹量
    uint16_t projectile_allowance_42mm;  // 42mm弹丸允许发弹量
    uint16_t remaining_gold_coin;        // 剩余金币数量
    uint16_t projectile_allowance_fortress; // 堡垒增益点储备17mm弹丸允许发弹量
} PACKED projectile_allowance_t;

// 0x020A 飞镖选手端指令
// 通信协议V2.0.0 表1-20 页19
typedef struct
{
    uint8_t  dart_launch_opening_status; // 飞镖发射站状态(0=已开启 1=关闭 2=正在开关)
    uint8_t  reserved;                   // 保留位
    uint16_t target_change_time;         // 切换目标时的比赛剩余时间(秒)
    uint16_t latest_launch_cmd_time;     // 最后一次发射指令时的比赛剩余时间(秒)
} PACKED dart_client_cmd_t;

// 0x020B 地面机器人坐标
// 通信协议V2.0.0 表1-21 页19
typedef struct
{
    float hero_x;                        // 己方英雄x坐标(m)
    float hero_y;                        // 己方英雄y坐标(m)
    float engineer_x;                    // 己方工程x坐标(m)
    float engineer_y;                    // 己方工程y坐标(m)
    float standard_3_x;                  // 己方3号步兵x坐标(m)
    float standard_3_y;                  // 己方3号步兵y坐标(m)
    float standard_4_x;                  // 己方4号步兵x坐标(m)
    float standard_4_y;                  // 己方4号步兵y坐标(m)
    float reserved1;                     // 保留位
    float reserved2;                     // 保留位
} PACKED ground_robot_position_t;

// 0x020D 哨兵自主信息（V2.0.0修订：新增sentry_info_3）
// 通信协议V2.0.0 表1-23 页21-22
typedef struct
{
    uint32_t sentry_info;                // 哨兵信息位域(兑换弹量/复活/能量机关等)
    uint16_t sentry_info_2;              // 哨兵姿态/脱战/可兑换信息位域
    uint64_t sentry_info_3;              // 各姿态剩余持续时长位域
} PACKED sentry_info_t;

// 0x020E 雷达自主信息
// 通信协议V2.0.0 表1-24 页23
typedef struct
{
    uint8_t radar_info;                  // bit0-1双倍易伤机会 bit2是否触发 bit3-4加密等级 bit5可否修改密钥
} PACKED radar_info_t;

// 0x0301 机器人交互数据头
// 通信协议V2.0.0 表1-25 页23
typedef struct
{
    uint16_t data_cmd_id;                // 子内容ID
    uint16_t sender_id;                  // 发送者ID
    uint16_t receiver_id;                // 接收者ID(仅限己方通信)
    uint8_t  user_data[112];             // 内容数据段(最大112字节)
} PACKED robot_interaction_data_t;

// 0x0303 小地图点击指令
// 通信协议V2.0.0 表1-35 页33
typedef struct
{
    float   target_position_x;           // 目标x坐标(m),发送ID时为0
    float   target_position_y;           // 目标y坐标(m),发送ID时为0
    uint8_t cmd_keyboard;                // 云台手按下的键盘通用键值(无按键为0)
    uint8_t target_robot_id;             // 对方机器人ID(发送坐标时为0)
    uint16_t cmd_source;                 // 信息来源ID
} PACKED map_command_t;

// 0x0306 键鼠模拟数据
// 通信协议V2.0.0 表1-43 页38
typedef struct
{
    uint16_t key_value;                  // 键盘键值(bit0-7按键1 bit8-15按键2)
    uint16_t x_position   : 12;          // 鼠标X轴像素位置
    uint16_t mouse_left   : 4;           // 鼠标左键状态(1=按下)
    uint16_t y_position   : 12;          // 鼠标Y轴像素位置
    uint16_t mouse_right  : 4;           // 鼠标右键状态(1=按下)
    uint16_t reserved;                   // 保留位
} PACKED custom_client_data_t;

// 0x0307 哨兵路径数据
// 通信协议V2.0.0 表1-37 页35
typedef struct
{
    uint8_t  intention;                  // 意图(1=到目标点攻击 2=到目标点防守 3=移动到目标点)
    uint16_t start_position_x;           // 路径起点x坐标(dm)
    uint16_t start_position_y;           // 路径起点y坐标(dm)
    int8_t   delta_x[49];                // 路径点x轴增量数组(共49个点位)
    int8_t   delta_y[49];                // 路径点y轴增量数组
    uint16_t sender_id;                  // 发送者ID
} PACKED map_data_t;

// 子命令0x0120 哨兵自主指令
// 通信协议V2.0.0 表1-33 页29
typedef struct
{
    uint32_t sentry_cmd;                 // bit0确认复活 bit1兑换立即复活 bit2-12兑换弹量 bit13-16远程兑换弹量次数 bit17-20远程兑换血量次数 bit21-23姿态 bit24激活能量机关
} PACKED sentry_cmd_t;

// 子命令0x0121 雷达自主指令
// 通信协议V2.0.0 表1-34 页31
typedef struct
{
    uint8_t radar_cmd;                   // 触发双倍易伤(单调递增)
    uint8_t password_cmd;                // 密钥指令(1=更新密钥 2=验证破解)
    uint8_t password_1;                  // 密钥byte1(ASCII字母/数字)
    uint8_t password_2;                  // 密钥byte2
    uint8_t password_3;                  // 密钥byte3
    uint8_t password_4;                  // 密钥byte4
    uint8_t password_5;                  // 密钥byte5
    uint8_t password_6;                  // 密钥byte6
} PACKED radar_cmd_t;

/************************* 全数据指针总结构体 *************************/
typedef struct
{
    // ========== 新增：专用存储缓冲（与解析器 frame_buf 隔离） ==========
    game_status_t            _game_status;
    game_result_t            _game_result;
    game_robot_HP_t          _ally_hp;
    uint32_t                 _field_event;
    referee_warning_t        _ref_warning;
    dart_info_t              _dart_info;
    robot_status_t           _robot_status;
    power_heat_data_t        _power_heat;
    robot_pos_t              _robot_pos;
    robot_buff_t             _buff;
    uint8_t                  _damage[16];
    shoot_data_t             _shoot_data;
    projectile_allowance_t   _bullet_data;
    uint8_t                  _rfid[16];
    dart_client_cmd_t        _dart_client_cmd;
    ground_robot_position_t  _ground_pos;
    uint16_t                 _radar_mark;
    sentry_info_t            _sentry_sync;
    radar_info_t             _radar_sync;
    robot_interaction_data_t _robot_inter;
    map_command_t            _map_click;
    custom_client_data_t     _mouse_key;
    map_data_t               _sentry_path;
    sentry_cmd_t             _sentry_auto_cmd;
    radar_cmd_t              _radar_cmd;

    // ========== 基础比赛数据指针 ==========
    game_status_t            *p_game_status;
    game_result_t            *p_game_result;
    game_robot_HP_t          *p_ally_hp;
    uint32_t                 *p_field_event;
    referee_warning_t        *p_ref_warning;
    dart_info_t              *p_dart_info;

    // ========== 本机状态指针 ==========
    robot_status_t           *p_robot_status;
    power_heat_data_t        *p_power_heat;
    robot_pos_t              *p_robot_pos;
    robot_buff_t             *p_buff;
    uint8_t                  *p_damage_buf;
    shoot_data_t             *p_shoot_data;
    projectile_allowance_t   *p_bullet_data;
    uint8_t                  *p_rfid_buf;
    dart_client_cmd_t        *p_dart_client_cmd;
    ground_robot_position_t  *p_ally_ground_pos;
    uint16_t                 *p_radar_mark_bit;
    sentry_info_t            *p_sentry_sync;
    radar_info_t             *p_radar_sync;

    // ========== 交互数据指针 ==========
    robot_interaction_data_t *p_robot_inter;
    map_command_t            *p_map_click;
    custom_client_data_t     *p_mouse_key;
    map_data_t               *p_sentry_path;

    // ========== 子指令指针 ==========
    sentry_cmd_t             *p_sentry_auto_cmd;
    radar_cmd_t              *p_radar_auto_cmd;

    // 更新标志位联合体
    union
    {
        uint32_t all_flag;
        struct
        {
            uint32_t game_status_upd     : 1;
            uint32_t game_result_upd     : 1;
            uint32_t ally_hp_upd         : 1;
            uint32_t field_event_upd     : 1;
            uint32_t ref_warn_upd        : 1;
            uint32_t dart_info_upd       : 1;
            uint32_t robot_status_upd    : 1;
            uint32_t power_heat_upd      : 1;
            uint32_t robot_pos_upd       : 1;
            uint32_t buff_upd            : 1;
            uint32_t damage_upd          : 1;
            uint32_t shoot_upd           : 1;
            uint32_t bullet_upd          : 1;
            uint32_t rfid_upd            : 1;
            uint32_t dart_client_upd     : 1;
            uint32_t ally_ground_upd     : 1;
            uint32_t radar_mark_upd      : 1;
            uint32_t sentry_sync_upd     : 1;
            uint32_t radar_sync_upd      : 1;
            uint32_t robot_inter_upd     : 1;
            uint32_t map_click_upd       : 1;
            uint32_t mouse_key_upd       : 1;
            uint32_t sentry_path_upd     : 1;
            uint32_t reserved            : 8;
        } bit;
    } update;
} PACKED referee_all_data_t;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#pragma pack()
#endif

/************************* 解析状态机 *************************/
typedef enum
{
    WAIT_SOF = 0,
    WAIT_HEADER_REST,
    WAIT_DATA_AND_CRC16
} ref_parse_state_e;

// 解析器句柄
typedef struct
{
    ref_parse_state_e  state;
    uint16_t           data_cnt;
    referee_full_frame_t frame_buf;
    referee_all_data_t *store;         // 外部数据存储指针（由使用者注入）
} referee_parser_t;

/************************* CRC校验函数声明（复用 CRC 模块） *************************/
#include "CRC.h"

/************************* 解析器函数声明 *************************/
void Referee_Parser_Init(referee_parser_t *parser, referee_all_data_t *store);
void Referee_Parser_Byte(referee_parser_t *parser, uint8_t byte);

/************************* 打包发送函数声明（下位机发送用） *************************/
// 通用打包函数：填充帧头+命令码+数据+CRC，返回整包长度
uint16_t Referee_Pack_Frame(uint16_t cmd_id, uint8_t *data, uint16_t data_len, uint8_t seq, uint8_t *out_buf);

// 常用发送封装
uint16_t Referee_Pack_Interaction(uint16_t sub_cmd, uint16_t sender_id, uint16_t receiver_id,
                                  uint8_t *user_data, uint8_t data_len, uint8_t seq, uint8_t *out_buf);
uint16_t Referee_Pack_Sentry_Cmd(uint32_t cmd_val, uint16_t sender_id, uint8_t seq, uint8_t *out_buf);
uint16_t Referee_Pack_Radar_Cmd(uint8_t cmd_val, uint8_t *password, uint16_t sender_id, uint8_t seq, uint8_t *out_buf);
uint16_t Referee_Pack_Custom_Ctrl(uint8_t *data, uint8_t seq, uint8_t *out_buf);
uint16_t Referee_Pack_Mouse_Key(custom_client_data_t *mk_data, uint8_t seq, uint8_t *out_buf);

#ifdef __cplusplus
}
#endif

#endif // REFEREEPROTOCOL_H_