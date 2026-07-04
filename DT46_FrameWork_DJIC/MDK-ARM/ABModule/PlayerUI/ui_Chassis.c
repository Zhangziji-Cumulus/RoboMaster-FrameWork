//
// Created by RM UI Designer
// Dynamic Edition
//

#include "string.h"
#include "ui_interface.h"
#include "ui_Chassis.h"

#define TOTAL_FIGURE 1
#define TOTAL_STRING 0

ui_interface_figure_t ui_Chassis_now_figures[TOTAL_FIGURE];
uint8_t ui_Chassis_dirty_figure[TOTAL_FIGURE];

uint8_t ui_Chassis_max_send_count[TOTAL_FIGURE + TOTAL_STRING] = {
    1,
};

#ifndef MANUAL_DIRTY
ui_interface_figure_t ui_Chassis_last_figures[TOTAL_FIGURE];
#endif

#define SCAN_AND_SEND() ui_scan_and_send(ui_Chassis_now_figures, ui_Chassis_dirty_figure, NULL, NULL, TOTAL_FIGURE, TOTAL_STRING)

void ui_init_Chassis() {
    ui_Chassis_Ungroup_NewLine->figure_type = 0;
    ui_Chassis_Ungroup_NewLine->operate_type = 1;
    ui_Chassis_Ungroup_NewLine->layer = 0;
    ui_Chassis_Ungroup_NewLine->color = 0;
    ui_Chassis_Ungroup_NewLine->start_x = 357;
    ui_Chassis_Ungroup_NewLine->start_y = 679;
    ui_Chassis_Ungroup_NewLine->width = 15;
    ui_Chassis_Ungroup_NewLine->end_x = 403;
    ui_Chassis_Ungroup_NewLine->end_y = 774;

    uint32_t idx = 0;
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Chassis_now_figures[i].figure_name[2] = idx & 0xFF;
        ui_Chassis_now_figures[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_Chassis_now_figures[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_Chassis_now_figures[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_Chassis_last_figures[i] = ui_Chassis_now_figures[i];
#endif
        ui_Chassis_dirty_figure[i] = 1;
        idx++;
    }

    SCAN_AND_SEND();

    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Chassis_now_figures[i].operate_type = 2;
    }
}

void ui_update_Chassis() {
#ifndef MANUAL_DIRTY
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        if (memcmp(&ui_Chassis_now_figures[i], &ui_Chassis_last_figures[i], sizeof(ui_Chassis_now_figures[i])) != 0) {
            ui_Chassis_dirty_figure[i] = ui_Chassis_max_send_count[i];
            ui_Chassis_last_figures[i] = ui_Chassis_now_figures[i];
        }
    }
#endif
    SCAN_AND_SEND();
}
