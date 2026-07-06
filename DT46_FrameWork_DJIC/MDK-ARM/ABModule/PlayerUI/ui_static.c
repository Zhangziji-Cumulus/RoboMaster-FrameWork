//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>

#include "ui_interface.h"

ui_2_frame_t ui_Static_Fire_0;

ui_interface_rect_t *ui_Static_Fire_AutoAimRange = (ui_interface_rect_t*)&(ui_Static_Fire_0.data[0]);
ui_interface_round_t *ui_Static_Fire_FireFlag = (ui_interface_round_t*)&(ui_Static_Fire_0.data[1]);

void _ui_init_Static_Fire_0() {
    for (int i = 0; i < 2; i++) {
        ui_Static_Fire_0.data[i].figure_name[0] = 1;
        ui_Static_Fire_0.data[i].figure_name[1] = 0;
        ui_Static_Fire_0.data[i].figure_name[2] = i + 0;
        ui_Static_Fire_0.data[i].operate_type = 1;
    }
    for (int i = 2; i < 2; i++) {
        ui_Static_Fire_0.data[i].operate_type = 0;
    }

    ui_Static_Fire_AutoAimRange->figure_type = 1;
    ui_Static_Fire_AutoAimRange->operate_type = 1;
    ui_Static_Fire_AutoAimRange->layer = 0;
    ui_Static_Fire_AutoAimRange->color = 0;
    ui_Static_Fire_AutoAimRange->start_x = 484;
    ui_Static_Fire_AutoAimRange->start_y = 294;
    ui_Static_Fire_AutoAimRange->width = 2;
    ui_Static_Fire_AutoAimRange->end_x = 1426;
    ui_Static_Fire_AutoAimRange->end_y = 809;

    ui_Static_Fire_FireFlag->figure_type = 2;
    ui_Static_Fire_FireFlag->operate_type = 1;
    ui_Static_Fire_FireFlag->layer = 0;
    ui_Static_Fire_FireFlag->color = 0;
    ui_Static_Fire_FireFlag->start_x = 1380;
    ui_Static_Fire_FireFlag->start_y = 400;
    ui_Static_Fire_FireFlag->width = 5;
    ui_Static_Fire_FireFlag->r = 30;


    ui_proc_2_frame(&ui_Static_Fire_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Fire_0, sizeof(ui_Static_Fire_0));
}

void _ui_update_Static_Fire_0() {
    for (int i = 0; i < 2; i++) {
        ui_Static_Fire_0.data[i].operate_type = 2;
    }

    ui_proc_2_frame(&ui_Static_Fire_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Fire_0, sizeof(ui_Static_Fire_0));
}

void _ui_remove_Static_Fire_0() {
    for (int i = 0; i < 2; i++) {
        ui_Static_Fire_0.data[i].operate_type = 3;
    }

    ui_proc_2_frame(&ui_Static_Fire_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Fire_0, sizeof(ui_Static_Fire_0));
}

ui_string_frame_t ui_Static_Fire_1;
ui_interface_string_t* ui_Static_Fire_FireTest = &(ui_Static_Fire_1.option);

void _ui_init_Static_Fire_1() {
    ui_Static_Fire_1.option.figure_name[0] = 1;
    ui_Static_Fire_1.option.figure_name[1] = 0;
    ui_Static_Fire_1.option.figure_name[2] = 2;
    ui_Static_Fire_1.option.operate_type = 1;

    ui_Static_Fire_FireTest->figure_type = 7;
    ui_Static_Fire_FireTest->operate_type = 1;
    ui_Static_Fire_FireTest->layer = 0;
    ui_Static_Fire_FireTest->color = 1;
    ui_Static_Fire_FireTest->start_x = 1434;
    ui_Static_Fire_FireTest->start_y = 411;
    ui_Static_Fire_FireTest->width = 3;
    ui_Static_Fire_FireTest->font_size = 25;
    ui_Static_Fire_FireTest->str_length = 4;
    strcpy(ui_Static_Fire_FireTest->string, "Fire");


    ui_proc_string_frame(&ui_Static_Fire_1);
    SEND_MESSAGE((uint8_t *) &ui_Static_Fire_1, sizeof(ui_Static_Fire_1));
}

void _ui_update_Static_Fire_1() {
    ui_Static_Fire_1.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Fire_1);
    SEND_MESSAGE((uint8_t *) &ui_Static_Fire_1, sizeof(ui_Static_Fire_1));
}

void _ui_remove_Static_Fire_1() {
    ui_Static_Fire_1.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Fire_1);
    SEND_MESSAGE((uint8_t *) &ui_Static_Fire_1, sizeof(ui_Static_Fire_1));
}

void ui_init_Static_Fire() {
    _ui_init_Static_Fire_0();
    _ui_init_Static_Fire_1();
}

void ui_update_Static_Fire() {
    _ui_update_Static_Fire_0();
    _ui_update_Static_Fire_1();
}

void ui_remove_Static_Fire() {
    _ui_remove_Static_Fire_0();
    _ui_remove_Static_Fire_1();
}

ui_1_frame_t ui_Static_Ungroup_0;

ui_interface_round_t *ui_Static_Ungroup_ChassisFront = (ui_interface_round_t*)&(ui_Static_Ungroup_0.data[0]);

void _ui_init_Static_Ungroup_0() {
    for (int i = 0; i < 1; i++) {
        ui_Static_Ungroup_0.data[i].figure_name[0] = 1;
        ui_Static_Ungroup_0.data[i].figure_name[1] = 1;
        ui_Static_Ungroup_0.data[i].figure_name[2] = i + 0;
        ui_Static_Ungroup_0.data[i].operate_type = 1;
    }
    for (int i = 1; i < 1; i++) {
        ui_Static_Ungroup_0.data[i].operate_type = 0;
    }

    ui_Static_Ungroup_ChassisFront->figure_type = 2;
    ui_Static_Ungroup_ChassisFront->operate_type = 1;
    ui_Static_Ungroup_ChassisFront->layer = 0;
    ui_Static_Ungroup_ChassisFront->color = 0;
    ui_Static_Ungroup_ChassisFront->start_x = 250;
    ui_Static_Ungroup_ChassisFront->start_y = 700;
    ui_Static_Ungroup_ChassisFront->width = 5;
    ui_Static_Ungroup_ChassisFront->r = 50;


    ui_proc_1_frame(&ui_Static_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Ungroup_0, sizeof(ui_Static_Ungroup_0));
}

void _ui_update_Static_Ungroup_0() {
    for (int i = 0; i < 1; i++) {
        ui_Static_Ungroup_0.data[i].operate_type = 2;
    }

    ui_proc_1_frame(&ui_Static_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Ungroup_0, sizeof(ui_Static_Ungroup_0));
}

void _ui_remove_Static_Ungroup_0() {
    for (int i = 0; i < 1; i++) {
        ui_Static_Ungroup_0.data[i].operate_type = 3;
    }

    ui_proc_1_frame(&ui_Static_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Ungroup_0, sizeof(ui_Static_Ungroup_0));
}


void ui_init_Static_Ungroup() {
    _ui_init_Static_Ungroup_0();
}

void ui_update_Static_Ungroup() {
    _ui_update_Static_Ungroup_0();
}

void ui_remove_Static_Ungroup() {
    _ui_remove_Static_Ungroup_0();
}

