#include "AutoAim.h"

#if((BOARD_MODE == BOARD_MODE_DUAL && BOARD_ID == GIMBAL_BOARD )|| BOARD_MODE == BOARD_MODE_SINGLE)

#if(AUTOAIM_IFOPEN)

//static 
	AutoAim_Instance_t  AutoAim_Instance;//自瞄实例
//static 
	AutoAim_Ctrl_t AutoAim_Ctrl;//自瞄控制量

//** ================================================================================ **//
//** ================================== 融合算法 ==================================== **//
//** ================================================================================ **//

/**
 * @brief   整型(int16_t)加权融合函数 —— 手动控制量与自瞄控制量的加权融合
 * @param  manual    手动控制量（操作手给定值）
 * @param  auto_val  自瞄控制量（视觉算法输出值）
 * @param  aim_valid 自瞄有效性标志（0=无效/无目标，1=有效/有目标）
 * @param  min_out   输出下限（限幅）
 * @param  max_out   输出上限（限幅）
 * @return int16_t   融合后的控制量
 * @note   当自瞄无效时完全输出手动值；
 *         当自瞄有效时按权重比 AUTOAIM_WEIGHT_AUTO / AUTOAIM_WEIGHT_MANUAL 加权融合；
 *         最终结果做限幅保护，防止溢出。
 */
int16_t AutoAim_WeightFusion_Int16(int16_t manual, int16_t auto_val, uint8_t aim_valid, int16_t min_out, int16_t max_out)
{
    int32_t output;

    if (aim_valid == 0)
    {
        output = manual;
    }
    else
    {
        output =  ((int32_t)auto_val * AUTOAIM_WEIGHT_AUTO +
                   (int32_t)manual  * AUTOAIM_WEIGHT_MANUAL) / 100;
    }

    if (output > max_out) output = max_out;
    if (output < min_out) output = min_out;

    return (int16_t)output;
}

/**
 * @brief   浮点(float)加权融合函数 —— 手动控制量与自瞄控制量的加权融合
 * @param  manual    手动控制量（操作手给定值）
 * @param  auto_val  自瞄控制量（视觉算法输出值）
 * @param  aim_valid 自瞄有效性标志（0=无效/无目标，1=有效/有目标）
 * @param  min_out   输出下限（限幅）
 * @param  max_out   输出上限（限幅）
 * @return float     融合后的控制量
 * @note   与 Int16 版本逻辑一致，但使用浮点运算避免精度损失；
 *         先乘权重再除以100，等价于加权平均。
 */
float AutoAim_WeightFusion_Float(float manual, float auto_val, uint8_t aim_valid, float min_out, float max_out)
{
    float output;

    // if (aim_valid == 0)
    // {
    //     output = manual;
    // }
    // else
    // {
        output =  auto_val * AUTOAIM_WEIGHT_AUTO +
                  manual  * AUTOAIM_WEIGHT_MANUAL;
        output /= 100.0f;
    //}

    if (output > max_out)
        output = max_out;
    if (output < min_out)
        output = min_out;

    return output;
}

/**
 * @brief   获取自瞄控制量结构体指针
 * @return const AutoAim_Ctrl_t*  自瞄控制量（Yaw/Pitch/FireOK/IsOnline）的指针
 * @note    Yaw/Pitch 为视觉给出的期望角度，FireOK 为开火允许标志，
 *         IsOnline 表示视觉上位机是否在线。
 *         外部模块通过此接口只读访问自瞄控制量，无需直接操作本文件内部变量。
 */

//** ================================================================================ **//
//** ========================== 获取自瞄控制数据（对外） ============================= **//
//** ================================================================================ **//
const AutoAim_Ctrl_t* AutoAim_Ctrl_Get_point(void)
{
    return &AutoAim_Ctrl;
}

//** ------------------------------------------------------------ **//
//** ======================== 初始化通信 ======================== **//
//** ------------------------------------------------------------ **//
void AutoAim_Init(void)
{
    __HAL_UART_ENABLE_IT(&AUTO_USART_HANDLE, UART_IT_IDLE);

    AutoAim_Instance.Rx_ActiveBuf = 0;
    // DMA循环模式绑定第一个缓冲，硬件自动循环切换
    HAL_UART_Receive_DMA(&AUTO_USART_HANDLE, (uint8_t *)&AutoAim_Instance.Rx_Buf[AutoAim_Instance.Rx_ActiveBuf], sizeof(AutoAim_Rx_t));

    AutoAim_Instance.Tx_Done = 1;
    AutoAim_Instance.Tx.Frame_head = AUTO_USART_HEADER;
    AutoAim_Instance.Tx.Enemy_Color = AUTOAIM_ENEMY_COLOR;
}

