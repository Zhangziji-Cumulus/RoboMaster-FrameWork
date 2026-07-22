//
// Created by RM UI Designer
// Static Edition
//

#ifndef UI_Static_H
#define UI_Static_H

#include "ui_interface.h"

extern ui_interface_round_t *ui_Static_Graphic_ChassisFront;
extern ui_interface_rect_t *ui_Static_Graphic_AutoAimRange;
extern ui_interface_rect_t *ui_Static_Graphic_PitchRect;
extern ui_interface_round_t *ui_Static_Graphic_FireFlag;
extern ui_interface_round_t *ui_Static_Graphic_AutoAimFlag;
extern ui_interface_round_t *ui_Static_Graphic_LoadFlag;
extern ui_interface_arc_t *ui_Static_Graphic_SuperPower;

void ui_init_Static_Graphic();
void ui_update_Static_Graphic();
void ui_remove_Static_Graphic();

extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine;
extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine2;
extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine3;
extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine4;
extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine5;
extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine6;
extern ui_interface_line_t *ui_Static_Graphic2_PredictionLine7;

void ui_init_Static_Graphic2();
void ui_update_Static_Graphic2();
void ui_remove_Static_Graphic2();

extern ui_interface_line_t *ui_Static_Graphic3_PredictionLine8;
extern ui_interface_line_t *ui_Static_Graphic3_PredictionLine9;

void ui_init_Static_Graphic3();
void ui_update_Static_Graphic3();
void ui_remove_Static_Graphic3();

extern ui_interface_string_t *ui_Static_Text1_AutoAimText;

void ui_init_Static_Text1();
void ui_update_Static_Text1();
void ui_remove_Static_Text1();

extern ui_interface_string_t *ui_Static_Text2_FireText;

void ui_init_Static_Text2();
void ui_update_Static_Text2();
void ui_remove_Static_Text2();

extern ui_interface_string_t *ui_Static_Text3_LoadText;

void ui_init_Static_Text3();
void ui_update_Static_Text3();
void ui_remove_Static_Text3();

extern ui_interface_string_t *ui_Static_Text4_SpinText;

void ui_init_Static_Text4();
void ui_update_Static_Text4();
void ui_remove_Static_Text4();

extern ui_interface_string_t *ui_Static_Text5_PitchText;

void ui_init_Static_Text5();
void ui_update_Static_Text5();
void ui_remove_Static_Text5();


#endif // UI_Static_H
