//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>

#include "ui_interface.h"

ui_1_frame_t ui_Static_Gimbal_0;

ui_interface_number_t *ui_Static_Gimbal_PitchAngle = (ui_interface_number_t*)&(ui_Static_Gimbal_0.data[0]);

void _ui_init_Static_Gimbal_0() {
    for (int i = 0; i < 1; i++) {
        ui_Static_Gimbal_0.data[i].figure_name[0] = 1;
        ui_Static_Gimbal_0.data[i].figure_name[1] = 0;
        ui_Static_Gimbal_0.data[i].figure_name[2] = i + 0;
        ui_Static_Gimbal_0.data[i].operate_type = 1;
    }
    for (int i = 1; i < 1; i++) {
        ui_Static_Gimbal_0.data[i].operate_type = 0;
    }

    ui_Static_Gimbal_PitchAngle->figure_type = 5;
    ui_Static_Gimbal_PitchAngle->operate_type = 1;
    ui_Static_Gimbal_PitchAngle->layer = 0;
    ui_Static_Gimbal_PitchAngle->color = 8;
    ui_Static_Gimbal_PitchAngle->start_x = 1734;
    ui_Static_Gimbal_PitchAngle->start_y = 710;
    ui_Static_Gimbal_PitchAngle->width = 3;
    ui_Static_Gimbal_PitchAngle->font_size = 25;
    ui_Static_Gimbal_PitchAngle->number = 12345;


    ui_proc_1_frame(&ui_Static_Gimbal_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Gimbal_0, sizeof(ui_Static_Gimbal_0));
}

void _ui_update_Static_Gimbal_0() {
    for (int i = 0; i < 1; i++) {
        ui_Static_Gimbal_0.data[i].operate_type = 2;
    }

    ui_proc_1_frame(&ui_Static_Gimbal_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Gimbal_0, sizeof(ui_Static_Gimbal_0));
}

void _ui_remove_Static_Gimbal_0() {
    for (int i = 0; i < 1; i++) {
        ui_Static_Gimbal_0.data[i].operate_type = 3;
    }

    ui_proc_1_frame(&ui_Static_Gimbal_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Gimbal_0, sizeof(ui_Static_Gimbal_0));
}


void ui_init_Static_Gimbal() {
    _ui_init_Static_Gimbal_0();
}

void ui_update_Static_Gimbal() {
    _ui_update_Static_Gimbal_0();
}

void ui_remove_Static_Gimbal() {
    _ui_remove_Static_Gimbal_0();
}

ui_7_frame_t ui_Static_Graphic_0;

ui_interface_round_t *ui_Static_Graphic_ChassisFront = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[0]);
ui_interface_rect_t *ui_Static_Graphic_AutoAimRange = (ui_interface_rect_t*)&(ui_Static_Graphic_0.data[1]);
ui_interface_rect_t *ui_Static_Graphic_PitchRect = (ui_interface_rect_t*)&(ui_Static_Graphic_0.data[2]);
ui_interface_round_t *ui_Static_Graphic_FireFlag = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[3]);
ui_interface_round_t *ui_Static_Graphic_AutoAimFlag = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[4]);
ui_interface_round_t *ui_Static_Graphic_LoadFlag = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[5]);
ui_interface_arc_t *ui_Static_Graphic_SuperPower = (ui_interface_arc_t*)&(ui_Static_Graphic_0.data[6]);

