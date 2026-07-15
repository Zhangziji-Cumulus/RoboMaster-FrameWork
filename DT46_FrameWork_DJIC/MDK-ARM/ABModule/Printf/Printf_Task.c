#include "Printf_Task.h"

//extern float SpinSpeed;

static UBaseType_t remain_PrintfTask;
__attribute__((used)) void PrintfTask(void *argument)
{
  Printf_Init();

  for(;;)
  {

    //Printf("Float: %.2f\r\n", SpinSpeed); 

    //=============================检测剩余栈=================================//
	  remain_PrintfTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(100);
  }
}


