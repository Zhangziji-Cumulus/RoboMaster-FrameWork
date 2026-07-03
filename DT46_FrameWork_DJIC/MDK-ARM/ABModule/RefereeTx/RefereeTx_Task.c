// #include "RefereeTx_Task.h"



// void Draw_CenterCircle_Test(void)
// {
//     DrawFigure_t center_circle = {0};

//     // 图形唯一标识
//     center_circle.name[0] = 'C';
//     center_circle.name[1] = '0';
//     center_circle.name[2] = '1';

//     center_circle.op_type    = OP_ADD;       // 新增图形
//     center_circle.fig_type   = FIG_CIRCLE;   // 圆形
//     center_circle.layer      = 0;            // 图层0
//     center_circle.color      = COLOR_SELF;   // 己方队伍颜色
//     center_circle.line_width = 10;            // 线条粗细3像素

//     // 圆心：屏幕正中间 960, 540
//     center_circle.x0 = 960.0f;
//     center_circle.y0 = 540.0f;

//     // 圆形半径
//     center_circle.param_a = 100.0f;

//     // 圆形无x1/y1参数，置0即可
//     center_circle.x1 = 0;
//     center_circle.y1 = 0;

//     // 发送给红3选手端 0x0103
//     Draw_SingleFigure(&center_circle, 0x0103);
// }

// static UBaseType_t remain_PlyerUITask;
// __attribute__((used)) void PlyerUITask(void *argument)
// {
//     uint32_t tick_ms = 0;
//     TickType_t xLastWakeTime;

//     // 获取当前系统滴答基准，用于固定周期延时
//     xLastWakeTime = xTaskGetTickCount();

//     g_robot_self_id = 3;
//     g_is_sentry_radar = 0;

//     // 初始化交互队列（全局FIFO）
//     InterQueue_Init();

    

//     for(;;)
//     {
//         // ===================== 1、监控任务剩余栈空间 =====================
//         remain_PlyerUITask = uxTaskGetStackHighWaterMark(NULL);
        
//         // 获取当前系统毫秒tick（FreeRTOS tick转ms）
//         tick_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;

//         // ===================== 2、带宽窗口刷新 + 交互包调度（核心33ms周期） =====================
//         Inter_Bandwidth_Tick_Update(tick_ms);
//         Inter_Schedule_Run(tick_ms);

//         // ===================== 3、示例：业务绘图/发消息（可放到其他任务，此处仅演示） =====================
// #if 1

//         Draw_CenterCircle_Test();

// #endif

//         // 固定100ms周期循环，内部调度函数会自动限制30Hz(33ms)发包频率
//         vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
//     }
// }
