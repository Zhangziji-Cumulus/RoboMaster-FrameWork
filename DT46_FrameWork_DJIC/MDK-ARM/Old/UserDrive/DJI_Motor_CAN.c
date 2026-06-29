#include "DJI_Motor_CAN.h"
//#include "can.h"

/*
	3508ʹ��C620���֧��ID 1-8����0x200+ID��ʼ��0x201/0x202/0x203/0x204/0x205/0x206/0x207/0x208
	���Ƶ���ֵ��Χ -16384~0~16384����Ӧ��������ת�ص�����Χ-20~0~20A
	�������ǣ���M3508����C620����������10A
	
	2006ʹ��C610���֧��ID 1-8����0x200+ID��ʼ��0x201/0x202/0x203/0x204/0x205/0x206/0x207/0x208
	����ת�ص���ֵ��Χ -10000~0~10000����Ӧ��������ת�ص�����Χ -10~0~10A
	�������ǣ���M2006����C610��������3A
	
	6020�Դ�����֧�� ID1-7����0x204+ID��ʼ��0x205/0x206/0x207/0x208/0x209/0x20A/0x20B
	���Ƶ�ѹֵ��Χ��-25000~0~25000
	���Ƶ���ֵ��Χ��-16384~0~16384, ��Ӧ���ת�ص�����Χ -3A~0~3A
	
*/

//** ���ܵ���������� **//

//* ����ṹ�� *//
DJI_MotorFeedback_t DJI_MFeedback_CAN1[8];
DJI_MotorFeedback_t DJI_MFeedback_CAN2[8];
//* ���⺯�� *//
/**
  * @brief  ��������������� (����ͼƬЭ��)
  * @param  std_id: CAN ��׼֡ ID (���� 0x201)
  * @param  data: 8�ֽ���������
  * @retval None
  */
void CAN_DJI_Motor_Feedback(DJI_MotorFeedback_t* DJI_MFeedback,uint32_t std_id, uint8_t* data)
{
    // 1. ��� ID �Ƿ��ڷ�����Χ�� (0x200 + ID)
    // ����֧�� ID 1 �� 8���� 0x201 �� 0x208
    if (std_id >= 0x201 && std_id <= 0x208) 
    {
        uint8_t index = std_id - 0x201; // �� ID ӳ�䵽�������� 0-7
        
        DJI_MFeedback[index].id = index + 1;
        DJI_MFeedback[index].is_online = true; // �յ����ݣ��������

        // 2. �����Ƕ� (DATA[0] ��8λ, DATA[1] ��8λ)
        // ��Χ 0-8191 ��Ӧ 0-360��
        DJI_MFeedback[index].angle_raw = (uint16_t)((data[0] << 8) | data[1]);
        
        // ת��Ϊ�Ƕ�ֵ (��ѡ���������)
        DJI_MFeedback[index].angle_deg = (float)DJI_MFeedback[index].angle_raw * 360.0f / 8192.0f;

        // 3. ����ת�� (DATA[2] ��8λ, DATA[3] ��8λ)
        // ��λ rpm
        DJI_MFeedback[index].speed_rpm = (int16_t)((data[2] << 8) | data[3]);

        // 4. �������� (DATA[4] ��8λ, DATA[5] ��8λ)
        // ע�⣺ͼƬδ��ȷ��λ����ͨ�� DJI ���������λΪ mA
        DJI_MFeedback[index].current_ma = (int16_t)((data[4] << 8) | data[5]);

        // 5. ���������� (DATA[7])
        DJI_MFeedback[index].error_code = (DJI_MotorErrorCode_t)data[7];
        
        // DATA[6] Ϊ�գ�����
    }
}



//** ���Ƶ������ **//

//* ���⺯�� *//

/* ================= �������ʵ�� ================= */

// 20A �汾������� (Э�� 1: -20A~20A ��Ӧ -16384~16384)
const ESC_Config_t ESC_C620_20A = {
    .name = "ESC_20A_Protocol",
    .max_current_amps = 20.0f,
    .max_raw_value = 16384
};

// 10A �汾������� (Э�� 2: -10A~10A ��Ӧ -10000~10000)
const ESC_Config_t ESC_C610_10A = {
    .name = "ESC_10A_Protocol",
    .max_current_amps = 10.0f,
    .max_raw_value = 10000
};

/* ================= �ڲ��������� ================= */

// 16 λ����ת���ģʽ (�� 8 λ��ǰ���� 8 λ�ں�)
static void Int16_To_BigEndian(int16_t value, uint8_t* high, uint8_t* low) {
    uint16_t u_value = (uint16_t)value;
    *high = (u_value >> 8) & 0xFF;
    *low  = u_value & 0xFF;
}

// ���Ļ��㣺���� �� ԭʼֵ
static int16_t Amps_To_Raw(const ESC_Config_t* config, float amps) {
    if (config == NULL || config->max_current_amps <= 0) {
        return 0;
    }

    // �������ϵ��
    float ratio = (float)config->max_raw_value / config->max_current_amps;
    
    // ����
    int32_t raw_val_32 = (int32_t)(amps * ratio);
    
    // �޷�����
    if (raw_val_32 > config->max_raw_value) 
        raw_val_32 = config->max_raw_value;
    if (raw_val_32 < -config->max_raw_value) 
        raw_val_32 = -config->max_raw_value;
    if (raw_val_32 > 32767) raw_val_32 = 32767;
    if (raw_val_32 < -32768) raw_val_32 = -32768;

    return (int16_t)raw_val_32;
}

// ��ȡ CAN ID
static uint32_t Get_CAN_ID(uint8_t motor_start_id) {
    if (motor_start_id == 1) return ESC_CAN_ID_GROUP_1;
    if (motor_start_id == 5) return ESC_CAN_ID_GROUP_2;
    return 0;
}

