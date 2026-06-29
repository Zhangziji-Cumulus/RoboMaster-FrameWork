/*
 * ============================================================================
 * DJI 电机电调 CAN 通信协议说明
 * ============================================================================
 * 1. M3508 + C620 电调:
 *    - 支持电机 ID: 1 ~ 8
 *    - CAN 标准帧 ID: 0x201 ~ 0x208 (基址 0x200 + ID)
 *    - 控制电流原始值范围: -16384 ~ 0 ~ 16384
 *    - 对应物理电流范围: -20A ~ 0A ~ 20A
 *    - 额定持续电流: 10A
 *
 * 2. M2006 + C610 电调:
 *    - 支持电机 ID: 1 ~ 8
 *    - CAN 标准帧 ID: 0x201 ~ 0x208 (基址 0x200 + ID)
 *    - 控制电流原始值范围: -10000 ~ 0 ~ 10000
 *    - 对应物理电流范围: -10A ~ 0A ~ 10A
 *    - 额定持续电流: 3A
 *
 * 3. M6020 (内置电调):
 *    - 支持电机 ID: 1 ~ 7
 *    - CAN 标准帧 ID: 0x205 ~ 0x20B (基址 0x204 + ID)
 *    - 控制电压原始值范围: -25000 ~ 0 ~ 25000
 *    - 控制电流原始值范围: -16384 ~ 0 ~ 16384
 *    - 对应物理电流范围: -3A ~ 0A ~ 3A
 * ============================================================================
 */
 
 #include "A_CommonDrivers.h"

//** #################################################################################################### **//
//** ==================================== 定义数据、变量 ================================================= **//
//** #################################################################################################### **//

// CAN 总线电机状态反馈数组 (CAN1 、CAN2 、CAN3各独立维护，最多支持 8 个电机)
DJI_MotorFeedback_t DJI_MFeedback_CAN1[8];
DJI_MotorFeedback_t DJI_MFeedback_CAN2[8];
DJI_MotorFeedback_t DJI_MFeedback_CAN3[8];

//** #################################################################################################### **//
//** ========================================== 对外函数 ================================================= **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** ================================ 接收反馈数据 =================================== **//
//** ================================================================================ **//

/**
 * @brief 解析 DJI 电机 CAN 反馈帧
 * @param  DJI_MFeedback: 电机状态数组指针
 * @param  std_id:        CAN 标准帧 ID (有效范围 0x201 ~ 0x208)
 * @param  data:          8 字节 CAN 数据载荷
 * @retval None
 */
void CAN_DJI_Motor_Feedback(DJI_MotorFeedback_t* DJI_MFeedback, uint32_t std_id, uint8_t* data)
{
    // 1. 校验 ID 是否在有效范围内 (协议规定: 0x200 + 电机ID)
    // 仅支持 ID 1 ~ 8，对应 CAN ID 0x201 ~ 0x208
    if (std_id >= 0x201 && std_id <= 0x208) 
    {
        uint8_t index = std_id - 0x201; // 将 CAN ID 映射为数组索引 0~7
        
        DJI_MFeedback[index].id = index + 1;

        // 收到数据 → 立刻重置超时计数器
        DJI_MFeedback[index].offline_timeout_cnt = 0;

        // 成功接收数据，标记该电机在线
        DJI_MFeedback[index].is_online = 1; 

        // 2. 解析机械角度 (DATA[0] 高8位, DATA[1] 低8位)
        // 原始值范围 0 ~ 8191，对应物理角度 0° ~ 360°
        DJI_MFeedback[index].angle_raw = (uint16_t)((data[0] << 8) | data[1]);
        
        // 转换为实际角度值 (浮点型，按需使用)
        DJI_MFeedback[index].angle_deg = (float)DJI_MFeedback[index].angle_raw * 360.0f / 8192.0f;

        // 3. 解析转速 (DATA[2] 高8位, DATA[3] 低8位)
        // 单位: rpm (转/分钟)
        DJI_MFeedback[index].speed_rpm = (int16_t)((data[2] << 8) | data[3]);

        // 4. 解析实际电流 (DATA[4] 高8位, DATA[5] 低8位)
        // 注: 官方协议中该值为电调实际输出电流，单位通常为 mA
        DJI_MFeedback[index].current_ma = (int16_t)((data[4] << 8) | data[5]);

        // 5. 解析错误码 (DATA[7])
        DJI_MFeedback[index].error_code = (DJI_MotorErrorCode_t)data[7];
        
        // DATA[6] 为保留字段 (部分电调版本用于表示温度)，此处按协议忽略
    }
}

/**
 * @brief  电机在线状态检测（必须 1ms 调用一次）
 * @param  DJI_MFeedback: 电机反馈结构体数组
 */
