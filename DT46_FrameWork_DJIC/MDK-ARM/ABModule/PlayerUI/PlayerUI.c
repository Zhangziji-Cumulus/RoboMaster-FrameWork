#include "PlayerUI.h"





static UBaseType_t remain_PlyerUITask;
__attribute__((used)) void PlyerUITask(void *argument)
{


		DrawUI_Init();

    static uint16_t i = 0;

    for(;;)
    {
        if(i > 100)
        {
            
            i = 0;
            
        }

        if(i%10 == 0)
        {
            ui_testAPI_AutoAim_AutoAimFlag->color = 8;
            ui_testAPI_AutoAim_AutoAimFlag->width = 3;
            ui_testAPI_AutoAim_AutoAimFlag->r = 200;
        }
        else
        {
            ui_testAPI_AutoAim_AutoAimFlag->color = 4;
            ui_testAPI_AutoAim_AutoAimFlag->width = 5;
            ui_testAPI_AutoAim_AutoAimFlag->r = 150;
        }

        ui_Spin_Ungroup_Power->number = i;

        i++;

        DrawUI_Update();

        //=============================检测剩余栈=================================//
        remain_PlyerUITask = uxTaskGetStackHighWaterMark(NULL);
        osDelay(100);
    }
}