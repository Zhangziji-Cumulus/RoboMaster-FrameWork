#include "bsp_CAN.h"

//** #################################################################################################### **//
//** ============================================ CAN初始化 ============================================= **//
//** #################################################################################################### **//


//** ================================================================================ **//
//** ================================= CAN过滤器接收所有ID =========================== **//
//** ================================================================================ **//

/**
 * @brief  CAN接收所有ID配置函数（标准帧+扩展帧全接收，硬件过滤模式）
 * @param  hcan: CAN外设句柄（&hcan1 / &hcan2）
 * @param  filter_bank: 指定使用的过滤器组（CAN1:0~13，CAN2:14~27，推荐CAN1=0，CAN2=14）
 * @retval HAL_StatusTypeDef: HAL_OK=成功，HAL_ERROR=参数错误/配置失败
 * @note   1. 仅占用1个过滤器组，即可接收所有ID
 *         2. 配置后需确保CAN外设已完成底层初始化（时钟/GPIO/波特率）
 */
HAL_StatusTypeDef CAN_Filter_AcceptAllID(CAN_HandleTypeDef *hcan, uint8_t filter_bank)
{
	// 1. 核心参数校验
    if (hcan == NULL) return HAL_ERROR;

    // 2. 过滤器组合法性校验（严格遵守CAN1/CAN2分配规则）
    if (hcan->Instance == CAN1 && (filter_bank > 13 || filter_bank < 0)) {
        return HAL_ERROR; // CAN1只能用0~13组
    }
    if (hcan->Instance == CAN2 && (filter_bank < 14 || filter_bank > 27)) {
        return HAL_ERROR; // CAN2只能用14~27组
    }
	// 3. 配置CAN过滤器（掩码模式+全0掩码=接收所有ID）
	CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
	
    can_filter_st.SlaveStartFilterBank = filter_bank;
    can_filter_st.FilterBank = filter_bank;

    // 4. 应用过滤器配置
    if (HAL_CAN_ConfigFilter(hcan, &can_filter_st) != HAL_OK) {
        return HAL_ERROR;
    }

    // 5. 启动CAN外设（确保CAN处于工作状态）
    if (HAL_CAN_Start(hcan) != HAL_OK) {
            return HAL_ERROR;
    }
    
    // 6. 开启FIFO0接收中断（可选，推荐开启，提升实时性）
    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

void can_filter_init(void)
{
    CAN_FilterTypeDef can_filter_st;

    // 1. 公共配置部分
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    
    // 【关键】设置过滤器分组：过滤器 0-13 归 CAN1，14-27 归 CAN2
    // 这个参数只需要设置一次，且必须通过 hcan1 设置
    can_filter_st.SlaveStartFilterBank = 14;

    // 2. 配置 CAN1 的过滤器 (Filter 0)
    can_filter_st.FilterBank = 0;
    // 注意：传 &hcan1
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);

    // 3. 配置 CAN2 的过滤器 (Filter 14)
    can_filter_st.FilterBank = 14;
    // 【修正重点】这里依然要传 &hcan1！！！
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);

    // 4. 启动 CAN1 和 CAN2 (建议过滤器配置完再统一启动)
    HAL_CAN_Start(&hcan1);
    HAL_CAN_Start(&hcan2);

    // 5. 开启中断
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

//** #################################################################################################### **//
//** =========================================== CAN发送函数 ============================================ **//
//** #################################################################################################### **//

//** ================================================================================ **//
//** =================================== 标准帧CAN发送函数 =========================== **//
//** ================================================================================ **//

/**
 * @brief 标准帧CAN发送函数
 * 
 * @param hcan CAN句柄指针
 * @param id   标准帧ID，11位有效
 * @param data 指向8字节数据缓冲区的指针
 */
void CAN_Send_STD(CAN_HandleTypeDef *hcan,uint32_t id, uint8_t* data) {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;

    // 1. 配置发送帧头
    TxHeader.StdId = id;              // 标准帧 ID
    TxHeader.IDE = CAN_ID_STD;        // 标准帧
    TxHeader.RTR = CAN_RTR_DATA;      // 数据帧
    TxHeader.DLC = 8;                 // 数据长度 8 字节
    TxHeader.TransmitGlobalTime = DISABLE;

    // 2. 等待发送邮箱空闲 (防止发送过快导致丢包)
    // 如果你的控制频率非常高 (如 >1kHz)，建议去掉这个 while 循环，改用中断发送
    while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        // 死等直到有空闲邮箱，或者你可以加一个超时退出机制
    }
		
    // 3. 添加到发送邮箱并发送
    if (HAL_CAN_AddTxMessage(hcan, &TxHeader, data, &TxMailbox) != HAL_OK) {
        // 发送失败，可以在此处添加错误指示灯闪烁等逻辑
        return;
    }
}


//** ================================================================================ **//
//** ======================= 带帧序号的CAN发送函数（使用扩展帧）======================= **//
//** ================================================================================ **//

/**
 * @brief 将浮点数转换为字节数组
 * 
 * @param value 
 * @param bytes 
 */
static void FloatToBytes(float value, uint8_t* bytes) {
    uint32_t* intValue = (uint32_t*)&value;
    bytes[0] = (*intValue >> 0) & 0xFF;
    bytes[1] = (*intValue >> 8) & 0xFF;
    bytes[2] = (*intValue >> 16) & 0xFF;
    bytes[3] = (*intValue >> 24) & 0xFF;
}

/**
 * @brief 将字节数组转换回浮点数
 * 
 * @param bytes 
 * @return float 
 */
static float BytesToFloat(uint8_t* bytes) {
    uint32_t intValue = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
    return *((float*)&intValue);
}



/**
 * @brief 一个发送浮点数数组的CAN发送函数，使用扩展帧，并在ID中携带帧序号
 * 
 * @param hcan CAN句柄指针
 * @param data 浮点数据数组指针
 * @param length 浮点数据个数，最大64位
 * @param ID    最大64位ID
 * @return bool发送是否成功
 */
bool CAN_SendFloatArray(CAN_HandleTypeDef* hcan, float* data, uint8_t length,uint16_t ID) { 
	
	if(length > 64 ){length = 64;}
	
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];
    uint32_t TxMailbox;
	
	if (length % 2 != 0) {
		length = length + 1; 
    } 
	
    uint8_t frames = (length) / 2;  /* 计算需要的帧数 */
    
	  TxHeader.StdId = 0;       
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = CAN_ID_EXT;
    TxHeader.TransmitGlobalTime = DISABLE;
    
    for (uint8_t i = 0; i < frames + 1; i++) {
        memset(TxData, 0, 8);
        
		if(i == 0)
		{
			//TxData[0] = length;
			TxData[0] = frames;//总帧数
		}
		else if(i > 0)
		{
			/* 填充浮点数数据 */
			if ((i-1) * 2 < length) {
				FloatToBytes(data[(i-1) * 2], &TxData[0]);
			}
			if ((i-1) * 2 + 1 < length) {
				FloatToBytes(data[(i-1) * 2 + 1], &TxData[4]);
			}
		}
		
        /* 使用ID的低5位作为帧序号*/
        TxHeader.ExtId = ID + i;
        TxHeader.DLC = 8;
        
        if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
            Error_Handler();
        }
        
        /* 等待发送完成 */
        uint32_t timeout = 1000;
        while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && timeout-- > 0);
        if (timeout == 0) {
            Error_Handler();
        }
				//return 1;
    }
}