/* ================= �ײ㷢�� (STM32 HAL) ================= */

//extern CAN_HandleTypeDef hcan1;

//void CAN_DJI_SendSTD(uint32_t id, uint8_t* data) {
//    CAN_TxHeaderTypeDef TxHeader;
//    uint32_t TxMailbox;

//    TxHeader.StdId = id;
//    TxHeader.IDE = CAN_ID_STD;
//    TxHeader.RTR = CAN_RTR_DATA;
//    TxHeader.DLC = 8;
//    TxHeader.TransmitGlobalTime = DISABLE;

//    // �ȴ������������
//    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

//    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox);
//}

/* ================= ����ֵ���ƺ��� ================= */

/**
 * @brief ����ֵ���Ƶ������
 * @param canָ����
 * @param config: �������ָ�� (�� &ESC_CONFIG_20A)
 * @param motor_id: ��� ID (1 ~ 8)
 * @param current_amps: Ŀ����� (��λ������ A)���� 5.5f �� -3.0f
 */
void ESC_Control_Amps_Single(CAN_HandleTypeDef *hcan,const ESC_Config_t* config, uint8_t motor_id, float current_amps){
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

    int16_t raw_val = Amps_To_Raw(config, current_amps);
    Int16_To_BigEndian(raw_val, &data[data_index], &data[data_index + 1]);
    CAN_Send_STD(hcan,can_id, data);
}

/**
 * @brief ����ֵ����һ���� (4��)
 * @param config: �������ָ��
 * @param motor_start_id: ��ʼ��� ID (1 �� 5)
 * @param currents_amps: ����Ϊ 4 �ĸ������飬��Ӧ 4 ������ĵ���
 */
void ESC_Control_Amps_Group(CAN_HandleTypeDef *hcan,const ESC_Config_t* config, uint8_t motor_start_id, float currents_amps[4]) {
    uint8_t data[8] = {0};
    uint32_t can_id = Get_CAN_ID(motor_start_id);
    
    if (can_id == 0) return;

    for (int i = 0; i < 4; i++) {
        int16_t raw_val = Amps_To_Raw(config, currents_amps[i]);
        Int16_To_BigEndian(raw_val, &data[i*2], &data[i*2 + 1]);
    }

    CAN_Send_STD(hcan,can_id, data);
}

///**
// * @brief ����ֵ�������е�� (8��)
// * @param config: �������ָ��
// * @param currents_amps: ����Ϊ 8 �ĸ������飬���� 0 ��Ӧ��� 1
// */
//void ESC_Control_Amps_All(CAN_HandleTypeDef *hcan,const ESC_Config_t* config, float currents_amps[8]) {
//    uint8_t data[8] = {0};

//    // ���͵�һ֡ (��� 1-4)
//    for (int i = 0; i < 4; i++) {
//        int16_t raw_val = Amps_To_Raw(config, currents_amps[i]);
//        Int16_To_BigEndian(raw_val, &data[i*2], &data[i*2 + 1]);
//    }
//    CAN_DJI_SendSTD(ESC_CAN_ID_GROUP_1, data);

//    // ���͵ڶ�֡ (��� 5-8)
//    for (int i = 0; i < 4; i++) {
//        int16_t raw_val = Amps_To_Raw(config, currents_amps[i + 4]);
//        Int16_To_BigEndian(raw_val, &data[i*2], &data[i*2 + 1]);
//    }
//    CAN_Send_STD(hcan,ESC_CAN_ID_GROUP_2, data);
//}

/* ================= ԭʼֵ���ƺ��� ================= */

/**
 * @brief ԭʼֵ���Ƶ������
 * @param motor_id: ��� ID (1 ~ 8)
 * @param raw_value: Э��ԭʼֵ���� 8192 �� -10000
 */
void ESC_Control_Raw_Single(CAN_HandleTypeDef *hcan,uint8_t motor_id, int16_t raw_value) {
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
    CAN_Send_STD(hcan,can_id, data);
}

/**
 * @brief ԭʼֵ����һ���� (4��)
 * @param motor_start_id: ��ʼ��� ID (1 �� 5)
 * @param raw_values: ����Ϊ 4 ���������飬��Ӧ 4 �������ԭʼֵ
 */
void ESC_Control_Raw_Group(CAN_HandleTypeDef *hcan,uint8_t motor_start_id, int16_t raw_values[4]) {
    uint8_t data[8];
    uint32_t can_id = Get_CAN_ID(motor_start_id);
    
    if (can_id == 0) return;

    for (int i = 0; i < 4; i++) {
        Int16_To_BigEndian(raw_values[i], &data[i*2], &data[i*2 + 1]);
    }

    CAN_Send_STD(hcan,can_id, data);
}

///**
// * @brief ԭʼֵ�������е�� (8��)
// * @param raw_values: ����Ϊ 8 ���������飬���� 0 ��Ӧ��� 1
// */
//void ESC_Control_Raw_All(CAN_HandleTypeDef *hcan,int16_t raw_values[8]) {
//    uint8_t data[8];

//    // ���͵�һ֡ (��� 1-4)
//    for (int i = 0; i < 4; i++) {
//        Int16_To_BigEndian(raw_values[i], &data[i*2], &data[i*2 + 1]);
//    }
//    CAN_DJI_SendSTD(ESC_CAN_ID_GROUP_1, data);

//    // ���͵ڶ�֡ (��� 5-8)
//    for (int i = 0; i < 4; i++) {
//        Int16_To_BigEndian(raw_values[i + 4], &data[i*2], &data[i*2 + 1]);
//    }
//    CAN_Send_STD(hcan,ESC_CAN_ID_GROUP_2, data);
//}
