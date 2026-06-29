#include "MotorCtrl_DJI_Check_Task.h"
void DJIMotorCheckTask(void *argument)
{
  for(;;)
  {
    #if defined(CAN1)
    CAN_DJI_Motor_CheckOnline(DJI_MFeedback_CAN1);
    #endif

    #if defined(CAN2)
    CAN_DJI_Motor_CheckOnline(DJI_MFeedback_CAN2);
    #endif

    #if defined(CAN3)
    CAN_DJI_Motor_CheckOnline(DJI_MFeedback_CAN3);
    #endif
    osDelay(1);
  }
}
