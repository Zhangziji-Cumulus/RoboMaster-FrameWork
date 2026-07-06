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
    ui_init_static_Ungroup();
    osDelay(100);
    ui_update_static_Ungroup();
    osDelay(100);
    ui_init_ShootingFrame();

}

void DrawUI_Update(void)
{
    //更新动态数据
    DrawUI_Dynamic();

    //更新UI
    //ui_update_static_Ungroup();

    ui_update_ShootingFrame();

    //自动重初始化UI
    DrawUI_ReInit(5000);

}

//重新初始化UI手动触发、自动触发（每隔 timems 毫秒执行一次，基于 HAL 滴答）
static void DrawUI_ReInit(uint32_t timems)
{
    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();

    if (now - last_tick < timems)
        return;
    last_tick = now;

    Referee_DrawUI = Referee_GetData();
    ui_self_id = Referee_DrawUI->_robot_status.robot_id; 
    
    ui_init_static_Ungroup();
    osDelay(100);
    ui_update_static_Ungroup();
    osDelay(100);
    ui_init_ShootingFrame();
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

    static uint16_t i;

    if(i%10 == 0)
    {
        ui_ShootingFrame_AutoAim_AutoAimFlag->color = 8;
        ui_ShootingFrame_AutoAim_AutoAimFlag->width = 3;
        ui_ShootingFrame_AutoAim_AutoAimFlag->r = 200;

        ui_ShootingFrame_AutoAim_AutoAimFlag->color = 2;

        i = 0;
    }
    else
    {
        ui_ShootingFrame_AutoAim_AutoAimFlag->color = 4;
        ui_ShootingFrame_AutoAim_AutoAimFlag->width = 5;
        ui_ShootingFrame_AutoAim_AutoAimFlag->r = 150;

        ui_ShootingFrame_AutoAim_AutoAimFlag->color = 0;
    }
    i++;

    //绘制地盘方向
    {
        float out_x, out_y;
        RotateLineUI(
            ui_Chassis_Ungroup_NewLine->start_x,
            ui_Chassis_Ungroup_NewLine->start_y,
            50,
            Yaw,
            &out_x,
            &out_y
        );
        ui_Chassis_Ungroup_NewLine->end_x = out_x;
        ui_Chassis_Ungroup_NewLine->end_y = out_y;
    }
}