void CAN_DJI_Motor_CheckOnline(DJI_MotorFeedback_t* DJI_MFeedback)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (DJI_MFeedback[i].offline_timeout_cnt < 100)
        {
            // 没超时 → 计数器 +1
            DJI_MFeedback[i].offline_timeout_cnt++;
        }
        else
        {
            // 超时 → 标记离线
            DJI_MFeedback[i].is_online = 0;
        }
    }
}



//** ================================================================================ **//
//** ============================= 获取电机反馈数据 ================================== **//
//** ================================================================================ **//

#ifdef CAN1
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback_CAN1(void) {
    return DJI_MFeedback_CAN1;
}
#endif

#ifdef CAN2
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback_CAN2(void) {
    return DJI_MFeedback_CAN2;
}
#endif

#ifdef CAN3
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback_CAN3(void) {
    return DJI_MFeedback_CAN3;
}
#endif

/**
 * @brief 通用接口：通过 CAN 句柄获取对应的电机反馈数据指针数组
 * @param  hcan: CAN 句柄指针 (hcan1, hcan2)
 * @retval 对应的电机反馈数据数组指针，若句柄无效则返回 NULL
 */
const DJI_MotorFeedback_t* MotorCtrl_DJI_GetDJI_MFeedback(CAN_HandleTypeDef* hcan) {
    if (hcan == NULL) {
        return NULL;
    }
    
    // 通过比较 CAN 实例地址来判断是哪个 CAN 总线
#ifdef CAN1
    if (hcan->Instance == CAN1) {
        return DJI_MFeedback_CAN1;
    } 
#endif

#ifdef CAN2
    if (hcan->Instance == CAN2) {
        return DJI_MFeedback_CAN2;
    } 
#endif

#ifdef CAN3
    if (hcan->Instance == CAN3) {
        return DJI_MFeedback_CAN3;
    } 
#endif

    else {
        return NULL;  // 无效的 CAN 实例
    }
}

//** ================================================================================ **//
//** ============================= 基本控制数据函数 ================================== **//
//** ================================================================================ **//

//** ------------------------------------------------------------ **//
//** ===================== 电调配置参数实例 ====================== **//
//** ------------------------------------------------------------ **//

// 20A 协议版本配置 (C620 电调: -20A~20A 对应原始值 -16384~16384)
const ESC_Config_t ESC_C620_20A = {
    .name = "ESC_20A_Protocol",
    .max_current_amps = 20.0f,
    .max_raw_value = 16384
};

// 10A 协议版本配置 (C610 电调: -10A~10A 对应原始值 -10000~10000)
const ESC_Config_t ESC_C610_10A = {
    .name = "ESC_10A_Protocol",
    .max_current_amps = 10.0f,
    .max_raw_value = 10000
};

//** ------------------------------------------------------------ **//
//** ======================= 内部辅助函数 ======================== **//
//** ------------------------------------------------------------ **//

/**
 * @brief 16位有符号整数转大端字节序 (高字节在前，低字节在后)
 * @param value: 待转换的16位整数
 * @param high:  输出高8位字节指针
 * @param low:   输出低8位字节指针
 */
static void Int16_To_BigEndian(int16_t value, uint8_t* high, uint8_t* low) {
    uint16_t u_value = (uint16_t)value;
    *high = (u_value >> 8) & 0xFF;
    *low  = u_value & 0xFF;
}

/**
 * @brief 物理电流(A) 转 协议原始值
 * @param config: 电调配置结构体指针
 * @param amps:   目标电流值 (单位: A)
 * @return 限幅后的协议原始值 (int16_t)
 */
static int16_t Amps_To_Raw(const ESC_Config_t* config, float amps) {
    if (config == NULL || config->max_current_amps <= 0) {
        return 0;
    }

    // 计算比例系数: 原始值满量程 / 物理电流满量程
    float ratio = (float)config->max_raw_value / config->max_current_amps;
    
    // 浮点转整型计算
    int32_t raw_val_32 = (int32_t)(amps * ratio);
    
    // 协议级限幅
    if (raw_val_32 > config->max_raw_value) 
        raw_val_32 = config->max_raw_value;
    if (raw_val_32 < -config->max_raw_value) 
        raw_val_32 = -config->max_raw_value;
        
    // int16_t 安全限幅 (防溢出)
    if (raw_val_32 > 32767) raw_val_32 = 32767;
    if (raw_val_32 < -32768) raw_val_32 = -32768;

    return (int16_t)raw_val_32;
}

/**
 * @brief 根据起始电机 ID 获取对应的 CAN 组播控制帧 ID
 * @param motor_start_id: 起始电机编号 (1 或 5)
 * @return CAN 标准帧 ID (失败返回 0)
 */
