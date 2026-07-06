//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>

#include "ui_interface.h"

ui_5_frame_t ui_Static_Graphic_0;

ui_interface_round_t *ui_Static_Graphic_ChassisFront = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[0]);
ui_interface_rect_t *ui_Static_Graphic_AutoAimRange = (ui_interface_rect_t*)&(ui_Static_Graphic_0.data[1]);
ui_interface_round_t *ui_Static_Graphic_FireFlag = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[2]);
ui_interface_round_t *ui_Static_Graphic_AutoAimFlag = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[3]);
ui_interface_round_t *ui_Static_Graphic_LoadFlag = (ui_interface_round_t*)&(ui_Static_Graphic_0.data[4]);

void _ui_init_Static_Graphic_0() {
    for (int i = 0; i < 5; i++) {
        ui_Static_Graphic_0.data[i].figure_name[0] = 1;
        ui_Static_Graphic_0.data[i].figure_name[1] = 0;
        ui_Static_Graphic_0.data[i].figure_name[2] = i + 0;
        ui_Static_Graphic_0.data[i].operate_type = 1;
    }
    for (int i = 5; i < 5; i++) {
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


    ui_proc_5_frame(&ui_Static_Graphic_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Graphic_0, sizeof(ui_Static_Graphic_0));
}

void _ui_update_Static_Graphic_0() {
    for (int i = 0; i < 5; i++) {
        ui_Static_Graphic_0.data[i].operate_type = 2;
    }

    ui_proc_5_frame(&ui_Static_Graphic_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Graphic_0, sizeof(ui_Static_Graphic_0));
}

void _ui_remove_Static_Graphic_0() {
    for (int i = 0; i < 5; i++) {
        ui_Static_Graphic_0.data[i].operate_type = 3;
    }

    ui_proc_5_frame(&ui_Static_Graphic_0);
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


ui_string_frame_t ui_Static_Text_0;
ui_interface_string_t* ui_Static_Text_FireText = &(ui_Static_Text_0.option);

void _ui_init_Static_Text_0() {
    ui_Static_Text_0.option.figure_name[0] = 1;
    ui_Static_Text_0.option.figure_name[1] = 1;
    ui_Static_Text_0.option.figure_name[2] = 0;
    ui_Static_Text_0.option.operate_type = 1;

    ui_Static_Text_FireText->figure_type = 7;
    ui_Static_Text_FireText->operate_type = 1;
    ui_Static_Text_FireText->layer = 0;
    ui_Static_Text_FireText->color = 1;
    ui_Static_Text_FireText->start_x = 1190;
    ui_Static_Text_FireText->start_y = 364;
    ui_Static_Text_FireText->width = 3;
    ui_Static_Text_FireText->font_size = 25;
    ui_Static_Text_FireText->str_length = 4;
    strcpy(ui_Static_Text_FireText->string, "Fire");


    ui_proc_string_frame(&ui_Static_Text_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_0, sizeof(ui_Static_Text_0));
}

void _ui_update_Static_Text_0() {
    ui_Static_Text_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_0, sizeof(ui_Static_Text_0));
}

void _ui_remove_Static_Text_0() {
    ui_Static_Text_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text_0);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_0, sizeof(ui_Static_Text_0));
}
ui_string_frame_t ui_Static_Text_1;
ui_interface_string_t* ui_Static_Text_LoadText = &(ui_Static_Text_1.option);

void _ui_init_Static_Text_1() {
    ui_Static_Text_1.option.figure_name[0] = 1;
    ui_Static_Text_1.option.figure_name[1] = 1;
    ui_Static_Text_1.option.figure_name[2] = 1;
    ui_Static_Text_1.option.operate_type = 1;

    ui_Static_Text_LoadText->figure_type = 7;
    ui_Static_Text_LoadText->operate_type = 1;
    ui_Static_Text_LoadText->layer = 0;
    ui_Static_Text_LoadText->color = 1;
    ui_Static_Text_LoadText->start_x = 1122;
    ui_Static_Text_LoadText->start_y = 269;
    ui_Static_Text_LoadText->width = 3;
    ui_Static_Text_LoadText->font_size = 25;
    ui_Static_Text_LoadText->str_length = 4;
    strcpy(ui_Static_Text_LoadText->string, "Load");


    ui_proc_string_frame(&ui_Static_Text_1);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_1, sizeof(ui_Static_Text_1));
}

