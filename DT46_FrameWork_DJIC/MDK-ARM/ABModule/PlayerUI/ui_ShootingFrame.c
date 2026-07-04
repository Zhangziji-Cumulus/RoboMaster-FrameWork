//
// Created by RM UI Designer
// Dynamic Edition
//

#include "string.h"
#include "ui_interface.h"
#include "ui_ShootingFrame.h"

#define TOTAL_FIGURE 2
#define TOTAL_STRING 0

ui_interface_figure_t ui_ShootingFrame_now_figures[TOTAL_FIGURE];
uint8_t ui_ShootingFrame_dirty_figure[TOTAL_FIGURE];

uint8_t ui_ShootingFrame_max_send_count[TOTAL_FIGURE + TOTAL_STRING] = {
    1,
    1,
};

#ifndef MANUAL_DIRTY
ui_interface_figure_t ui_ShootingFrame_last_figures[TOTAL_FIGURE];
#endif

#define SCAN_AND_SEND() ui_scan_and_send(ui_ShootingFrame_now_figures, ui_ShootingFrame_dirty_figure, NULL, NULL, TOTAL_FIGURE, TOTAL_STRING)

void ui_init_ShootingFrame() {
    ui_ShootingFrame_AutoAim_AutoAimFlag->figure_type = 2;
    ui_ShootingFrame_AutoAim_AutoAimFlag->operate_type = 1;
    ui_ShootingFrame_AutoAim_AutoAimFlag->layer = 0;
    ui_ShootingFrame_AutoAim_AutoAimFlag->color = 8;
    ui_ShootingFrame_AutoAim_AutoAimFlag->start_x = 960;
    ui_ShootingFrame_AutoAim_AutoAimFlag->start_y = 540;
    ui_ShootingFrame_AutoAim_AutoAimFlag->width = 3;
    ui_ShootingFrame_AutoAim_AutoAimFlag->r = 200;

    ui_ShootingFrame_AutoAim_DetectionRange->figure_type = 1;
    ui_ShootingFrame_AutoAim_DetectionRange->operate_type = 1;
    ui_ShootingFrame_AutoAim_DetectionRange->layer = 0;
    ui_ShootingFrame_AutoAim_DetectionRange->color = 0;
    ui_ShootingFrame_AutoAim_DetectionRange->start_x = 526;
    ui_ShootingFrame_AutoAim_DetectionRange->start_y = 268;
    ui_ShootingFrame_AutoAim_DetectionRange->width = 3;
    ui_ShootingFrame_AutoAim_DetectionRange->end_x = 1405;
    ui_ShootingFrame_AutoAim_DetectionRange->end_y = 794;

    uint32_t idx = 0;
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_ShootingFrame_now_figures[i].figure_name[2] = idx & 0xFF;
        ui_ShootingFrame_now_figures[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_ShootingFrame_now_figures[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_ShootingFrame_now_figures[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_ShootingFrame_last_figures[i] = ui_ShootingFrame_now_figures[i];
#endif
        ui_ShootingFrame_dirty_figure[i] = 1;
        idx++;
    }

    SCAN_AND_SEND();

    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_ShootingFrame_now_figures[i].operate_type = 2;
    }
}

void ui_update_ShootingFrame() {
#ifndef MANUAL_DIRTY
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        if (memcmp(&ui_ShootingFrame_now_figures[i], &ui_ShootingFrame_last_figures[i], sizeof(ui_ShootingFrame_now_figures[i])) != 0) {
            ui_ShootingFrame_dirty_figure[i] = ui_ShootingFrame_max_send_count[i];
            ui_ShootingFrame_last_figures[i] = ui_ShootingFrame_now_figures[i];
        }
    }
#endif
    SCAN_AND_SEND();
}
