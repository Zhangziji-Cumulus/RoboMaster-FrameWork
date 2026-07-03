//
// Created by RM UI Designer
// Dynamic Edition
//

#include "string.h"
#include "ui_interface.h"
#include "ui_Spin.h"

#define TOTAL_FIGURE 2
#define TOTAL_STRING 0

ui_interface_figure_t ui_Spin_now_figures[TOTAL_FIGURE];
uint8_t ui_Spin_dirty_figure[TOTAL_FIGURE];

uint8_t ui_Spin_max_send_count[TOTAL_FIGURE + TOTAL_STRING] = {
    1,
    1,
};

#ifndef MANUAL_DIRTY
ui_interface_figure_t ui_Spin_last_figures[TOTAL_FIGURE];
#endif

#define SCAN_AND_SEND() ui_scan_and_send(ui_Spin_now_figures, ui_Spin_dirty_figure, NULL, NULL, TOTAL_FIGURE, TOTAL_STRING)

void ui_init_Spin() {
    ui_Spin_Ungroup_AutoAim->figure_type = 2;
    ui_Spin_Ungroup_AutoAim->operate_type = 1;
    ui_Spin_Ungroup_AutoAim->layer = 0;
    ui_Spin_Ungroup_AutoAim->color = 8;
    ui_Spin_Ungroup_AutoAim->start_x = 960;
    ui_Spin_Ungroup_AutoAim->start_y = 540;
    ui_Spin_Ungroup_AutoAim->width = 20;
    ui_Spin_Ungroup_AutoAim->r = 150;

    ui_Spin_Ungroup_Power->figure_type = 6;
    ui_Spin_Ungroup_Power->operate_type = 1;
    ui_Spin_Ungroup_Power->layer = 0;
    ui_Spin_Ungroup_Power->color = 2;
    ui_Spin_Ungroup_Power->start_x = 500;
    ui_Spin_Ungroup_Power->start_y = 800;
    ui_Spin_Ungroup_Power->width = 5;
    ui_Spin_Ungroup_Power->font_size = 50;
    ui_Spin_Ungroup_Power->number = 1;

    uint32_t idx = 0;
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Spin_now_figures[i].figure_name[2] = idx & 0xFF;
        ui_Spin_now_figures[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_Spin_now_figures[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_Spin_now_figures[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_Spin_last_figures[i] = ui_Spin_now_figures[i];
#endif
        ui_Spin_dirty_figure[i] = 1;
        idx++;
    }

    SCAN_AND_SEND();

    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Spin_now_figures[i].operate_type = 2;
    }
}

void ui_update_Spin() {
#ifndef MANUAL_DIRTY
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        if (memcmp(&ui_Spin_now_figures[i], &ui_Spin_last_figures[i], sizeof(ui_Spin_now_figures[i])) != 0) {
            ui_Spin_dirty_figure[i] = ui_Spin_max_send_count[i];
            ui_Spin_last_figures[i] = ui_Spin_now_figures[i];
        }
    }
#endif
    SCAN_AND_SEND();
}
