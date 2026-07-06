#include "DrawUI.h"


static const referee_all_data_t* Referee_DrawUI;
static const float* INS_angle_DrawUI;
static const CMD_t* CMD_DrawUI;


//使用这个函数来更新交互、数据
static void Update_Data(void);
static void DrawUI_Dynamic(void);
static void DrawUI_ReInit(uint32_t timems);

void DrawUI_Init(void)
{
    osDelay(1000);

    //获取机器人ID
    Referee_DrawUI = Referee_GetData();
    ui_self_id = Referee_DrawUI->_robot_status.robot_id; 

    //初始化UI

    //初始并跟新静态UI
    /*
        注：静态UI init 之间需要适当间隔，否则裁判系统处理不过来
        字符串占用带宽是其他元素的七倍，也就是一个字符串相当于7个其他元素
        所以字符串尽量是静态的
    */

    ui_init_Static_Text1();
    osDelay(30);
    ui_init_Static_Text2();
    osDelay(30);
    ui_init_Static_Text3();
    osDelay(30);
    ui_init_Static_Text4();
    osDelay(30);
    ui_init_Static_Text5();
    osDelay(30);
    ui_init_Static_Graphic();
    osDelay(50);

    //初始化动态UI
    ui_init_Dynamic();
    osDelay(50);
}

void DrawUI_Update(void)
{
    //更新动态数据
    DrawUI_Dynamic();

    ui_update_Dynamic();

    //自动重初始化UI
    DrawUI_ReInit(2000);

}

//重新初始化UI手动触发、自动触发（每隔 timems 毫秒执行一次，基于 HAL 滴答）
static void DrawUI_ReInit(uint32_t timems)
{
    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();

    if (now - last_tick < timems)
        return;
    last_tick = now;

    //获取裁判系统数据
    Referee_DrawUI = Referee_GetData();
    //自动获取当前机器人ID,发送和接受要对应上UI才能上去
    ui_self_id = Referee_DrawUI->_robot_status.robot_id; 
    
    // osDelay(30);
    // ui_init_Static_Text1();
    // osDelay(30);
    // ui_init_Static_Text2();
    // osDelay(30);
    // ui_init_Static_Text3();
    // osDelay(30);
    // ui_init_Static_Text4();
    // osDelay(30);
    // ui_init_Static_Text5();
    // osDelay(30);
    // ui_init_Static_Graphic();
    // osDelay(200);

    ui_init_Dynamic();
    
    //osDelay(50);
}

//使用这个函数来更新交互、数据
static void Update_Data(void)
{
    //获取裁判系统数据
    Referee_DrawUI = Referee_GetData();
    //获取地盘控制板姿态
    INS_angle_DrawUI = IMU_Get_point();
    //获取控制指令
    CMD_DrawUI =  CMD_Get_point();
} 

//使用这个函数来更新动态值
static void DrawUI_Dynamic(void)
{
    Update_Data();

    float Yaw = INS_angle_DrawUI[0];
    float Pitch = INS_angle_DrawUI[2];

    uint32_t now = HAL_GetTick();       /* 统一使用硬件tick，不受FreeRTOS调度影响 */

    //绘制自瞄状态标志
    {
        uint32_t phase = now % 1000;
        if (phase < 200) {
            ui_Dynamic_Fire_AutoAimLock->color = 2;
            ui_Dynamic_Fire_AutoAimLock->width = 3;
            ui_Dynamic_Fire_AutoAimLock->r = 200;
        } else {
            ui_Dynamic_Fire_AutoAimLock->color = 0;
            ui_Dynamic_Fire_AutoAimLock->width = 5;
            ui_Dynamic_Fire_AutoAimLock->r = 150;
        }
    }

    //绘制地盘方向
    {
        float out_x, out_y;
        RotateLineUI(
            ui_Dynamic_Chassis_ChassisFront->start_x,
            ui_Dynamic_Chassis_ChassisFront->start_y,
            50,
            Yaw,
            &out_x,
            &out_y
        );
        ui_Dynamic_Chassis_ChassisFront->end_x = out_x;
        ui_Dynamic_Chassis_ChassisFront->end_y = out_y;
    }

    //绘制云台角度
    {
        float out_x, out_y;
        PitchLineUI(
            ui_Dynamic_Gimbal_PitchLine->start_x,
            ui_Dynamic_Gimbal_PitchLine->start_y,
            50,
            Pitch,
            &out_x,
            &out_y
        );
        ui_Dynamic_Gimbal_PitchLine->end_x = out_x;
        ui_Dynamic_Gimbal_PitchLine->end_y = out_y;
    }

    //显示云台角度（Pitch为弧度，先转角度，再转定点数，如12.345°→12345）
    ui_Dynamic_Gimbal_PitchAngle->number = (int32_t)(Pitch * 180.0f / (float)M_PI * 1000.0f);
    

    //绘制开火标志位
    {
        uint32_t phase = now % 2000;
        if (phase < 1000) {
            ui_Dynamic_Fire_FireFlag->color = 1;
            ui_Dynamic_Fire_AutoAimFlag->color = 2;
            ui_Dynamic_Fire_LoadFlag->color = 3;
            ui_Dynamic_Fire_SpinFlag ->color = 4;

        } else {
            ui_Dynamic_Fire_FireFlag->color = 8;
            ui_Dynamic_Fire_AutoAimFlag->color = 8;
            ui_Dynamic_Fire_LoadFlag->color = 8;
            ui_Dynamic_Fire_SpinFlag ->color = 8;
        }
    }

    //绘制超电容量：2.1s 锯齿波 0→100
    {
        float powertest = (float)(now % 2100) * 100.0f / 2100.0f;
        ui_Dynamic_Chassis_SuperPower->start_angle = LerpAngle(
            ui_Dynamic_Chassis_SuperPower->end_angle,
            -60.0f,
            powertest,
            1);
    }
}


