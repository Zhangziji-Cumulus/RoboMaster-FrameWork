#ifndef AUTOAIM_H_
#define AUTOAIM_H_

#include "A_MCommon.h"
#include "INS_task.h"


/* 自瞄接收超时时间 (ms)：超过此时间未收到有效帧视为离线 */
#define AUTOAIM_RX_TIMEOUT_MS   25

/* 帧率测量低通系数（仅用于统计显示，不参与滤波） */
#define AUTOAIM_FPS_LPF_ALPHA       0.1f

/* 跟随器工况模式 */
#define FOLLOWER_MODE_STILL     0
#define FOLLOWER_MODE_RAMP      1
#define FOLLOWER_MODE_CORNER    2
#define FOLLOWER_MODE_STEP      3

/* ================================================================
 * 运行时可调参数结构体
 * 在 Debug 时将 &autoaim_param 加入 Watch 窗口，展开后双击值即可
 * 实时修改，无需重新编译下载
 * ================================================================ */
typedef struct {
    // ==================== Phase 1: 位置跟踪 ====================
    float Scale_Yaw;            /* [0.10] Yaw缩放因子, ↑跟不上则增大, ↓抖动则减小 */
    float Scale_Pitch;          /* [0.08] Pitch缩放因子, 比Yaw小一点(惯量小) */
    float Gain;                 /* [1.0]  全局增益, 调好Scale后仍不满意再动 */
    float Alpha_Still;          /* [0.10] 静止滤波系数, ↑=更强滤波抑制抖动 */
    float DeadBand;             /* [0.1]  指令死区°, 过滤视觉帧间小噪声 */
    float Pos_DeadBand;         /* [0.00] 输出死区°, Ctrl变化小于此值不更新云台, 静止微抖时尝试0.01~0.03 */
    float Max_Jump_Deg;         /* [5.0]  跳变丢弃阈值°, 飞点抖动时减小 */

    // ==================== Phase 2: 匀速跟踪 ====================
    float Alpha_Ramp;           /* [0.40] 匀速滤波系数, ↑跟上更快, ↓更平滑 */
    float Kff_Ramp;             /* [0.0]  匀速前馈增益, ↑减小滞后, 过冲则↓ */
    float Delta_Thr_Ramp;       /* [0.5]  匀速判定阈值°, 偏大则更难触发 */

    // ==================== Phase 3: 阶跃响应 ====================
    float Alpha_Step;           /* [0.90] 阶跃滤波系数, 目标突跳时响应速度 */
    float Kff_Step;             /* [0.0]  阶跃前馈增益, ↑加快到位, 过冲则↓ */
    float Delta_Thr_Step;       /* [3.0]  阶跃判定阈值°, 越小越易触发 */

    // ==================== Phase 4: 拐点平滑 ====================
    float Alpha_Corner;         /* [0.25] 拐点滤波系数, ↑响应快, ↓更平滑 */
    float Kff_Corner;           /* [0.0]  拐点前馈衰减, ↑减少拐点停顿感 */
    float Corner_Hold_Time;     /* [200]  拐点保持时间ms, ↑更久才退出拐点模式 */

    // ==================== PID层速度前馈 ====================
    float PID_FF_Gain_Yaw;      /* [0]  Yaw PID层前馈, 跟踪滞后时开启 */
    float PID_FF_Gain_Pitch;    /* [0]  Pitch PID层前馈 */
    float FF_Decay_K;           /* [5.0] 前馈误差衰减, 误差小时减小前馈 */
    int16_t FF_Max_Yaw;         /* [800]  Yaw前馈限幅 */
    int16_t FF_Max_Pitch;       /* [300]  Pitch前馈限幅 */

    // ==================== 目标层速度前馈 ====================
    float TargetFF_Gain;        /* [0.0] 目标层前馈增益, 融合进位置环 */
    float FF_LPF_Alpha;         /* [0.5] 速度低通滤波, ↑响应快, ↓更平滑 */
    float FF_DeadZone;          /* [1.0] 速度死区°/s, 静止时前馈漂移则增大 */

} AutoAim_Param_t;

/* 全局参数量实例声明（在 AutoAim.c 中定义并初始化） */
extern AutoAim_Param_t autoaim_param;

/* 自瞄结构体自己对齐可以确定结构体的大小 */
#pragma pack(push, 1)   // 1字节对齐（通信必备）

// ==============================
// 接收：上位机 → 下位机
// 大小：20 字节
// ==============================
typedef struct
{
    uint8_t Frame_head;//帧头 0x5A

    // uint8_t reserved0;// 填充
    // uint8_t reserved1;// 填充
    // uint8_t reserved2;// 填充（4字节对齐）

    float Yaw;        // 4
    float Pitch;      // 4
    int32_t Fire;      // 1

    uint16_t crc;

    // int8_t reserved3; // 1
    // int8_t reserved4; // 1
    // int8_t reserved5; // 1

    //int32_t Match;    // 4

} AutoAim_Rx_t;       // 总：4+4+4+1+1+1+1+4 = 20 字节


