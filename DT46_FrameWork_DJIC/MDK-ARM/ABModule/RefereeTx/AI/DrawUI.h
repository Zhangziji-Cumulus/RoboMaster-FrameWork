// #ifndef __DRAWUI_H__
// #define __DRAWUI_H__

// #include "RMUI_bsp.h"

// typedef enum
// {
//     FIG_ID_POWER = 0,
//     FIG_ID_HEAT_BAR,
//     FIG_ID_AIM_CIRCLE,
//     FIG_ID_BULLET_CNT,
//     FIG_ID_LAST
// } DrawUI_FigureID_t;

// static inline void Figure_SetName(DrawFigure_t *fig, DrawUI_FigureID_t id)
// {
//     fig->name[0] = (id >> 0) & 0xFF;
//     fig->name[1] = (id >> 8) & 0xFF;
//     fig->name[2] = (id >> 16) & 0xFF;
// }

// void DrawUI_Init(void);
// void DrawUI_PowerDisplay(uint16_t power);
// void DrawUI_HeatBar(uint16_t heat, uint16_t limit);
// void DrawUI_AutoAimRing(float angle);
// void DrawUI_BulletCount(uint16_t count);
// void DrawUI_ClearLayer(uint8_t layer);

// #endif
