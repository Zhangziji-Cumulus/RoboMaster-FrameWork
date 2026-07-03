#include "PlayerUI.h"



static UBaseType_t remain_PlyerUITask;
__attribute__((used)) void PlyerUITask(void *argument)
{
    osDelay(1000);
	ui_init_g_Ungroup();
    osDelay(1000);
    
    
    for(;;)
    {
        ui_update_g_Ungroup();
        //=============================检测剩余栈=================================//
        remain_PlyerUITask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(100);
    }
}
