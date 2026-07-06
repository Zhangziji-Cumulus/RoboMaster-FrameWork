//
// Created by RM UI Designer
// Dynamic Edition
//

#include "string.h"
#include "ui_interface.h"
#include "ui_Dynamic.h"

#define TOTAL_FIGURE 4
#define TOTAL_STRING 0

ui_interface_figure_t ui_Dynamic_now_figures[TOTAL_FIGURE];
uint8_t ui_Dynamic_dirty_figure[TOTAL_FIGURE];

uint8_t ui_Dynamic_max_send_count[TOTAL_FIGURE + TOTAL_STRING] = {
    1,
    1,
    1,
    1,
};

#ifndef MANUAL_DIRTY
ui_interface_figure_t ui_Dynamic_last_figures[TOTAL_FIGURE];
#endif

#define SCAN_AND_SEND() ui_scan_and_send(ui_Dynamic_now_figures, ui_Dynamic_dirty_figure, NULL, NULL, TOTAL_FIGURE, TOTAL_STRING)

void ui_init_Dynamic() {
    ui_Dynamic_Fire_AutoAimFlag->figure_type = 2;
    ui_Dynamic_Fire_AutoAimFlag->operate_type = 1;
    ui_Dynamic_Fire_AutoAimFlag->layer = 0;
    ui_Dynamic_Fire_AutoAimFlag->color = 8;
    ui_Dynamic_Fire_AutoAimFlag->start_x = 960;
    ui_Dynamic_Fire_AutoAimFlag->start_y = 540;
    ui_Dynamic_Fire_AutoAimFlag->width = 2;
    ui_Dynamic_Fire_AutoAimFlag->r = 200;

    ui_Dynamic_Chassis_SuperPower->figure_type = 4;
    ui_Dynamic_Chassis_SuperPower->operate_type = 1;
    ui_Dynamic_Chassis_SuperPower->layer = 0;
    ui_Dynamic_Chassis_SuperPower->color = 2;
    ui_Dynamic_Chassis_SuperPower->start_x = 960;
    ui_Dynamic_Chassis_SuperPower->start_y = 450;
    ui_Dynamic_Chassis_SuperPower->width = 18;
    ui_Dynamic_Chassis_SuperPower->start_angle = 150;
    ui_Dynamic_Chassis_SuperPower->end_angle = 210;
    ui_Dynamic_Chassis_SuperPower->rx = 400;
    ui_Dynamic_Chassis_SuperPower->ry = 350;

    ui_Dynamic_Chassis_ChassisFront->figure_type = 0;
    ui_Dynamic_Chassis_ChassisFront->operate_type = 1;
    ui_Dynamic_Chassis_ChassisFront->layer = 0;
    ui_Dynamic_Chassis_ChassisFront->color = 0;
    ui_Dynamic_Chassis_ChassisFront->start_x = 250;
    ui_Dynamic_Chassis_ChassisFront->start_y = 700;
    ui_Dynamic_Chassis_ChassisFront->width = 8;
    ui_Dynamic_Chassis_ChassisFront->end_x = 250;
    ui_Dynamic_Chassis_ChassisFront->end_y = 749;

    ui_Dynamic_Fire_FireFlag->figure_type = 2;
    ui_Dynamic_Fire_FireFlag->operate_type = 1;
    ui_Dynamic_Fire_FireFlag->layer = 0;
    ui_Dynamic_Fire_FireFlag->color = 8;
    ui_Dynamic_Fire_FireFlag->start_x = 1380;
    ui_Dynamic_Fire_FireFlag->start_y = 400;
    ui_Dynamic_Fire_FireFlag->width = 10;
    ui_Dynamic_Fire_FireFlag->r = 10;

    uint32_t idx = 0;
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Dynamic_now_figures[i].figure_name[2] = idx & 0xFF;
        ui_Dynamic_now_figures[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_Dynamic_now_figures[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_Dynamic_now_figures[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_Dynamic_last_figures[i] = ui_Dynamic_now_figures[i];
#endif
        ui_Dynamic_dirty_figure[i] = 1;
        idx++;
    }

    SCAN_AND_SEND();

    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Dynamic_now_figures[i].operate_type = 2;
    }
}

void ui_update_Dynamic() {
#ifndef MANUAL_DIRTY
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        if (memcmp(&ui_Dynamic_now_figures[i], &ui_Dynamic_last_figures[i], sizeof(ui_Dynamic_now_figures[i])) != 0) {
            ui_Dynamic_dirty_figure[i] = ui_Dynamic_max_send_count[i];
            ui_Dynamic_last_figures[i] = ui_Dynamic_now_figures[i];
        }
    }
#endif
    SCAN_AND_SEND();
}
