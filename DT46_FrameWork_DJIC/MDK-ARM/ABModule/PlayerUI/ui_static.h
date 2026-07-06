//
// Created by RM UI Designer
// Static Edition
//

#ifndef UI_Static_H
#define UI_Static_H

#include "ui_interface.h"

extern ui_interface_round_t *ui_Static_Graphic_ChassisFront;
extern ui_interface_rect_t *ui_Static_Graphic_AutoAimRange;
extern ui_interface_round_t *ui_Static_Graphic_FireFlag;
extern ui_interface_round_t *ui_Static_Graphic_AutoAimFlag;
extern ui_interface_round_t *ui_Static_Graphic_LoadFlag;

void ui_init_Static_Graphic();
void ui_update_Static_Graphic();
void ui_remove_Static_Graphic();

extern ui_interface_string_t *ui_Static_Text_FireText;
extern ui_interface_string_t *ui_Static_Text_LoadText;
extern ui_interface_string_t *ui_Static_Text_AutoAimText;
extern ui_interface_string_t *ui_Static_Text_SpinText;

void ui_init_Static_Text();
void ui_update_Static_Text();
void ui_remove_Static_Text();


#endif // UI_Static_H
