#include "Printf_Task.h"

#include "dm_motor_drv.h"
#include "dm_motor_ctrl.h"

//extern float SpinSpeed;
float testspeed = 10;

static UBaseType_t remain_PrintfTask;
__attribute__((used)) void PrintfTask(void *argument)
{
  Printf_Init();

	dm_motor_init();

  osDelay(10);
	write_motor_data(motor[Motor1].id, 10, 1, 0, 0, 0);
  osDelay(10);
	save_motor_data(motor[Motor1].id, 10);
  osDelay(10);
	dm_motor_enable(&hcan1, &motor[Motor1]);

  static float testangle;
  

  for(;;)
  {
    testangle += 15.0;

    //pos_ctrl(&hcan1,motor[Motor1].id,testangle,testspeed);
    spd_ctrl(&hcan1,1,testspeed);
    //Printf("Float: %.2f\r\n", SpinSpeed); 

    //=============================检测剩余栈=================================//
	  remain_PrintfTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}


