#include "DrawUI.h"


void DrawUI_Init(void)
{
    ui_init_Spin();
    ui_init_testAPI();
}

void DrawUI_Update(void)
{
    ui_update_testAPI();
    ui_update_Spin();
}
