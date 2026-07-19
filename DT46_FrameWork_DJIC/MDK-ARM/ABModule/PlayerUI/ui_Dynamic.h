//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_Dynamic_H
#define UI_Dynamic_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_Dynamic_now_figures[7];
extern uint8_t ui_Dynamic_dirty_figure[7];

extern uint8_t ui_Dynamic_max_send_count[7];

#define ui_Dynamic_Fire_AutoAimLock ((ui_interface_round_t*)&(ui_Dynamic_now_figures[0]))
#define ui_Dynamic_Chassis_SuperPower ((ui_interface_arc_t*)&(ui_Dynamic_now_figures[1]))
#define ui_Dynamic_Chassis_ChassisFront ((ui_interface_line_t*)&(ui_Dynamic_now_figures[2]))
#define ui_Dynamic_Fire_AutoAimFlag ((ui_interface_round_t*)&(ui_Dynamic_now_figures[3]))
#define ui_Dynamic_Fire_FireFlag ((ui_interface_round_t*)&(ui_Dynamic_now_figures[4]))
#define ui_Dynamic_Fire_LoadFlag ((ui_interface_round_t*)&(ui_Dynamic_now_figures[5]))
#define ui_Dynamic_Gimbal_PitchAngle ((ui_interface_number_t*)&(ui_Dynamic_now_figures[6]))


#define ui_Dynamic_Fire_AutoAimLock_max_send_count (ui_Dynamic_max_send_count[0])
#define ui_Dynamic_Chassis_SuperPower_max_send_count (ui_Dynamic_max_send_count[1])
#define ui_Dynamic_Chassis_ChassisFront_max_send_count (ui_Dynamic_max_send_count[2])
#define ui_Dynamic_Fire_AutoAimFlag_max_send_count (ui_Dynamic_max_send_count[3])
#define ui_Dynamic_Fire_FireFlag_max_send_count (ui_Dynamic_max_send_count[4])
#define ui_Dynamic_Fire_LoadFlag_max_send_count (ui_Dynamic_max_send_count[5])
#define ui_Dynamic_Gimbal_PitchAngle_max_send_count (ui_Dynamic_max_send_count[6])


#ifdef MANUAL_DIRTY
#define ui_Dynamic_Fire_AutoAimLock_dirty (ui_Dynamic_dirty_figure[0])
#define ui_Dynamic_Chassis_SuperPower_dirty (ui_Dynamic_dirty_figure[1])
#define ui_Dynamic_Chassis_ChassisFront_dirty (ui_Dynamic_dirty_figure[2])
#define ui_Dynamic_Fire_AutoAimFlag_dirty (ui_Dynamic_dirty_figure[3])
#define ui_Dynamic_Fire_FireFlag_dirty (ui_Dynamic_dirty_figure[4])
#define ui_Dynamic_Fire_LoadFlag_dirty (ui_Dynamic_dirty_figure[5])
#define ui_Dynamic_Gimbal_PitchAngle_dirty (ui_Dynamic_dirty_figure[6])

#endif

void ui_init_Dynamic();
void ui_update_Dynamic();

#endif // UI_Dynamic_H
