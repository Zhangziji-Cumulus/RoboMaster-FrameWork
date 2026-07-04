#ifndef AUTOAIM_H_
#define AUTOAIM_H_

#include "A_MCommon.h"
#include "INS_task.h"


/* 自瞄结构体自己对齐可以确定结构体的大小 */
#pragma pack(push, 1)   // 1字节对齐（通信必备）

// ==============================
// 接收：上位机 → 下位机
// 大小：20 字节
// ==============================
typedef struct
{
    uint8_t Frame_head;//帧头 0x5A
    uint8_t reserved0;// 填充
    uint8_t reserved1;// 填充
    uint8_t reserved2;// 填充（4字节对齐）

    float Yaw;        // 4
    float Pitch;      // 4
    int8_t Fire;      // 1
    int8_t reserved3; // 1
    int8_t reserved4; // 1
    int8_t reserved5; // 1
    int32_t Match;    // 4

} AutoAim_Rx_t;       // 总：4+4+4+1+1+1+1+4 = 20 字节


// ==============================
// 发送：下位机 → 上位机（已修正对齐 + 帧尾）
// 大小：24 字节
// ==============================
typedef struct
{
    uint8_t Frame_head;   // 帧头 0x5A
    uint8_t Enemy_Color;  // 敌方颜色
    uint8_t reserved0;    // 填充
    uint8_t reserved1;    // 填充（4字节对齐）

    float IMU_Roll;       // 4
    float IMU_Pitch;      // 4
    float IMU_Yaw;        // 4

    int32_t Match;        // 4

} AutoAim_Tx_t;
// 总大小：
// 1+1+1+1 = 4
// +4+4+4 = 12
// +4 = 4
// +1+1+1+1 = 4
// 总计：4+12+4+4 = 24 字节 

#pragma pack(pop)

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

    // 通信发送
    AutoAim_Tx_t Tx;    
    uint8_t Tx_Buff[sizeof(AutoAim_Tx_t)];  //发送缓存
    uint8_t Tx_Done;    //已经发送一组数据标志位

}AutoAim_Instance_t;

typedef struct{
    float Yaw;
    float Pitch;
    uint8_t FireOK;//可以视觉瞄准了，可以开火了
    uint8_t IsOnline;//视觉是否在线标志,0不在线，1在线
}AutoAim_Ctrl_t;

/* 纯整数 int16_t 自瞄+手动融合函数 */
int16_t AutoAim_WeightFusion_Int16(int16_t manual, int16_t auto_val, uint8_t aim_valid, int16_t min_out, int16_t max_out);
/* 浮点型 float 自瞄+手动融合函数*/
float AutoAim_WeightFusion_Float(float manual, float auto_val, uint8_t aim_valid, float min_out, float max_out);
/* 获取自瞄控制参数 */
const AutoAim_Ctrl_t* AutoAim_Ctrl_Get_point(void);

#endif // AUTOAIM_H_
