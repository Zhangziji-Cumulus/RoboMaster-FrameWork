//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_testAPI_H
#define UI_testAPI_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_testAPI_now_figures[2];
extern uint8_t ui_testAPI_dirty_figure[2];

extern uint8_t ui_testAPI_max_send_count[2];

#define ui_testAPI_AutoAim_AutoAimFlag ((ui_interface_round_t*)&(ui_testAPI_now_figures[0]))
#define ui_testAPI_Ungroup_DetectionRange ((ui_interface_rect_t*)&(ui_testAPI_now_figures[1]))


#define ui_testAPI_AutoAim_AutoAimFlag_max_send_count (ui_testAPI_max_send_count[0])
#define ui_testAPI_Ungroup_DetectionRange_max_send_count (ui_testAPI_max_send_count[1])


#ifdef MANUAL_DIRTY
#define ui_testAPI_AutoAim_AutoAimFlag_dirty (ui_testAPI_dirty_figure[0])
#define ui_testAPI_Ungroup_DetectionRange_dirty (ui_testAPI_dirty_figure[1])

#endif

void ui_init_testAPI();
void ui_update_testAPI();

#endif // UI_testAPI_H