void _ui_init_Static_Graphic_0() {
    for (int i = 0; i < 7; i++) {
        ui_Static_Graphic_0.data[i].figure_name[0] = 1;
        ui_Static_Graphic_0.data[i].figure_name[1] = 1;
        ui_Static_Graphic_0.data[i].figure_name[2] = i + 0;
        ui_Static_Graphic_0.data[i].operate_type = 1;
    }
    for (int i = 7; i < 7; i++) {
        ui_Static_Graphic_0.data[i].operate_type = 0;
    }

    ui_Static_Graphic_ChassisFront->figure_type = 2;
    ui_Static_Graphic_ChassisFront->operate_type = 1;
    ui_Static_Graphic_ChassisFront->layer = 0;
    ui_Static_Graphic_ChassisFront->color = 0;
    ui_Static_Graphic_ChassisFront->start_x = 250;
    ui_Static_Graphic_ChassisFront->start_y = 700;
    ui_Static_Graphic_ChassisFront->width = 5;
    ui_Static_Graphic_ChassisFront->r = 50;

    ui_Static_Graphic_AutoAimRange->figure_type = 1;
    ui_Static_Graphic_AutoAimRange->operate_type = 1;
    ui_Static_Graphic_AutoAimRange->layer = 0;
    ui_Static_Graphic_AutoAimRange->color = 0;
    ui_Static_Graphic_AutoAimRange->start_x = 465;
    ui_Static_Graphic_AutoAimRange->start_y = 289;
    ui_Static_Graphic_AutoAimRange->width = 2;
    ui_Static_Graphic_AutoAimRange->end_x = 1407;
    ui_Static_Graphic_AutoAimRange->end_y = 804;

    ui_Static_Graphic_PitchRect->figure_type = 1;
    ui_Static_Graphic_PitchRect->operate_type = 1;
    ui_Static_Graphic_PitchRect->layer = 0;
    ui_Static_Graphic_PitchRect->color = 0;
    ui_Static_Graphic_PitchRect->start_x = 1723;
    ui_Static_Graphic_PitchRect->start_y = 655;
    ui_Static_Graphic_PitchRect->width = 2;
    ui_Static_Graphic_PitchRect->end_x = 1899;
    ui_Static_Graphic_PitchRect->end_y = 779;

    ui_Static_Graphic_FireFlag->figure_type = 2;
    ui_Static_Graphic_FireFlag->operate_type = 1;
    ui_Static_Graphic_FireFlag->layer = 0;
    ui_Static_Graphic_FireFlag->color = 6;
    ui_Static_Graphic_FireFlag->start_x = 1348;
    ui_Static_Graphic_FireFlag->start_y = 334;
    ui_Static_Graphic_FireFlag->width = 5;
    ui_Static_Graphic_FireFlag->r = 25;

    ui_Static_Graphic_AutoAimFlag->figure_type = 2;
    ui_Static_Graphic_AutoAimFlag->operate_type = 1;
    ui_Static_Graphic_AutoAimFlag->layer = 0;
    ui_Static_Graphic_AutoAimFlag->color = 3;
    ui_Static_Graphic_AutoAimFlag->start_x = 1390;
    ui_Static_Graphic_AutoAimFlag->start_y = 425;
    ui_Static_Graphic_AutoAimFlag->width = 5;
    ui_Static_Graphic_AutoAimFlag->r = 25;

    ui_Static_Graphic_LoadFlag->figure_type = 2;
    ui_Static_Graphic_LoadFlag->operate_type = 1;
    ui_Static_Graphic_LoadFlag->layer = 0;
    ui_Static_Graphic_LoadFlag->color = 5;
    ui_Static_Graphic_LoadFlag->start_x = 1300;
    ui_Static_Graphic_LoadFlag->start_y = 250;
    ui_Static_Graphic_LoadFlag->width = 5;
    ui_Static_Graphic_LoadFlag->r = 25;

    ui_Static_Graphic_SuperPower->figure_type = 4;
    ui_Static_Graphic_SuperPower->operate_type = 1;
    ui_Static_Graphic_SuperPower->layer = 0;
    ui_Static_Graphic_SuperPower->color = 8;
    ui_Static_Graphic_SuperPower->start_x = 960;
    ui_Static_Graphic_SuperPower->start_y = 450;
    ui_Static_Graphic_SuperPower->width = 20;
    ui_Static_Graphic_SuperPower->start_angle = 150;
    ui_Static_Graphic_SuperPower->end_angle = 210;
    ui_Static_Graphic_SuperPower->rx = 400;
    ui_Static_Graphic_SuperPower->ry = 350;


    ui_proc_7_frame(&ui_Static_Graphic_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Graphic_0, sizeof(ui_Static_Graphic_0));
}

void _ui_update_Static_Graphic_0() {
    for (int i = 0; i < 7; i++) {
        ui_Static_Graphic_0.data[i].operate_type = 2;
    }

    ui_proc_7_frame(&ui_Static_Graphic_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Graphic_0, sizeof(ui_Static_Graphic_0));
}

void _ui_remove_Static_Graphic_0() {
    for (int i = 0; i < 7; i++) {
        ui_Static_Graphic_0.data[i].operate_type = 3;
    }

    ui_proc_7_frame(&ui_Static_Graphic_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Graphic_0, sizeof(ui_Static_Graphic_0));
}


void ui_init_Static_Graphic() {
    _ui_init_Static_Graphic_0();
}

void ui_update_Static_Graphic() {
    _ui_update_Static_Graphic_0();
}

