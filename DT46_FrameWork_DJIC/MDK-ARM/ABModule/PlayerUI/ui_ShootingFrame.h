//
// Created by RM UI Designer
// Dynamic Edition
//

#ifndef UI_ShootingFrame_H
#define UI_ShootingFrame_H

#include "ui_interface.h"

extern ui_interface_figure_t ui_ShootingFrame_now_figures[2];
extern uint8_t ui_ShootingFrame_dirty_figure[2];

extern uint8_t ui_ShootingFrame_max_send_count[2];

#define ui_ShootingFrame_AutoAim_AutoAimFlag ((ui_interface_round_t*)&(ui_ShootingFrame_now_figures[0]))
#define ui_ShootingFrame_AutoAim_DetectionRange ((ui_interface_rect_t*)&(ui_ShootingFrame_now_figures[1]))


#define ui_ShootingFrame_AutoAim_AutoAimFlag_max_send_count (ui_ShootingFrame_max_send_count[0])
#define ui_ShootingFrame_AutoAim_DetectionRange_max_send_count (ui_ShootingFrame_max_send_count[1])


#ifdef MANUAL_DIRTY
#define ui_ShootingFrame_AutoAim_AutoAimFlag_dirty (ui_ShootingFrame_dirty_figure[0])
#define ui_ShootingFrame_AutoAim_DetectionRange_dirty (ui_ShootingFrame_dirty_figure[1])

#endif

void ui_init_ShootingFrame();
void ui_update_ShootingFrame();

#endif // UI_ShootingFrame_H
