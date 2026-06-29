#include "User_CallBack.h"

//** ####################################### **//
//** ================= 串口 ================= **//
//** ####################################### **//

//** ================================================================================ **//
//** ============================= 串口发送回调函数 ================================== **//
//** ================================================================================ **//

// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart == &huart1)
//     {
//         AutoAim_TxCpltCallback();
//     }
// }

//** ########################################### **//
//** ================= CAN总线 ================= **//
//** ########################################## **//

// 全局变量存储接收到的数据
#if (BOARD_MODE == BOARD_MODE_DUAL)

#if(BOARD_ID == GIMBAL_BOARD)

static uint8_t g_dataValid = 0;

BoardTransmit_Gimbal_RX_t   BoardGRX;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef Temp_RxHeader;
    uint8_t Temp_RxData[8];

#if defined(CAN1)
    if (hcan->Instance == CAN1)
    {
        while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
        {
            if (Temp_RxHeader.IDE == CAN_ID_STD)
            {
                CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN1, Temp_RxHeader.StdId, Temp_RxData);
            }
            else if (Temp_RxHeader.IDE == CAN_ID_EXT)
            {
                if (DualBoard_ParseStruct(&Temp_RxHeader, Temp_RxData, RX_BASE_ID, &BoardGRX, sizeof(BoardGRX)))
                {
                    g_dataValid = 1;
                }
            }

        }
    }
#endif

#if defined(CAN2)
    if (hcan->Instance == CAN2)
    {
        while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
        {
            if (Temp_RxHeader.IDE == CAN_ID_STD)
            {
                CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN2, Temp_RxHeader.StdId, Temp_RxData);
            }
            else if (Temp_RxHeader.IDE == CAN_ID_EXT)
            {
                if (DualBoard_ParseStruct(&Temp_RxHeader, Temp_RxData, RX_BASE_ID, &BoardGRX, sizeof(BoardGRX)))
                {
                    g_dataValid = 1;
                }

                CAN_ZDT_Motor_FeedBack(&ZDT_FeedBack,1,&Temp_RxHeader,Temp_RxData);
            }
        }
    }
#endif

#if defined(CAN3)
    if (hcan->Instance == CAN3)
    {
        while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
        {
            if (Temp_RxHeader.IDE == CAN_ID_STD)
            {
                CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN3, Temp_RxHeader.StdId, Temp_RxData);
            }
            else if (Temp_RxHeader.IDE == CAN_ID_EXT)
            {
                if (DualBoard_ParseStruct(&Temp_RxHeader, Temp_RxData, RX_BASE_ID, &BoardGRX, sizeof(BoardGRX)))
                {
                    g_dataValid = 1;
                }
            }
        }
    }
#endif
}
#endif

#if(BOARD_ID == CHASSIS_BOARD)

static uint8_t g_dataValid = 0;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef Temp_RxHeader;
    uint8_t Temp_RxData[8];

#if defined(CAN1)
    if (hcan->Instance == CAN1)
    {
        while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
        {
            if (Temp_RxHeader.IDE == CAN_ID_STD)
            {
                CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN1, Temp_RxHeader.StdId, Temp_RxData);
            }
            else if (Temp_RxHeader.IDE == CAN_ID_EXT)
            {
                if (DualBoard_ParseStruct(&Temp_RxHeader, Temp_RxData, RX_BASE_ID, &BoardCRX, sizeof(BoardCRX)))
                {
                    g_dataValid = 1;
                }
            }

        }
    }
#endif

#if defined(CAN2)
    if (hcan->Instance == CAN2)
    {
        while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
        {
            if (Temp_RxHeader.IDE == CAN_ID_STD)
            {
                CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN2, Temp_RxHeader.StdId, Temp_RxData);
            }
            else if (Temp_RxHeader.IDE == CAN_ID_EXT)
            {
                if (DualBoard_ParseStruct(&Temp_RxHeader, Temp_RxData, RX_BASE_ID, &BoardCRX, sizeof(BoardCRX)))
                {
                    g_dataValid = 1;
                }
            }
        }
    }
#endif

#if defined(CAN3)
    if (hcan->Instance == CAN3)
    {
        while (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Temp_RxHeader, Temp_RxData) == HAL_OK)
        {
            if (Temp_RxHeader.IDE == CAN_ID_STD)
            {
                CAN_DJI_Motor_Feedback(DJI_MFeedback_CAN3, Temp_RxHeader.StdId, Temp_RxData);
            }
            else if (Temp_RxHeader.IDE == CAN_ID_EXT)
            {
                if (DualBoard_ParseStruct(&Temp_RxHeader, Temp_RxData, RX_BASE_ID, &BoardCRX, sizeof(BoardCRX)))
                {
                    g_dataValid = 1;
                }
            }
        }
    }
#endif
}
#endif

#endif