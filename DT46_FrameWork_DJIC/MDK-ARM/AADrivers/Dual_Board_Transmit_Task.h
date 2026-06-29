#ifndef DUAL_BOARD_TRANSMIT_TASK_H_
#define DUAL_BOARD_TRANSMIT_TASK_H_

#include "Dual_Board_Transmit.h"
#include "CMDCenter.h"

#if(BOARD_ID == GIMBAL_BOARD)
//外部写函数
void Dual_Board_TX_Set_LoadStartFlag(uint8_t flag);
#endif

#if(BOARD_ID == CHASSIS_BOARD)
//外部写函数
void Dual_Board_TX_Set_LoadEndFlag(uint8_t flag);
#endif

#endif // DUAL_BOARD_TRANSMIT_TASK_H_