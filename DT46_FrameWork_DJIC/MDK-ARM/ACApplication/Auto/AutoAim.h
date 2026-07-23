#ifndef AUTOAIM_H_
#define AUTOAIM_H_

#include "A_MCommon.h"
#include "INS_task.h"


/* 自瞄接收超时时间 (ms)：超过此时间未收到有效帧视为离线 */
#define AUTOAIM_RX_TIMEOUT_MS   25

/* 帧率自适应基准配置（换摄像头无需改参数，自动适配） */
#define AUTOAIM_FPS_BASE            60.0f   /* 基准帧率 */
#define AUTOAIM_EMA_ALPHA_BASE      0.25f   /* 基准帧率下的 EMA_Alpha_Min */
#define AUTOAIM_EMA_ALPHA_ABSOLUTE_MIN  0.10f  /* EMA_Alpha_Min 下限 */
#define AUTOAIM_EMA_ALPHA_ABSOLUTE_MAX  0.80f  /* EMA_Alpha_Min 上限 */
#define AUTOAIM_FPS_LPF_ALPHA       0.1f    /* 帧率测量低通系数 */

/* ================================================================
 * 运行时可调参数结构体
 * 在 Debug 时将 &autoaim_param 加入 Watch 窗口，展开后双击值即可
 * 实时修改，无需重新编译下载
 * ================================================================ */
typedef struct {
    /* 角度修正 */
    float Gain;                 /* 自瞄修正增益 (推荐1.0f) */

    /* 速度前馈（PID层） */
    float PID_FF_Gain_Yaw;      /* Yaw PID层前馈增益 (原50.0f) */
    float PID_FF_Gain_Pitch;    /* Pitch PID层前馈增益 (原8.0f) */
    float FF_Decay_K;           /* 前馈误差衰减常数 (推荐5.0f) */
    int16_t FF_Max_Yaw;         /* Yaw前馈最大控制量 (800) */
    int16_t FF_Max_Pitch;       /* Pitch前馈最大控制量 (300) */

    /* 目标层速度前馈 */
    float TargetFF_Gain;        /* 目标层前馈增益 (推荐0.5f) */
    float FF_LPF_Alpha;         /* 速度低通滤波系数 (推荐0.5f) */
    float FF_DeadZone;          /* 速度死区°/s (推荐1.0f) */

    /* 自适应EMA滤波器 */
    float EMA_Alpha_Min;        /* EMA最小平滑系数 (推荐0.40f) */
    float EMA_Alpha_Max;        /* EMA最大平滑系数 (推荐0.95f) */
    float EMA_Threshold;        /* EMA变化率阈值° (推荐1.0f) */
    float Max_Jump_Deg;         /* 异常跳变丢弃阈值° (推荐5.0f) */

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

/* 自适应EMA滤波器状态结构体 */
typedef struct
{
    float Yaw;              // 滤波后的Yaw（原始域，未乘0.1）
    float Pitch;            // 滤波后的Pitch
    float Yaw_PrevRaw;      // 上一次原始Yaw（用于计算变化率）
    float Pitch_PrevRaw;    // 上一次原始Pitch
    uint8_t Initialized;    // 首次初始化标志（首次直接赋值不滤波）
} AutoAim_EMA_t;

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

    // 自适应EMA滤波器
    AutoAim_EMA_t EMA;

    // 速度前馈：上一次Ctrl值和时刻（用于计算目标角速度）
    float Ctrl_Yaw_Prev;
    float Ctrl_Pitch_Prev;
    uint32_t Ctrl_Tick_Prev;

    // 通信发送
    AutoAim_Tx_t Tx;    
    uint8_t Tx_Buff[sizeof(AutoAim_Tx_t)];  //发送缓存
    uint8_t Tx_Done;    //已经发送一组数据标志位

    // === 帧率自适应 ===
    float   MeasuredFPS;         //实测帧率
    uint32_t LastRxTick;         //上一次有效帧的RxTick(用于检测数据是否更新)

    // === 异常帧检测 ===
    float   LastRawYaw;          //上一次原始Yaw(用于跳变检测)
    float   LastRawPitch;        //上一次原始Pitch

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
