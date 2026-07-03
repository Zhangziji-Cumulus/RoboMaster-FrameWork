//#include "PlayerUI.h"



//void PlayerUI_SetSelfID(uint16_t robot_id)
//{
//    g_robot_self_id = robot_id;
//}

//void PlayerUI_SetIsSentryRadar(uint8_t is_sentry_radar)
//{
//    g_is_sentry_radar = is_sentry_radar;
//}

//void PlayerUI_TxCompleteCallback(void)
//{
//    g_tx_busy = 0;
//}

// static UBaseType_t remain_PlyerUITask;
// __attribute__((used)) void PlyerUITask(void *argument)
// {
//     osDelay(500);
//     InterQueue_Init();
//     DrawUI_Init();

//     static uint16_t i = 0;

//     for (;;)
//     {
//         if(i>100)
//         {
//             i=0;
//             DrawUI_PowerDisplay(i);
//         }

//         i++;

//         uint32_t tick = osKernelGetTickCount();
//         Inter_Schedule_Run(tick);
//         osDelay(33);
//         remain_PlyerUITask = uxTaskGetStackHighWaterMark(NULL);
//     }
// }
