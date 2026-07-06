//
// Created by RM UI Designer
// Static Edition
//

#ifndef UI_Static_H
#define UI_Static_H

#include "ui_interface.h"

extern ui_interface_rect_t *ui_Static_Fire_AutoAimRange;
extern ui_interface_round_t *ui_Static_Fire_FireFlag;
extern ui_interface_string_t *ui_Static_Fire_FireTest;

void ui_init_Static_Fire();
void ui_update_Static_Fire();
void ui_remove_Static_Fire();

extern ui_interface_round_t *ui_Static_Ungroup_ChassisFront;

void ui_init_Static_Ungroup();
void ui_update_Static_Ungroup();
void ui_remove_Static_Ungroup();


#endif // UI_Static_H
