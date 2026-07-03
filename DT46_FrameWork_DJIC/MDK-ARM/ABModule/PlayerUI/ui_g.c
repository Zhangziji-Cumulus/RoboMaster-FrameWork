//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>

#include "ui_interface.h"

ui_2_frame_t ui_g_Ungroup_0;

ui_interface_round_t *ui_g_Ungroup_Power = (ui_interface_round_t*)&(ui_g_Ungroup_0.data[0]);
ui_interface_round_t *ui_g_Ungroup_AutoAim = (ui_interface_round_t*)&(ui_g_Ungroup_0.data[1]);

void _ui_init_g_Ungroup_0() {
    for (int i = 0; i < 2; i++) {
        ui_g_Ungroup_0.data[i].figure_name[0] = 0;
        ui_g_Ungroup_0.data[i].figure_name[1] = 0;
        ui_g_Ungroup_0.data[i].figure_name[2] = i + 0;
        ui_g_Ungroup_0.data[i].operate_type = 1;
    }
    for (int i = 2; i < 2; i++) {
        ui_g_Ungroup_0.data[i].operate_type = 0;
    }

    ui_g_Ungroup_Power->figure_type = 2;
    ui_g_Ungroup_Power->operate_type = 1;
    ui_g_Ungroup_Power->layer = 0;
    ui_g_Ungroup_Power->color = 0;
    ui_g_Ungroup_Power->start_x = 436;
    ui_g_Ungroup_Power->start_y = 678;
    ui_g_Ungroup_Power->width = 10;
    ui_g_Ungroup_Power->r = 92;

    ui_g_Ungroup_AutoAim->figure_type = 2;
    ui_g_Ungroup_AutoAim->operate_type = 1;
    ui_g_Ungroup_AutoAim->layer = 5;
    ui_g_Ungroup_AutoAim->color = 1;
    ui_g_Ungroup_AutoAim->start_x = 950;
    ui_g_Ungroup_AutoAim->start_y = 533;
    ui_g_Ungroup_AutoAim->width = 10;
    ui_g_Ungroup_AutoAim->r = 133;


    ui_proc_2_frame(&ui_g_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Ungroup_0, sizeof(ui_g_Ungroup_0));
}

void _ui_update_g_Ungroup_0() {
    for (int i = 0; i < 2; i++) {
        ui_g_Ungroup_0.data[i].operate_type = 2;
    }

    ui_proc_2_frame(&ui_g_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Ungroup_0, sizeof(ui_g_Ungroup_0));
}

void _ui_remove_g_Ungroup_0() {
    for (int i = 0; i < 2; i++) {
        ui_g_Ungroup_0.data[i].operate_type = 3;
    }

    ui_proc_2_frame(&ui_g_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Ungroup_0, sizeof(ui_g_Ungroup_0));
}


void ui_init_g_Ungroup() {
    _ui_init_g_Ungroup_0();
}

void ui_update_g_Ungroup() {
    _ui_update_g_Ungroup_0();
}

void ui_remove_g_Ungroup() {
    _ui_remove_g_Ungroup_0();
}

