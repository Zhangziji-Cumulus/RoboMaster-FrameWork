#include "bsp_Init.h"

/** 
 * @brief 初始化驱动层函数
 * 
 */
void BSP_Init(void)
{
    can_filter_init();
    SBUS_Init(&huart3);
}