void ui_remove_Static_Graphic() {
    _ui_remove_Static_Graphic_0();
}


ui_string_frame_t ui_Static_Text1_0;
ui_interface_string_t* ui_Static_Text1_AutoAimText = &(ui_Static_Text1_0.option);

void _ui_init_Static_Text1_0() {
    ui_Static_Text1_0.option.figure_name[0] = 1;
    ui_Static_Text1_0.option.figure_name[1] = 2;
    ui_Static_Text1_0.option.figure_name[2] = 0;
    ui_Static_Text1_0.option.operate_type = 1;

    ui_Static_Text1_AutoAimText->figure_type = 7;
    ui_Static_Text1_AutoAimText->operate_type = 1;
    ui_Static_Text1_AutoAimText->layer = 0;
    ui_Static_Text1_AutoAimText->color = 1;
    ui_Static_Text1_AutoAimText->start_x = 1446;
    ui_Static_Text1_AutoAimText->start_y = 437;
    ui_Static_Text1_AutoAimText->width = 3;
    ui_Static_Text1_AutoAimText->font_size = 25;
    ui_Static_Text1_AutoAimText->str_length = 7;
    strcpy(ui_Static_Text1_AutoAimText->string, "AutoAim");


    ui_proc_string_frame(&ui_Static_Text1_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text1_0, sizeof(ui_Static_Text1_0));
}

void _ui_update_Static_Text1_0() {
    ui_Static_Text1_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text1_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text1_0, sizeof(ui_Static_Text1_0));
}

void _ui_remove_Static_Text1_0() {
    ui_Static_Text1_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text1_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text1_0, sizeof(ui_Static_Text1_0));
}

void ui_init_Static_Text1() {
    _ui_init_Static_Text1_0();
}

void ui_update_Static_Text1() {
    _ui_update_Static_Text1_0();
}

void ui_remove_Static_Text1() {
    _ui_remove_Static_Text1_0();
}


ui_string_frame_t ui_Static_Text2_0;
ui_interface_string_t* ui_Static_Text2_FireText = &(ui_Static_Text2_0.option);

void _ui_init_Static_Text2_0() {
    ui_Static_Text2_0.option.figure_name[0] = 1;
    ui_Static_Text2_0.option.figure_name[1] = 3;
    ui_Static_Text2_0.option.figure_name[2] = 0;
    ui_Static_Text2_0.option.operate_type = 1;

    ui_Static_Text2_FireText->figure_type = 7;
    ui_Static_Text2_FireText->operate_type = 1;
    ui_Static_Text2_FireText->layer = 0;
    ui_Static_Text2_FireText->color = 1;
    ui_Static_Text2_FireText->start_x = 1410;
    ui_Static_Text2_FireText->start_y = 347;
    ui_Static_Text2_FireText->width = 3;
    ui_Static_Text2_FireText->font_size = 25;
    ui_Static_Text2_FireText->str_length = 4;
    strcpy(ui_Static_Text2_FireText->string, "Fire");


    ui_proc_string_frame(&ui_Static_Text2_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text2_0, sizeof(ui_Static_Text2_0));
}

void _ui_update_Static_Text2_0() {
    ui_Static_Text2_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text2_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text2_0, sizeof(ui_Static_Text2_0));
}

void _ui_remove_Static_Text2_0() {
    ui_Static_Text2_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text2_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text2_0, sizeof(ui_Static_Text2_0));
}

void ui_init_Static_Text2() {
    _ui_init_Static_Text2_0();
}

void ui_update_Static_Text2() {
    _ui_update_Static_Text2_0();
}

void ui_remove_Static_Text2() {
    _ui_remove_Static_Text2_0();
}


ui_string_frame_t ui_Static_Text3_0;
ui_interface_string_t* ui_Static_Text3_LoadText = &(ui_Static_Text3_0.option);

void _ui_init_Static_Text3_0() {
    ui_Static_Text3_0.option.figure_name[0] = 1;
    ui_Static_Text3_0.option.figure_name[1] = 4;
    ui_Static_Text3_0.option.figure_name[2] = 0;
    ui_Static_Text3_0.option.operate_type = 1;

    ui_Static_Text3_LoadText->figure_type = 7;
    ui_Static_Text3_LoadText->operate_type = 1;
    ui_Static_Text3_LoadText->layer = 0;
    ui_Static_Text3_LoadText->color = 1;
    ui_Static_Text3_LoadText->start_x = 1347;
    ui_Static_Text3_LoadText->start_y = 258;
    ui_Static_Text3_LoadText->width = 3;
    ui_Static_Text3_LoadText->font_size = 25;
    ui_Static_Text3_LoadText->str_length = 4;
    strcpy(ui_Static_Text3_LoadText->string, "Load");


    ui_proc_string_frame(&ui_Static_Text3_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text3_0, sizeof(ui_Static_Text3_0));
}