// ==============================
// 发送：下位机 → 上位机（已修正对齐 + 帧尾）
// 大小：24 字节
// ==============================
typedef struct
{
    uint8_t Frame_head;   // 帧头 0x5A
    uint8_t Enemy_Color;  // 敌方颜色

    // uint8_t reserved0;    // 填充
    // uint8_t reserved1;    // 填充（4字节对齐）

    float IMU_Roll;       // 4
    float IMU_Pitch;      // 4
    float IMU_Yaw;        // 4

    //int32_t Match;        // 4

} AutoAim_Tx_t;

// 总大小：
// 1+1+1+1 = 4
// +4+4+4 = 12
// +4 = 4
// +1+1+1+1 = 4
// 总计：4+12+4+4 = 24 字节 

#pragma pack(pop)

/* 自适应跟随器状态结构体（每个轴独立一个实例） */
typedef struct
{
    float cmd_prev;             /* 上一帧原始指令值 */
    float cmd_delta_prev;       /* 上一帧指令变化量 */
    float filtered;             /* 滤波后的值（原始域，未乘Scale） */
    float feedforward;          /* 前馈速度值 (°/s) */
    uint32_t tick_prev;         /* 上一帧时间戳(ms)，用于计算帧间隔 */
    uint8_t initialized;        /* 首次初始化标志 */
    uint8_t direction_reversed; /* 方向反转标志 */
    float last_reversal_time;   /* 上次反转时间戳(ms) */
    uint8_t mode;               /* 当前工况 FOLLOWER_MODE_* */
} AutoAim_Follower_t;

typedef struct
{
    //下位机变量
    struct{

        const fp32* INS_angle;  //INS角度
        uint8_t  Self_Color;    //己方颜色

    }MCUData;

    // 通信接收
    AutoAim_Rx_t Rx;
    // 通信接收缓冲区
    AutoAim_Rx_t Rx_Buf[2];    //双DMA接收缓冲
    uint8_t Rx_ActiveBuf;      //DMA当前使用缓存索引
    AutoAim_Rx_t Rx_ParseBuf;  //独立解析缓存
    uint32_t Rx_LastTick;       //最后有效帧的时间戳(ms)
    uint8_t Rx_OnlineFlag;

    // 自适应跟随器（Yaw/Pitch各一个独立实例）
    AutoAim_Follower_t Follower_Yaw;
    AutoAim_Follower_t Follower_Pitch;

    // 位置输出死区：上一次实际输出的 Ctrl 值
    float Ctrl_Yaw_Last;
    float Ctrl_Pitch_Last;

    // 通信发送
    AutoAim_Tx_t Tx;    
    uint8_t Tx_Buff[sizeof(AutoAim_Tx_t)];  //发送缓存
    uint8_t Tx_Done;    //已经发送一组数据标志位

    // === 帧率统计（仅显示用） ===
    float   MeasuredFPS;
    uint32_t LastRxTick;         //上一次有效帧的RxTick(用于检测数据是否更新)

    // === 异常帧检测 ===
    float   LastRawYaw;
    float   LastRawPitch;

}AutoAim_Instance_t;

typedef struct{
    float Yaw;
    float Pitch;
    uint8_t FireOK;     //可以视觉瞄准了，可以开火了
    uint8_t IsOnline;   //视觉是否在线标志,0不在线，1在线
    uint32_t RxTick;    //最后有效帧的接收时间戳(ms)，供外部模块检测新数据
    float YawVel;       //目标Yaw角速度(°/s)，用于速度前馈
    float PitchVel;     //目标Pitch角速度(°/s)，用于速度前馈
}AutoAim_Ctrl_t;

/* 纯整数 int16_t 自瞄+手动融合函数 */
int16_t AutoAim_WeightFusion_Int16(int16_t manual, int16_t auto_val, uint8_t aim_valid, int16_t min_out, int16_t max_out);
/* 浮点型 float 自瞄+手动融合函数*/
float AutoAim_WeightFusion_Float(float manual, float auto_val, uint8_t aim_valid, float min_out, float max_out);
/* 单轴自瞄+手动融合函数（含速度前馈），供云台模块调用 */
float AutoAim_FusionAxis(float manual, float auto_val, float auto_vel, uint8_t aim_online, float dt_sec);
/* 获取自瞄控制参数 */
const AutoAim_Ctrl_t* AutoAim_Ctrl_Get_point(void);

#endif // AUTOAIM_H_
