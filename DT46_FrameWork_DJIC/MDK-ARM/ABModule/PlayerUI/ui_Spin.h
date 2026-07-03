//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_Spin_H
#define UI_Spin_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_Spin_now_figures[2];
extern uint8_t ui_Spin_dirty_figure[2];

extern uint8_t ui_Spin_max_send_count[2];

#define ui_Spin_Ungroup_AutoAim ((ui_interface_round_t*)&(ui_Spin_now_figures[0]))
#define ui_Spin_Ungroup_Power ((ui_interface_number_t*)&(ui_Spin_now_figures[1]))


#define ui_Spin_Ungroup_AutoAim_max_send_count (ui_Spin_max_send_count[0])
#define ui_Spin_Ungroup_Power_max_send_count (ui_Spin_max_send_count[1])


#ifdef MANUAL_DIRTY
#define ui_Spin_Ungroup_AutoAim_dirty (ui_Spin_dirty_figure[0])
#define ui_Spin_Ungroup_Power_dirty (ui_Spin_dirty_figure[1])

#endif

void ui_init_Spin();
void ui_update_Spin();

#endif // UI_Spin_H
