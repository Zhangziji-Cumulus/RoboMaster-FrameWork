//
// Created by RM UI Designer
// Dynamic Edition
//

#include "string.h"
#include "ui_interface.h"
#include "ui_Dynamic2.h"

#define TOTAL_FIGURE 2
#define TOTAL_STRING 0

ui_interface_figure_t ui_Dynamic2_now_figures[TOTAL_FIGURE];
uint8_t ui_Dynamic2_dirty_figure[TOTAL_FIGURE];

uint8_t ui_Dynamic2_max_send_count[TOTAL_FIGURE + TOTAL_STRING] = {
    1,
    1,
};

#ifndef MANUAL_DIRTY
ui_interface_figure_t ui_Dynamic2_last_figures[TOTAL_FIGURE];
#endif

#define SCAN_AND_SEND() ui_scan_and_send(ui_Dynamic2_now_figures, ui_Dynamic2_dirty_figure, NULL, NULL, TOTAL_FIGURE, TOTAL_STRING)

void ui_init_Dynamic2() {
    ui_Dynamic2_Gimbal_PitchLine->figure_type = 0;
    ui_Dynamic2_Gimbal_PitchLine->operate_type = 1;
    ui_Dynamic2_Gimbal_PitchLine->layer = 0;
    ui_Dynamic2_Gimbal_PitchLine->color = 0;
    ui_Dynamic2_Gimbal_PitchLine->start_x = 1669;
    ui_Dynamic2_Gimbal_PitchLine->start_y = 700;
    ui_Dynamic2_Gimbal_PitchLine->width = 8;
    ui_Dynamic2_Gimbal_PitchLine->end_x = 1620;
    ui_Dynamic2_Gimbal_PitchLine->end_y = 700;

    ui_Dynamic2_Gimbal_PitchAngle->figure_type = 5;
    ui_Dynamic2_Gimbal_PitchAngle->operate_type = 1;
    ui_Dynamic2_Gimbal_PitchAngle->layer = 0;
    ui_Dynamic2_Gimbal_PitchAngle->color = 8;
    ui_Dynamic2_Gimbal_PitchAngle->start_x = 1750;
    ui_Dynamic2_Gimbal_PitchAngle->start_y = 700;
    ui_Dynamic2_Gimbal_PitchAngle->width = 3;
    ui_Dynamic2_Gimbal_PitchAngle->font_size = 25;
    ui_Dynamic2_Gimbal_PitchAngle->number = 12345;

    uint32_t idx = 0;
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Dynamic2_now_figures[i].figure_name[2] = idx & 0xFF;
        ui_Dynamic2_now_figures[i].figure_name[1] = (idx >> 8) & 0xFF;
        ui_Dynamic2_now_figures[i].figure_name[0] = (idx >> 16) & 0xFF;
        ui_Dynamic2_now_figures[i].operate_type = 1;
#ifndef MANUAL_DIRTY
        ui_Dynamic2_last_figures[i] = ui_Dynamic2_now_figures[i];
#endif
        ui_Dynamic2_dirty_figure[i] = 1;
        idx++;
    }

    SCAN_AND_SEND();

    for (int i = 0; i < TOTAL_FIGURE; i++) {
        ui_Dynamic2_now_figures[i].operate_type = 2;
    }
}

void ui_update_Dynamic2() {
#ifndef MANUAL_DIRTY
    for (int i = 0; i < TOTAL_FIGURE; i++) {
        if (memcmp(&ui_Dynamic2_now_figures[i], &ui_Dynamic2_last_figures[i], sizeof(ui_Dynamic2_now_figures[i])) != 0) {
            ui_Dynamic2_dirty_figure[i] = ui_Dynamic2_max_send_count[i];
            ui_Dynamic2_last_figures[i] = ui_Dynamic2_now_figures[i];
        }
    }
#endif
    SCAN_AND_SEND();
}
