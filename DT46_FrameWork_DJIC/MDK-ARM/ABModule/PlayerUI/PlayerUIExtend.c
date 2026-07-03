#include "PlayerUIExtend.h"

// // 全局定义
// osMutexId_t uart6_tx_mtx;
// const osMutexAttr_t uart6_mtx_attr = {
//   .name = "uart6_tx_mtx",
//   .attr_bits = osMutexRecursive, // 允许同一任务重复获取，防止死锁
//   .cb_mem = NULL,
//   .cb_size = 0U
// };

// void PlayerUIExtend_Init(void)
// {
//     uart6_tx_mtx = osMutexNew(&uart6_mtx_attr);
//     if(uart6_tx_mtx == NULL)
//     {
//     Error_Handler(); // 互斥量创建失败
//     }
// }