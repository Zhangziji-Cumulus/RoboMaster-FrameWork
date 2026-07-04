//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>

#include "ui_interface.h"

ui_1_frame_t ui_static_Ungroup_0;

ui_interface_arc_t *ui_static_Ungroup_NewArc = (ui_interface_arc_t*)&(ui_static_Ungroup_0.data[0]);

void _ui_init_static_Ungroup_0() {
    for (int i = 0; i < 1; i++) {
        ui_static_Ungroup_0.data[i].figure_name[0] = 1;
        ui_static_Ungroup_0.data[i].figure_name[1] = 0;
        ui_static_Ungroup_0.data[i].figure_name[2] = i + 0;
        ui_static_Ungroup_0.data[i].operate_type = 1;
    }
    for (int i = 1; i < 1; i++) {
        ui_static_Ungroup_0.data[i].operate_type = 0;
    }

    ui_static_Ungroup_NewArc->figure_type = 4;
    ui_static_Ungroup_NewArc->operate_type = 1;
    ui_static_Ungroup_NewArc->layer = 0;
    ui_static_Ungroup_NewArc->color = 0;
    ui_static_Ungroup_NewArc->start_x = 960;
    ui_static_Ungroup_NewArc->start_y = 450;
    ui_static_Ungroup_NewArc->width = 20;
    ui_static_Ungroup_NewArc->start_angle = 150;
    ui_static_Ungroup_NewArc->end_angle = 210;
    ui_static_Ungroup_NewArc->rx = 389;
    ui_static_Ungroup_NewArc->ry = 279;


    ui_proc_1_frame(&ui_static_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_static_Ungroup_0, sizeof(ui_static_Ungroup_0));
}

void _ui_update_static_Ungroup_0() {
    for (int i = 0; i < 1; i++) {
        ui_static_Ungroup_0.data[i].operate_type = 2;
    }

    ui_proc_1_frame(&ui_static_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_static_Ungroup_0, sizeof(ui_static_Ungroup_0));
}

void _ui_remove_static_Ungroup_0() {
    for (int i = 0; i < 1; i++) {
        ui_static_Ungroup_0.data[i].operate_type = 3;
    }

    ui_proc_1_frame(&ui_static_Ungroup_0);
    SEND_MESSAGE((uint8_t *) &ui_static_Ungroup_0, sizeof(ui_static_Ungroup_0));
}


void ui_init_static_Ungroup() {
    _ui_init_static_Ungroup_0();
}

void ui_update_static_Ungroup() {
    _ui_update_static_Ungroup_0();
}

void ui_remove_static_Ungroup() {
    _ui_remove_static_Ungroup_0();
}

