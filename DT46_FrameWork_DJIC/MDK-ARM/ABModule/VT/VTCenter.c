#include "VTCenter.h"

#if(BOARD_ID == GIMBAL_BOARD)

// 定义接收缓冲区，大小严格匹配协议帧长
static uint8_t vt03_rx_buf[RC_FRAME_SIZE];

/**
 * @brief 初始化 VT03 串口 DMA 接收
 */
void VT03_DMA_Init(void)
{
    // 启动第一次 DMA 接收，固定接收 21 字节
    HAL_UART_Receive_DMA(&huart6, vt03_rx_buf, RC_FRAME_SIZE);
}

VideoTx_Ctrl_t RX_data = {0};

/**
 * @brief UART 接收完成回调函数（由 HAL 库在中断中自动调用）
 * @note  在普通模式下，每次收满 21 字节触发一次
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 1. 确认是串口6的接收完成中断
    if (huart->Instance == USART6) 
    {
        // 2. 获取解析后的数据结构指针
        //VideoTx_Ctrl_t *rc_data = (VideoTx_Ctrl_t *)get_VideoTx_Ctl_point();
        
        // 3. 调用你提供的协议解析函数（内部包含帧头校验和CRC校验）
        VT_ParseFrame(vt03_rx_buf, &RX_data);
        
        // 4. 【关键】DMA 普通模式下，接收完成后 DMA 会自动停止，
        //    必须在这里重新启动下一次 21 字节的接收，否则将不再接收数据
        HAL_UART_Receive_DMA(&huart6, vt03_rx_buf, RC_FRAME_SIZE);
    }
}

static UBaseType_t remain_VTTask;
__attribute__((used)) void VTTask(void *argument)
{
     VT03_DMA_Init(); 

    uint8_t temp_raw[RC_FRAME_SIZE];

    for(;;)
    {

        remain_VTTask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(1);
    }
}

#endif /* BOARD_ID == GIMBAL_BOARD */