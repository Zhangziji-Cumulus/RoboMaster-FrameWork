#include "CMDCenter.h"
#include "A_CommonSystem.h"
#include "INS_task.h"

//** ================================================================================ **//
//** ============================= 定义双板通信数据结构体 ============================ **//
//** ================================================================================ **//


#if(BOARD_MODE == BOARD_MODE_DUAL)

#if(BOARD_ID == GIMBAL_BOARD)

typedef struct{

    CMD_t CMD;
    fp32 INS_angle_Gimbal[3];   // [0]=Yaw, [1]=Roll, [2]=Pitch

    uint8_t LoadStartFlag;

}BoardTransmit_Gimbal_TX_t;

typedef struct{

    uint8_t LoadEndFlag;

}BoardTransmit_Gimbal_RX_t;

extern BoardTransmit_Gimbal_RX_t  BoardGRX;

#endif

#if(BOARD_ID == CHASSIS_BOARD)

typedef struct{

    uint8_t LoadEndFlag;
    
}BoardTransmit_Chassis_TX_t;

typedef struct{

    CMD_t CMD;
    fp32 INS_angle_Gimbal[3];   // [0]=Yaw, [1]=Roll, [2]=Pitch

    uint8_t LoadStartFlag;

}BoardTransmit_Chassis_RX_t;

extern BoardTransmit_Chassis_RX_t  BoardCRX;

#endif

#endif
