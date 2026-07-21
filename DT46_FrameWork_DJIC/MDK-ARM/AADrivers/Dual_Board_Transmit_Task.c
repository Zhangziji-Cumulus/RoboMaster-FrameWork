#include "Dual_Board_Transmit_Task.h"


#if(BOARD_MODE == BOARD_MODE_DUAL)

//** #################################################################################################### **//
//** =========================================== 云台板发送 ============================================= **//
//** #################################################################################################### **//

#if(BOARD_ID == GIMBAL_BOARD)

BoardTransmit_Gimbal_TX_t Tx  = {0};

static UBaseType_t remain_DualBoardTask;
__attribute__((used)) void DualBoardTask(void *argument)
{
    
  for(;;)
  {

	  Tx.CMD = *CMD_Get_point();
    {
        // IMU数据: [0]=Yaw, [1]=Roll, [2]=Pitch (角度度)
        const fp32* imu = IMU_Get_point();
        Tx.INS_angle_Gimbal[0] = imu[0];   // Yaw
        Tx.INS_angle_Gimbal[1] = imu[1];   // Roll
        Tx.INS_angle_Gimbal[2] = imu[2];   // Pitch
    }

    DualBoard_SendStruct(&hcan2,TX_BASE_ID,&Tx,sizeof(Tx));

		//=============================== 剩余栈检测 ===============================//
		remain_DualBoardTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(10);
  }

}

//外部写函数
void Dual_Board_TX_Set_LoadStartFlag(uint8_t flag)
{
    Tx.LoadStartFlag = flag;
}


#endif

//** #################################################################################################### **//
//** =========================================== 地盘板发送 ============================================= **//
//** #################################################################################################### **//

#if(BOARD_ID == CHASSIS_BOARD)

BoardTransmit_Chassis_TX_t Tx  = {0};

  static UBaseType_t remain_DualBoardTask;
__attribute__((used)) void DualBoardTask(void *argument)
{

  for(;;)
  {

    DualBoard_SendStruct(&hcan2,TX_BASE_ID,&Tx,sizeof(Tx));
		//=============================== 剩余栈检测 ===============================//
		remain_DualBoardTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }

}

//外部写函数
void Dual_Board_TX_Set_LoadEndFlag(uint8_t flag)
{
    Tx.LoadEndFlag = flag;
}

#endif

#endif