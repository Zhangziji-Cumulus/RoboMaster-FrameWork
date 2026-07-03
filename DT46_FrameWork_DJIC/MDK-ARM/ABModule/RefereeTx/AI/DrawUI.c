// #include "DrawUI.h"

// #define DRAWUI_FIG_COUNT FIG_ID_LAST

// static DrawFigure_t g_fig_pool[DRAWUI_FIG_COUNT];

// void DrawUI_Init(void)
// {
//     for (uint8_t i = 0; i < DRAWUI_FIG_COUNT; i++)
//     {
//         DrawFigure_t *fig = &g_fig_pool[i];
//         memset(fig, 0, sizeof(DrawFigure_t));
//         Figure_SetName(fig, (DrawUI_FigureID_t)i);
//         fig->op_type = OP_ADD;
//         fig->layer = 0;
//         fig->color = COLOR_CYAN;
//         fig->line_width = 2;
//     }

//     g_fig_pool[FIG_ID_POWER].fig_type = FIG_INT_NUM;
//     g_fig_pool[FIG_ID_POWER].x0 = 100;
//     g_fig_pool[FIG_ID_POWER].y0 = 100;
//     g_fig_pool[FIG_ID_POWER].param_a = 1;
//     g_fig_pool[FIG_ID_POWER].layer = 1;

//     g_fig_pool[FIG_ID_HEAT_BAR].fig_type = FIG_RECT;
//     g_fig_pool[FIG_ID_HEAT_BAR].x0 = 50;
//     g_fig_pool[FIG_ID_HEAT_BAR].y0 = 50;
//     g_fig_pool[FIG_ID_HEAT_BAR].x1 = 300;
//     g_fig_pool[FIG_ID_HEAT_BAR].y1 = 20;
//     g_fig_pool[FIG_ID_HEAT_BAR].layer = 0;

//     g_fig_pool[FIG_ID_AIM_CIRCLE].fig_type = FIG_CIRCLE;
//     g_fig_pool[FIG_ID_AIM_CIRCLE].x0 = 480;
//     g_fig_pool[FIG_ID_AIM_CIRCLE].y0 = 540;
//     g_fig_pool[FIG_ID_AIM_CIRCLE].param_a = 50;
//     g_fig_pool[FIG_ID_AIM_CIRCLE].color = COLOR_GREEN;
//     g_fig_pool[FIG_ID_AIM_CIRCLE].layer = 2;

//     g_fig_pool[FIG_ID_BULLET_CNT].fig_type = FIG_INT_NUM;
//     g_fig_pool[FIG_ID_BULLET_CNT].x0 = 100;
//     g_fig_pool[FIG_ID_BULLET_CNT].y0 = 150;
//     g_fig_pool[FIG_ID_BULLET_CNT].param_a = 1;
//     g_fig_pool[FIG_ID_BULLET_CNT].color = COLOR_YELLOW;
//     g_fig_pool[FIG_ID_BULLET_CNT].layer = 1;

//     for (uint8_t i = 0; i < DRAWUI_FIG_COUNT; i++)
//     {
//         Draw_SingleFigure(&g_fig_pool[i], 0x0101);
//     }
// }

// void DrawUI_PowerDisplay(uint16_t power)
// {
//     DrawFigure_t *fig = &g_fig_pool[FIG_ID_POWER];
//     fig->op_type = OP_MODIFY;
//     fig->param_a = power;
//     Draw_SingleFigure(fig, 0x0101);
// }

// void DrawUI_HeatBar(uint16_t heat, uint16_t limit)
// {
//     DrawFigure_t *fig = &g_fig_pool[FIG_ID_HEAT_BAR];
//     fig->op_type = OP_MODIFY;
//     uint16_t fill_width = 0;
//     if (limit > 0)
//     {
//         fill_width = (heat * 250) / limit;
//         if (fill_width > 250)
//             fill_width = 250;
//     }
//     fig->x1 = 50 + fill_width;
//     fig->color = (heat > (limit * 80 / 100)) ? COLOR_MAGENTA : COLOR_CYAN;
//     Draw_SingleFigure(fig, 0x0101);
// }

// void DrawUI_AutoAimRing(float angle)
// {
//     DrawFigure_t *fig = &g_fig_pool[FIG_ID_AIM_CIRCLE];
//     fig->op_type = OP_MODIFY;
//     fig->param_a = 50 + angle;
//     if (fig->param_a < 20)
//         fig->param_a = 20;
//     if (fig->param_a > 200)
//         fig->param_a = 200;
//     fig->color = (angle > 10) ? COLOR_MAGENTA : COLOR_GREEN;
//     Draw_SingleFigure(fig, 0x0101);
// }

// void DrawUI_BulletCount(uint16_t count)
// {
//     DrawFigure_t *fig = &g_fig_pool[FIG_ID_BULLET_CNT];
//     fig->op_type = OP_MODIFY;
//     fig->param_a = count;
//     Draw_SingleFigure(fig, 0x0101);
// }

// void DrawUI_ClearLayer(uint8_t layer)
// {
//     Draw_DeleteLayer(0, layer, 0x0101);
// }
