#include "DrawUI.h"
#include "A_CommonSystem.h"

static const referee_all_data_t* ReFeree_DrawUI;

//使用这个函数来更新交互、数据
static void Update_Data(void);
static void DrawUI_Dynamic(void);
static void DrawUI_ReInit(uint32_t timems);

void DrawUI_Init(void)
{
    osDelay(1000);

    ReFeree_DrawUI = Referee_GetData();
    ui_self_id = ReFeree_DrawUI->_robot_status.robot_id; 
    ui_init_ShootingFrame();

    ui_init_Chassis();
    //ui_init_Chassis();
}

void DrawUI_Update(void)
{
    DrawUI_Dynamic();

    ui_update_ShootingFrame();

    ui_update_Chassis();

    DrawUI_ReInit(10000);

    
}

//重新初始化UI手动触发、自动触发（每隔 timems 毫秒执行一次，基于 HAL 滴答）
static void DrawUI_ReInit(uint32_t timems)
{
    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();

    if (now - last_tick < timems)
        return;
    last_tick = now;

    ReFeree_DrawUI = Referee_GetData();
    ui_self_id = ReFeree_DrawUI->_robot_status.robot_id; 
    ui_init_ShootingFrame();
    ui_init_Chassis();
}

//使用这个函数来更新交互、数据
static void Update_Data(void)
{
    ReFeree_DrawUI = Referee_GetData();
} 

//使用这个函数来更新动态值
static void DrawUI_Dynamic(void)
{
    Update_Data();

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

}