static uint32_t Get_CAN_ID(uint8_t motor_start_id) {
    if (motor_start_id == 1) return ESC_CAN_ID_GROUP_1; // 通常对应 0x200
    if (motor_start_id == 5) return ESC_CAN_ID_GROUP_2; // 通常对应 0x1FF
    return 0;
}

//** ------------------------------------------------------------ **//
//** ============== 原始值控制函数 (底层/调试用) ================== **//
//** ------------------------------------------------------------ **//

/**
 * @brief 以协议原始值控制单个电机
 * @param hcan:      CAN 外设句柄指针
 * @param motor_id:  目标电机 ID (1 ~ 8)
 * @param raw_value: 协议原始值 (如 8192, -10000 等)
 */
void ESC_Control_Raw_Single(CAN_HandleTypeDef *hcan, uint8_t motor_id, int16_t raw_value) {
    uint8_t data[8] = {0};
    uint32_t can_id = 0;
    int data_index = 0;

    if (motor_id >= 1 && motor_id <= 4) {
        can_id = ESC_CAN_ID_GROUP_1;
        data_index = (motor_id - 1) * 2;
    } else if (motor_id >= 5 && motor_id <= 8) {
        can_id = ESC_CAN_ID_GROUP_2;
        data_index = (motor_id - 5) * 2;
    } else {
        return;
    }

    Int16_To_BigEndian(raw_value, &data[data_index], &data[data_index + 1]);
    CAN_Send_STD(hcan, can_id, data);
}

/**
 * @brief 以协议原始值批量控制一组电机 (4个)
 * @param hcan:            CAN 外设句柄指针
 * @param motor_start_id:  组起始电机 ID (1 或 5)
 * @param raw_values:      长度为 4 的整型数组，依次对应组内 4 个电机的原始控制值
 */
void ESC_Control_Raw_Group(CAN_HandleTypeDef *hcan, uint8_t motor_start_id, int16_t raw_values[4]) {
    uint8_t data[8];
    uint32_t can_id = Get_CAN_ID(motor_start_id);
    
    if (can_id == 0) return;

    for (int i = 0; i < 4; i++) {
        Int16_To_BigEndian(raw_values[i], &data[i*2], &data[i*2 + 1]);
    }

    CAN_Send_STD(hcan, can_id, data);
}


//** ------------------------------------------------------------ **//
//** =================== 电流值控制函数 (推荐) ==================== **//
//** ------------------------------------------------------------ **//

/**
 * @brief 以物理电流(A)控制单个电机
 * @param hcan:         CAN 外设句柄指针
 * @param config:       电调配置指针 (如 &ESC_C620_20A)
 * @param motor_id:     目标电机 ID (1 ~ 8)
 * @param current_amps: 目标电流值 (单位: 安培 A)，例如 5.5f 或 -3.0f
 */
void ESC_Control_Amps_Single(CAN_HandleTypeDef *hcan, const ESC_Config_t* config, uint8_t motor_id, float current_amps){
    uint8_t data[8] = {0};
    uint32_t can_id = 0;
    int data_index = 0;

    if (motor_id >= 1 && motor_id <= 4) {
        can_id = ESC_CAN_ID_GROUP_1;
        data_index = (motor_id - 1) * 2;
    } else if (motor_id >= 5 && motor_id <= 8) {
        can_id = ESC_CAN_ID_GROUP_2;
        data_index = (motor_id - 5) * 2;
    } else {
        return; // ID 越界，直接返回
    }

    int16_t raw_val = Amps_To_Raw(config, current_amps);
    Int16_To_BigEndian(raw_val, &data[data_index], &data[data_index + 1]);
    CAN_Send_STD(hcan, can_id, data);
}

/**
 * @brief 以物理电流(A)批量控制一组电机 (4个)
 * @param hcan:            CAN 外设句柄指针
 * @param config:          电调配置指针
 * @param motor_start_id:  组起始电机 ID (仅支持 1 或 5)
 * @param currents_amps:   长度为 4 的浮点数组，依次对应组内 4 个电机的目标电流
 */
void ESC_Control_Amps_Group(CAN_HandleTypeDef *hcan, const ESC_Config_t* config, uint8_t motor_start_id, float currents_amps[4]) {
    uint8_t data[8] = {0};
    uint32_t can_id = Get_CAN_ID(motor_start_id);
    
    if (can_id == 0) return; // 非法起始 ID

    for (int i = 0; i < 4; i++) {
        int16_t raw_val = Amps_To_Raw(config, currents_amps[i]);
        Int16_To_BigEndian(raw_val, &data[i*2], &data[i*2 + 1]);
    }

    CAN_Send_STD(hcan, can_id, data);
}

//** ================================================================================ **//
//** ============================= 进阶控制数据函数 ================================== **//
//** ================================================================================ **//

