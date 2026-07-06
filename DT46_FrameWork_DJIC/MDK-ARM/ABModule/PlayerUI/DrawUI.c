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


    ui_init_Static_Text();
    osDelay(1000);
    ui_update_Static_Text();
    osDelay(1000);

    // ui_init_Static_Graphic();
    // osDelay(150);
    // ui_update_Static_Graphic();
    // osDelay(150);

    //初始化动态UI
    //ui_init_Dynamic();
}

void DrawUI_Update(void)
{
    //更新动态数据
    DrawUI_Dynamic();

    //更新动态UI
    
    //ui_update_Dynamic();
    

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

    //获取裁判系统数据
    Referee_DrawUI = Referee_GetData();
    //自动获取当前机器人ID,发送和接受要对应上UI才能上去
    ui_self_id = Referee_DrawUI->_robot_status.robot_id; 
    
    ui_init_Static_Text();
    osDelay(500);
    ui_update_Static_Text();
    osDelay(500);

    // ui_init_Static_Graphic();
    // osDelay(150);
    // ui_update_Static_Graphic();
    // osDelay(150);

    //ui_init_Dynamic();
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

    //简单的测试数据
    static uint16_t i;
    i++;
    static uint16_t a;
    a++;
    static float powertest;

    if(a % 20)
    {
        if(powertest >= 100)
        {
            powertest = 0;
        }
        powertest += 5;
    }

    if(a > 100)
    {
        a = 0;
    }
    



    //绘制自瞄瞄状态标志
    if(i%10 == 0)
    {
        ui_Dynamic_Fire_AutoAimFlag->color = 8;
        ui_Dynamic_Fire_AutoAimFlag->width = 3;
        ui_Dynamic_Fire_AutoAimFlag->r = 200;

        ui_Dynamic_Fire_AutoAimFlag->color = 2;

        i = 0;
    }
    else
    {
        ui_Dynamic_Fire_AutoAimFlag->color = 4;
        ui_Dynamic_Fire_AutoAimFlag->width = 5;
        ui_Dynamic_Fire_AutoAimFlag->r = 150;

        ui_Dynamic_Fire_AutoAimFlag->color = 0;
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

    //绘制开火标志位
    if(a % 20 == 0)
    {
        ui_Dynamic_Fire_FireFlag->color = 1;
    }
    else if(a % 40 == 0)
    {
        ui_Dynamic_Fire_FireFlag->color = 8;
    }

    //绘制超电容量
    ui_Dynamic_Chassis_SuperPower->start_angle = LerpAngle(ui_Dynamic_Chassis_SuperPower->end_angle,
                                                         -60.0f,
                                                         powertest,
                                                         1);
}