//** ====================== 更新要发送的数据 ====================== **//
void AutoAim_UpdateTx(void)
{
    AutoAim_Instance.MCUData.INS_angle =  IMU_Get_point();
    AutoAim_Instance.Tx.IMU_Roll = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_ROLL];
    AutoAim_Instance.Tx.IMU_Pitch = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_PITCH];
    AutoAim_Instance.Tx.IMU_Yaw = AutoAim_Instance.MCUData.INS_angle[IMU_INDEX_YAW];

    //AutoAim_Instance.Tx.Match = 999;
}

//** ===================== 普通模式发送函数 ====================== **//
void AutoAim_SendData(void)
{
    HAL_UART_Transmit_DMA(&AUTO_USART_HANDLE, (uint8_t*)&AutoAim_Instance.Tx, sizeof(AutoAim_Tx_t));
}

//** ------------------------------------------------------------ **//
//** ======================== 接收解析函数 ======================= **//
//** ------------------------------------------------------------ **//

void AutoAim_ReceiveProcess(void)
{
    AutoAim_Rx_t *rx_buf = &AutoAim_Instance.Rx_ParseBuf;

    // 帧头校验
    if(rx_buf->Frame_head != AUTO_USART_HEADER)
    {
        AutoAim_Instance.Rx_OnlineFlag = 0;
        return;
    }

    AutoAim_Instance.Rx = *rx_buf;
    AutoAim_Instance.Rx_OnlineFlag = 1;
    AutoAim_Instance.Rx_LastTick = HAL_GetTick();
}

void AutoAim_UpdateRx(void)
{
    // 超时检测：超过 AUTOAIM_RX_TIMEOUT_MS 未收到有效帧 → 置离线
    if (HAL_GetTick() - AutoAim_Instance.Rx_LastTick > AUTOAIM_RX_TIMEOUT_MS)
    {
        AutoAim_Instance.Rx_OnlineFlag = 0;
    }
    
    if(AutoAim_Instance.Rx_OnlineFlag)
    {
        AutoAim_Ctrl.Yaw = (AutoAim_Instance.Rx.Yaw * 0.1);
        AutoAim_Ctrl.Pitch = (AutoAim_Instance.Rx.Pitch * 0.1);
        AutoAim_Ctrl.FireOK = AutoAim_Instance.Rx.Fire;
        AutoAim_Ctrl.IsOnline = 1;
        AutoAim_Ctrl.RxTick = AutoAim_Instance.Rx_LastTick;
    }
    else
    {
        AutoAim_Ctrl.Yaw = 0;
        AutoAim_Ctrl.Pitch = 0;
        AutoAim_Ctrl.FireOK = 0;
        AutoAim_Ctrl.IsOnline = 0;
    }
}

//** ------------------------------------------------------------ **//
//** ======================== 中断接收函数 ======================= **//
//** ------------------------------------------------------------ **//
void AutoAim_UART_IRQHandler(void)
{
    HAL_UART_IRQHandler(&AUTO_USART_HANDLE);

    if (__HAL_UART_GET_FLAG(&AUTO_USART_HANDLE, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&AUTO_USART_HANDLE);

        // 1. 把当前DMA写完的整帧拷贝到解析缓存（DMA此时在另一个缓冲继续存数据，不会被覆写）
        memcpy(&AutoAim_Instance.Rx_ParseBuf, &AutoAim_Instance.Rx_Buf[AutoAim_Instance.Rx_ActiveBuf], sizeof(AutoAim_Rx_t));

        // 2. 切换DMA目标缓冲，硬件自动切换，不再手动启停DMA
        AutoAim_Instance.Rx_ActiveBuf ^= 1U;
        HAL_UART_Receive_DMA(&AUTO_USART_HANDLE, (uint8_t *)&AutoAim_Instance.Rx_Buf[AutoAim_Instance.Rx_ActiveBuf], sizeof(AutoAim_Rx_t));

        // 3. 解析锁定好的数据
        AutoAim_ReceiveProcess();
    }
}

#endif

#endif
