#include "PlayerUI.h"
#include "ui.h"


// static UBaseType_t remain_PlyerUITask;
// __attribute__((used)) void PlyerUITask(void *argument)
// {

//     ui_init_Spin();

//     static uint16_t i = 0;

//     for(;;)
//     {
//         if(i > 100)
//         {
            
//             i = 0;
//         }

//         ui_Spin_Ungroup_Power->number = i;

//         i++;
        
//         ui_update_Spin();
//         //=============================检测剩余栈=================================//
//         remain_PlyerUITask = uxTaskGetStackHighWaterMark(NULL);
//         osDelay(100);
//     }
// }