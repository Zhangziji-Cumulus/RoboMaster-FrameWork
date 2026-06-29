#ifndef MOTORCTRL_DJI_H_
#define MOTORCTRL_DJI_H_

//** #################################################################################################### **//
//** ======================================== 头文件引用 ================================================= **//
//** #################################################################################################### **//

#include "A_CommonSystem.h"
#include "PID_Lib.h"

//** #################################################################################################### **//
//** =========================================== 宏定义 ================================================= **//
//** #################################################################################################### **//

/* 在线标志 */

#define DJI_MOTOR_ONLINE    true       // 在线
#define DJI_MOTOR_LOST      false      // 掉线/离线

/* 减速电机的减速比 */

#define DJI_M3508_RATIO     19      //M3508减速比为 19:1
#define DJI_M2006_RATIO     36      //M2006减速比为 36:1

/* 控制值范围 */
#define DJI_M2006_A   3.0f
#define DJI_M2006_R   10000
#define DJI_M3508_A   10.0f
#define DJI_M3508_R   16384
#define DJI_GM6020_A  3.0f
#define DJI_GM6020_R  16384

#define DJI_STOP_A    10000

/* CAN ID 组定义 */ 
#define ESC_CAN_ID_GROUP_1  0x200   // 控制电机 1, 2, 3, 4
#define ESC_CAN_ID_GROUP_2  0x1FF   // 控制电机 5, 6, 7, 8

/* CAN  控制 ID 组定义 */ 

#define DJI_CAN_ID_GROUP_1      1       // 控制电机 1, 2, 3, 4
#define DJI_CAN_ID_GROUP_2      2       // 控制电机 5, 6, 7, 8

#define DJI_MOTOR_STOP_TIME_MS    100    //连续进入停止模式的时间阈值，单位毫秒

//** #################################################################################################### **//
//** ========================================= 结构体、枚举 ============================================== **//
//** #################################################################################################### **//

//** 接受电机反馈数据 **//

// 错误码枚举 
typedef enum {
    MOTOR_ERR_NONE                  = 0,//无异常
    MOTOR_ERR_OVER_VOLTAGE          = 2,//电调供电电压过高（仅开机自检）
    MOTOR_ERR_PHASE_NOT_CONNECTED   = 3,//电机三相线未接入
    MOTOR_ERR_SENSOR_DATA_LOST      = 4,//与电机相连的数据线中位置传感器数据丢失
    MOTOR_ERR_STALL                 = 6,//电机堵转
    MOTOR_ERR_CALIBRATION_FAILED    = 7 //电机校准失败
} DJI_MotorErrorCode_t;

// 定义电机反馈数据结构
typedef struct {
    uint16_t id;            // 电机ID (1-8)
    uint16_t angle_raw;     // 原始角度值 (0-8191)
    float angle_deg;        // 角度值 (0-360度)
    int16_t speed_rpm;      // 转速 (rpm)
    int16_t current_ma;     // 电流 (mA, 通常电调反馈单位为mA)
    DJI_MotorErrorCode_t error_code;     // 错误码	电机错误码：
    volatile uint8_t is_online;         // 在线标志位，用于检测电机是否掉线
    uint32_t offline_timeout_cnt;
} DJI_MotorFeedback_t;

// 电调配置结构体
typedef struct {
    const char* name;           // 电调型号名称
    float max_current_amps;     // 最大电流 (A)，如 20.0 或 10.0
    int16_t max_raw_value;      // 对应的最大原始值，如 16384 或 10000
} ESC_Config_t;

//** #################################################################################################### **//
//** =================================== 结构体、数据、变量 ============================================== **//
//** #################################################################################################### **//

extern DJI_MotorFeedback_t DJI_MFeedback_CAN1[8];
extern DJI_MotorFeedback_t DJI_MFeedback_CAN2[8];
extern DJI_MotorFeedback_t DJI_MFeedback_CAN3[8];

//** #################################################################################################### **//
//** ========================================== 对外函数 ================================================= **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** =============================== 接收反馈数据 ==================================== **//
//** ================================================================================ **//

// CAN 接收回调函数，解析 DJI 电机反馈数据
void CAN_DJI_Motor_Feedback(DJI_MotorFeedback_t* DJI_MFeedback,uint32_t std_id, uint8_t* data);

//** ================================================================================ **//
//** ============================= 获取电机反馈数据 ================================== **//
//** ================================================================================ **//

#ifdef CAN1
//获取CAN1反馈数据指针数组
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback_CAN1(void);
#endif

#ifdef CAN2
//获取CAN2反馈数据指针数组
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback_CAN2(void);
#endif

#ifdef CAN3
//获取CAN3反馈数据指针数组
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback_CAN3(void);
#endif

//通用接口：通过 CAN 句柄获取对应的电机反馈数据指针数组
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback(CAN_HandleTypeDef* hcan);

//** ================================================================================ **//
//** ============================= 发射控制数据函数 ================================== **//
//** ================================================================================ **//

//** ------------------------------------------------------------ **//
//** ============== 原始值控制函数 (底层/调试用) ================== **//
//** ------------------------------------------------------------ **//

//安培值控制单个电机
void ESC_Control_Amps_Single(CAN_HandleTypeDef *hcan, const ESC_Config_t* config, uint8_t motor_id, float current_amps);
//安培值控制一组电机 (4个)
void ESC_Control_Amps_Group(CAN_HandleTypeDef *hcan, const ESC_Config_t* config, uint8_t motor_start_id, float currents_amps[4]);
//安培值控制所有电机 (8个)
void ESC_Control_Amps_All(CAN_HandleTypeDef *hcan, const ESC_Config_t* config, float currents_amps[8]);

//** ------------------------------------------------------------ **//
//** =================== 电流值控制函数 (推荐) ==================== **//
//** ------------------------------------------------------------ **//

//原始值控制单个电机
void ESC_Control_Raw_Single(CAN_HandleTypeDef *hcan, uint8_t motor_id, int16_t raw_value);
//原始值控制一组电机 (4个)
void ESC_Control_Raw_Group(CAN_HandleTypeDef *hcan, uint8_t motor_start_id, int16_t raw_values[4]);
//原始值控制所有电机 (8个)
void ESC_Control_Raw_All(CAN_HandleTypeDef *hcan,int16_t raw_values[8]);



#endif // MOTORCTRL_DJI_H_