void _ui_update_Static_Text3_0() {
    ui_Static_Text3_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text3_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text3_0, sizeof(ui_Static_Text3_0));
}

void _ui_remove_Static_Text3_0() {
    ui_Static_Text3_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text3_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text3_0, sizeof(ui_Static_Text3_0));
}

void ui_init_Static_Text3() {
    _ui_init_Static_Text3_0();
}

void ui_update_Static_Text3() {
    _ui_update_Static_Text3_0();
}

void ui_remove_Static_Text3() {
    _ui_remove_Static_Text3_0();
}


ui_string_frame_t ui_Static_Text4_0;
ui_interface_string_t* ui_Static_Text4_SpinText = &(ui_Static_Text4_0.option);

void _ui_init_Static_Text4_0() {
    ui_Static_Text4_0.option.figure_name[0] = 1;
    ui_Static_Text4_0.option.figure_name[1] = 5;
    ui_Static_Text4_0.option.figure_name[2] = 0;
    ui_Static_Text4_0.option.operate_type = 1;

    ui_Static_Text4_SpinText->figure_type = 7;
    ui_Static_Text4_SpinText->operate_type = 1;
    ui_Static_Text4_SpinText->layer = 0;
    ui_Static_Text4_SpinText->color = 1;
    ui_Static_Text4_SpinText->start_x = 96;
    ui_Static_Text4_SpinText->start_y = 743;
    ui_Static_Text4_SpinText->width = 2;
    ui_Static_Text4_SpinText->font_size = 20;
    ui_Static_Text4_SpinText->str_length = 4;
    strcpy(ui_Static_Text4_SpinText->string, "Spin");


    ui_proc_string_frame(&ui_Static_Text4_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text4_0, sizeof(ui_Static_Text4_0));
}

void _ui_update_Static_Text4_0() {
    ui_Static_Text4_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text4_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text4_0, sizeof(ui_Static_Text4_0));
}

void _ui_remove_Static_Text4_0() {
    ui_Static_Text4_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text4_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text4_0, sizeof(ui_Static_Text4_0));
}

void ui_init_Static_Text4() {
    _ui_init_Static_Text4_0();
}

void ui_update_Static_Text4() {
    _ui_update_Static_Text4_0();
}

void ui_remove_Static_Text4() {
    _ui_remove_Static_Text4_0();
}


ui_string_frame_t ui_Static_Text5_0;
ui_interface_string_t* ui_Static_Text5_PitchText = &(ui_Static_Text5_0.option);

void _ui_init_Static_Text5_0() {
    ui_Static_Text5_0.option.figure_name[0] = 1;
    ui_Static_Text5_0.option.figure_name[1] = 6;
    ui_Static_Text5_0.option.figure_name[2] = 0;
    ui_Static_Text5_0.option.operate_type = 1;

    ui_Static_Text5_PitchText->figure_type = 7;
    ui_Static_Text5_PitchText->operate_type = 1;
    ui_Static_Text5_PitchText->layer = 0;
    ui_Static_Text5_PitchText->color = 1;
    ui_Static_Text5_PitchText->start_x = 1740;
    ui_Static_Text5_PitchText->start_y = 755;
    ui_Static_Text5_PitchText->width = 2;
    ui_Static_Text5_PitchText->font_size = 20;
    ui_Static_Text5_PitchText->str_length = 5;
    strcpy(ui_Static_Text5_PitchText->string, "Pitch");


    ui_proc_string_frame(&ui_Static_Text5_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text5_0, sizeof(ui_Static_Text5_0));
}

void _ui_update_Static_Text5_0() {
    ui_Static_Text5_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text5_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text5_0, sizeof(ui_Static_Text5_0));
}

void _ui_remove_Static_Text5_0() {
    ui_Static_Text5_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text5_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text5_0, sizeof(ui_Static_Text5_0));
}

void ui_init_Static_Text5() {
    _ui_init_Static_Text5_0();
}

void ui_update_Static_Text5() {
    _ui_update_Static_Text5_0();
}

void ui_remove_Static_Text5() {
    _ui_remove_Static_Text5_0();
}

