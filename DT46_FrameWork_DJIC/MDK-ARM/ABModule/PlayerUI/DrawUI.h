#ifndef DRAWUI_H_
#define DRAWUI_H_

/* 更UI绘制有关的解释 */
/*
    基于南航开源RoboMaster UI Designer生成的API进行设计，
    其中其ui_interface.c/.h 已经做了修改适配。每次生成只需下载除了ui_interface.c/.h文件的内容即可

    重要修改：
    1、发送宏定义
    #define SEND_MESSAGE(message, len) do { \
    if (HAL_UART_Transmit(&huart6, message, len, 50) != HAL_OK) { \
        huart6.gState = HAL_UART_STATE_READY; \
    } \
    } while(0)
     2、


    颜色：0红蓝/1黄/2绿/3橙/4紫红/5粉/6青/7黑/8白
*/


#include "ui.h"


void DrawUI_Init(void);

void DrawUI_Update(void);

#endif // DRAWUI_H_