void _ui_update_Static_Text_1() {
    ui_Static_Text_1.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text_1);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_1, sizeof(ui_Static_Text_1));
}

void _ui_remove_Static_Text_1() {
    ui_Static_Text_1.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text_1);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_1, sizeof(ui_Static_Text_1));
}
ui_string_frame_t ui_Static_Text_2;
ui_interface_string_t* ui_Static_Text_AutoAimText = &(ui_Static_Text_2.option);

void _ui_init_Static_Text_2() {
    ui_Static_Text_2.option.figure_name[0] = 1;
    ui_Static_Text_2.option.figure_name[1] = 1;
    ui_Static_Text_2.option.figure_name[2] = 2;
    ui_Static_Text_2.option.operate_type = 1;

    ui_Static_Text_AutoAimText->figure_type = 7;
    ui_Static_Text_AutoAimText->operate_type = 1;
    ui_Static_Text_AutoAimText->layer = 0;
    ui_Static_Text_AutoAimText->color = 1;
    ui_Static_Text_AutoAimText->start_x = 1175;
    ui_Static_Text_AutoAimText->start_y = 461;
    ui_Static_Text_AutoAimText->width = 3;
    ui_Static_Text_AutoAimText->font_size = 25;
    ui_Static_Text_AutoAimText->str_length = 7;
    strcpy(ui_Static_Text_AutoAimText->string, "AutoAim");


    ui_proc_string_frame(&ui_Static_Text_2);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_2, sizeof(ui_Static_Text_2));
}

void _ui_update_Static_Text_2() {
    ui_Static_Text_2.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text_2);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_2, sizeof(ui_Static_Text_2));
}

void _ui_remove_Static_Text_2() {
    ui_Static_Text_2.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text_2);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_2, sizeof(ui_Static_Text_2));
}
ui_string_frame_t ui_Static_Text_3;
ui_interface_string_t* ui_Static_Text_SpinText = &(ui_Static_Text_3.option);

void _ui_init_Static_Text_3() {
    ui_Static_Text_3.option.figure_name[0] = 1;
    ui_Static_Text_3.option.figure_name[1] = 1;
    ui_Static_Text_3.option.figure_name[2] = 3;
    ui_Static_Text_3.option.operate_type = 1;

    ui_Static_Text_SpinText->figure_type = 7;
    ui_Static_Text_SpinText->operate_type = 1;
    ui_Static_Text_SpinText->layer = 0;
    ui_Static_Text_SpinText->color = 1;
    ui_Static_Text_SpinText->start_x = 82;
    ui_Static_Text_SpinText->start_y = 750;
    ui_Static_Text_SpinText->width = 3;
    ui_Static_Text_SpinText->font_size = 25;
    ui_Static_Text_SpinText->str_length = 4;
    strcpy(ui_Static_Text_SpinText->string, "Spin");


    ui_proc_string_frame(&ui_Static_Text_3);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_3, sizeof(ui_Static_Text_3));
}

void _ui_update_Static_Text_3() {
    ui_Static_Text_3.option.operate_type = 2;

    ui_proc_string_frame(&ui_Static_Text_3);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_3, sizeof(ui_Static_Text_3));
}

void _ui_remove_Static_Text_3() {
    ui_Static_Text_3.option.operate_type = 3;

    ui_proc_string_frame(&ui_Static_Text_3);
    SEND_MESSAGE((uint8_t *) &ui_Static_Text_3, sizeof(ui_Static_Text_3));
}

void ui_init_Static_Text() {
    _ui_init_Static_Text_0();
    _ui_init_Static_Text_1();
    _ui_init_Static_Text_2();
    _ui_init_Static_Text_3();
}

void ui_update_Static_Text() {
    _ui_update_Static_Text_0();
    _ui_update_Static_Text_1();
    _ui_update_Static_Text_2();
    _ui_update_Static_Text_3();
}

void ui_remove_Static_Text() {
    _ui_remove_Static_Text_0();
    _ui_remove_Static_Text_1();
    _ui_remove_Static_Text_2();
    _ui_remove_Static_Text_3();
}

