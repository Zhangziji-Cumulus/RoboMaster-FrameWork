#include "User_CanCallBack.h"

//** ####################################### **//
//** ================= 串口 ================= **//
//** ####################################### **//

//extern UART_HandleTypeDef huart3; // 确保引用你的串口句柄
//extern uint8_t sbusRxBuffer[];
//extern SBUS_Data_t sbusData;

//// 重写DMA接收完成回调
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == USART3) {
//        // DMA刚刚填满缓冲区，此时缓冲区里就是一帧完整的数据
//        // 我们只做一件事：标记有新数据
//        // 具体的解析工作留给 main loop 去做，避免阻塞中断
//        sbusData.newDataAvailable = 1;
//    }
//}

//** ########################################### **//
//** ================= CAN总线 ================= **//
//** ########################################## **//

extern float DualBoard_ReceiveDataBuff[64];

static void FloatToBytes(float value, uint8_t* bytes) {
    uint32_t* intValue = (uint32_t*)&value;
    bytes[0] = (*intValue >> 0) & 0xFF;
    bytes[1] = (*intValue >> 8) & 0xFF;
    bytes[2] = (*intValue >> 16) & 0xFF;
    bytes[3] = (*intValue >> 24) & 0xFF;
}

static float BytesToFloat(uint8_t* bytes) {
    uint32_t intValue = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
    return *((float*)&intValue);
}


/* 配置宏定义 */
#define CAN2_MAX_DATA_FRAMES  8       /* 最大支持数据帧数(不含第0帧) */
#define CAN_FRAME_DATA_SIZE   8       /* 每帧有效数据字节数 */
#define CAN_RX_TIMEOUT_MS     1000    /* 接收超时阈值(ms)，防丢帧卡死 */

/* 接收状态结构体(替代散乱的static变量) */
typedef struct {
    uint8_t  totalFrames;             /* 期望接收的总数据帧数 */
    uint32_t receivedMask;            /* 接收位掩码(bit0对应frameId=1) */
    uint8_t  buffer[CAN2_MAX_DATA_FRAMES * CAN_FRAME_DATA_SIZE + 8]; /* +8兼容原偏移逻辑 */
    uint32_t lastRxTick;             /* 最近一帧到达时间戳 */
    bool     isReceiving;            /* 接收状态机标志 */
} Can2RxState_t;

static Can2RxState_t g_can2RxState = {0};

/**
  * @brief  CAN 接收回调函数 (替换原 HAL_CAN_RxFifo0MsgPendingCallback 中的逻辑)
  */
void CAN_RxProcess(CAN_HandleTypeDef *hcan,CAN_RxHeaderTypeDef *pHeader, uint8_t RxData[])
{
//    CAN_RxHeaderTypeDef RxHeader;
//    uint8_t RxData[8] = {0};

//    /* 1. 安全读取消息，失败直接返回(不阻塞中断) */
//    if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
//        return;
//    }

    /* 2. 解析扩展ID */
    uint32_t baseId  = pHeader->ExtId & 0xFFFFFFE0; /* 高27位基ID */
    uint32_t frameId = pHeader->ExtId & 0x1F;       /* 低5位帧序号 */

    if (baseId != RX_BASE_ID) {
        return; /* 非目标设备数据，丢弃 */
    }

    /* 3. 超时保护：防止丢帧导致永久卡死 */
    if (g_can2RxState.isReceiving && (HAL_GetTick() - g_can2RxState.lastRxTick > CAN_RX_TIMEOUT_MS)) {
        g_can2RxState.isReceiving = false;
        g_can2RxState.receivedMask = 0;
        /* 可选：在此记录超时日志或触发超时回调 */
    }

    /* 4. 处理第0帧(控制帧) */
    if (frameId == 0) {
        g_can2RxState.totalFrames = RxData[0];
        
        /* 合法性检查 */
        if (g_can2RxState.totalFrames == 0 || g_can2RxState.totalFrames > CAN2_MAX_DATA_FRAMES) {
            g_can2RxState.isReceiving = false;
            return;
        }

        /* 初始化状态机 */
        g_can2RxState.receivedMask = 0;
        g_can2RxState.isReceiving = true;
        g_can2RxState.lastRxTick = HAL_GetTick();
        return; /* 控制帧处理完毕，不继续执行数据逻辑 */
    }

    /* 5. 处理数据帧 (frameId 1 ~ totalFrames) */
    if (!g_can2RxState.isReceiving || frameId > g_can2RxState.totalFrames) {
        return; /* 未处于接收状态或帧号越界 */
    }

    /* 检查是否重复接收 */
    uint32_t frameBit = (1UL << (frameId - 1));
    if (g_can2RxState.receivedMask & frameBit) {
        return; /* 已接收过该帧，直接忽略 */
    }

    /* 安全拷贝数据 (兼容原代码的8字节偏移逻辑) */
    uint32_t bufOffset = frameId * CAN_FRAME_DATA_SIZE; 
    memcpy(&g_can2RxState.buffer[bufOffset], RxData, CAN_FRAME_DATA_SIZE);

    /* 更新状态 */
    g_can2RxState.receivedMask |= frameBit;
    g_can2RxState.lastRxTick = HAL_GetTick();

    /* 6. 检查是否全部接收完成 */
    uint32_t expectedMask = (1UL << g_can2RxState.totalFrames) - 1;
    if ((g_can2RxState.receivedMask & expectedMask) == expectedMask) {
        /* 解析浮点数据 */
        uint32_t floatCount = g_can2RxState.totalFrames * 2; /* 每帧8字节=2个float */
        for (uint32_t i = 0; i < floatCount; i++) {
            /* 原逻辑从 buffer[8] 开始解析，这里保持兼容 */
            DualBoard_ReceiveDataBuff[i] = BytesToFloat(&g_can2RxState.buffer[(i + 2) * 4]);
        }

        /* 重置状态机，准备下一次接收 */
        g_can2RxState.isReceiving = false;
        g_can2RxState.receivedMask = 0;

        /* 触发用户回调 */
        Dual_Board_ReceiveCallBack();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

		if(hcan->Instance == CAN1)
		{
			CAN_RxHeaderTypeDef Temp_RxHeader;
			uint8_t Temp_RxData[8];
			
			while(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
			{
					if (Temp_RxHeader.IDE == CAN_ID_STD)// 标准帧
					{
							// 处理 DJI 电机数据
							CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN1,Temp_RxHeader.StdId, Temp_RxData);
					}
					else if(Temp_RxHeader.IDE == CAN_ID_EXT)// 扩展帧
					{
							CAN_RxProcess(hcan,&Temp_RxHeader, Temp_RxData);
					} 	
				}
		}
    else if (hcan->Instance == CAN2)
    {	
				CAN_RxHeaderTypeDef Temp_RxHeader;
				uint8_t Temp_RxData[8];

				// 循环读空FIFO
				while(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
				{
						if (Temp_RxHeader.IDE == 0)
						{
								// 标准帧 - 电机
								CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN2, Temp_RxHeader.StdId, Temp_RxData);
						}
						else if(Temp_RxHeader.IDE == 4)
						{
								// 扩展帧 - 双板通信 【现在绝对正确！】
								CAN_RxProcess(hcan, &Temp_RxHeader, Temp_RxData);
						}
				}
		}
}
	
