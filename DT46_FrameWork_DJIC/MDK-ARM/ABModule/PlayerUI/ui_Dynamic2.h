//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_Dynamic2_H
#define UI_Dynamic2_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_Dynamic2_now_figures[2];
extern uint8_t ui_Dynamic2_dirty_figure[2];

extern uint8_t ui_Dynamic2_max_send_count[2];

#define ui_Dynamic2_Gimbal_PitchLine ((ui_interface_line_t*)&(ui_Dynamic2_now_figures[0]))
#define ui_Dynamic2_Gimbal_PitchAngle ((ui_interface_number_t*)&(ui_Dynamic2_now_figures[1]))


#define ui_Dynamic2_Gimbal_PitchLine_max_send_count (ui_Dynamic2_max_send_count[0])
#define ui_Dynamic2_Gimbal_PitchAngle_max_send_count (ui_Dynamic2_max_send_count[1])


#ifdef MANUAL_DIRTY
#define ui_Dynamic2_Gimbal_PitchLine_dirty (ui_Dynamic2_dirty_figure[0])
#define ui_Dynamic2_Gimbal_PitchAngle_dirty (ui_Dynamic2_dirty_figure[1])

#endif

void ui_init_Dynamic2();
void ui_update_Dynamic2();

#endif // UI_Dynamic2_H
