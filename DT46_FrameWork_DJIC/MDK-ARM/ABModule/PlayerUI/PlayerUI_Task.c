#include "PlayerUI_Task.h"

static UBaseType_t remain_PlyerUITask;
__attribute__((used)) void PlyerUITask(void *argument)
{
	DrawUI_Init();

    osDelay(100);

    for(;;)
    {
        DrawUI_Update();
        //=============================检测剩余栈=================================//
        remain_PlyerUITask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(30);
    }
}
