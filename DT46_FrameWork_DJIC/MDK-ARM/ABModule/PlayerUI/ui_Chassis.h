//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_Chassis_H
#define UI_Chassis_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_Chassis_now_figures[1];
extern uint8_t ui_Chassis_dirty_figure[1];

extern uint8_t ui_Chassis_max_send_count[1];

#define ui_Chassis_Ungroup_NewLine ((ui_interface_line_t*)&(ui_Chassis_now_figures[0]))


#define ui_Chassis_Ungroup_NewLine_max_send_count (ui_Chassis_max_send_count[0])


#ifdef MANUAL_DIRTY
#define ui_Chassis_Ungroup_NewLine_dirty (ui_Chassis_dirty_figure[0])

#endif

void ui_init_Chassis();
void ui_update_Chassis();

#endif // UI_Chassis_H
