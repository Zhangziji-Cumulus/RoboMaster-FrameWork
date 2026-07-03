# PlayerUI 模块设计文档

## 1. 文件结构

```
MDK-ARM/ABModule/PlayerUI/
├── PlayerUI.h / .c       ← FreeRTOS Task：初始化DrawUI + 33ms循环调度
├── RMUI_bsp.h / .c       ← 基础BSP：队列、带宽管理、组包、DMA发送、绘图API
├── DrawUI.h / .c         ← 成品图形层：枚举命名 → ADD/MODIFY动态修改
└── RobotInteract.h / .c  ← 机器人自定义交互数据发送
```

## 2. 各文件职责

| 文件 | 核心内容 |
|------|----------|
| **PlayerUI.h** | 对外统一头文件，暴露 `PlayerUITask`、`PlayerUI_TxCompleteCallback`、`PlayerUI_SetSelfID`、`PlayerUI_SetIsSentryRadar` |
| **PlayerUI.c** | Task主循环：`osDelay(500)` → `InterQueue_Init()` → `DrawUI_Init()` → while(1) { `Inter_Schedule_Run(tick)`; `osDelay(33)` } |
| **RMUI_bsp.h** | 协议常量、`DrawFigure_t/interaction_figure_t/inter_head_t/inter_queue_node_t/inter_fifo_t` 结构体、颜色/图形/操作类型枚举、队列/发送/绘图/调度函数声明 |
| **RMUI_bsp.c** | CRC8/16表、`Figure_To_Proto` 转换、FIFO队列、带宽窗口管理(3720/5120)、`Inter_Schedule_Run`(30Hz限频 + `g_tx_busy` DMA互斥 + 出队打包 + 带宽检查)、绘图API(`Draw_SingleFigure/MultiFigure/DeleteLayer/Text`)、`Referee_SendInterFrame` 组包发送 |
| **DrawUI.h** | `DrawUI_FigureID_t` 枚举(FIG_ID_POWER=0, FIG_ID_HEAT_BAR=1...)、`Figure_SetName` 内联函数、`DrawUI_Init` / `DrawUI_xxxx` 成品API |
| **DrawUI.c** | 全局 `DrawFigure_t` 句柄池、`DrawUI_Init` 中 `OP_ADD` 创建所有图形、各成品API用 `OP_MODIFY` 更新字段 |
| **RobotInteract.h** | `Robot_Send` / `Robot_SendEx` 声明 |
| **RobotInteract.c** | 内部调用 `InterQueue_Push` 入队，数据走同一管道 |

## 3. 图形命名机制（方案三：枚举自增ID）

```c
typedef enum {
    FIG_ID_POWER = 0,       // 功率数字
    FIG_ID_HEAT_BAR,        // 热量条
    FIG_ID_AIM_CIRCLE,      // 自瞄圈
    FIG_ID_BULLET_CNT,      // 弹量
    // ... 按需扩展
} DrawUI_FigureID_t;

static void Figure_SetName(DrawFigure_t *fig, DrawUI_FigureID_t id)
{
    fig->name[0] = (id >> 0) & 0xFF;
    fig->name[1] = (id >> 8) & 0xFF;
    fig->name[2] = (id >> 16) & 0xFF;
}
```

## 4. 动态修改流程

| 阶段 | 操作类型 | 协议值 | 说明 |
|------|---------|-------|------|
| `DrawUI_Init()` | `OP_ADD` | 1 | 选手端创建图形 |
| 后续每次更新 | `OP_MODIFY` | 2 | 选手端找到同名图形更新属性 |
| 删除单个 | `OP_DELETE` | 3 | 删除图形 |
| 清空图层 | `Draw_DeleteLayer` | 0x0100 | 子命令删除整层 |

**原理**：每个成品图形在 `DrawUI_Init` 时注册一个3字节唯一名称（枚举ID小端），后续所有修改/删除都基于这个名称查找，协议保证了同名图形自动覆盖。

## 5. DMA 普通模式发送

```c
// RMUI_bsp.c
static volatile uint8_t g_tx_busy = 0;

void Inter_Schedule_Run(uint32_t tick_now) {
    if (g_tx_busy) return;                    // 上次DMA未完成 → 跳过
    // 带宽窗口刷新 + 30Hz限频 + 出队 + 打包
    g_tx_busy = 1;
    HAL_UART_Transmit_DMA(&huart6, tx_buf, total_len);
}

void PlayerUI_TxCompleteCallback(void) {
    g_tx_busy = 0;   // DMA传输完成
}
```

```c
// RefereeCenter.c
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == huart6.Instance)
        PlayerUI_TxCompleteCallback();
}
```

## 6. 数据流总图

```
应用层 (Chassis/Gimbal/Auto Task)
  ├─ DrawUI_PowerDisplay(1234) → g_fig.param_a=1234, op=MODIFY
  │   └─ Draw_Figure() → InterQueue_Push
  └─ Robot_Send(recv_id, data, len)
      └─ InterQueue_Push
              │
              ▼
┌───────────────────────────────────────┐
│       PlayerUI Task (33ms ≈ 30Hz)     │
│  Inter_Schedule_Run:                  │
│    1. g_tx_busy ? 跳过                │
│    2. 带宽窗口1秒刷新                  │
│    3. 距上次发送≥33ms?                │
│    4. 出队 → 打包0x0301帧             │
│    5. 带宽未超限?                      │
│    6. DMA发送 + g_tx_busy=1           │
└───────────────┬───────────────────────┘
                │ DMA Normal
                ▼
          huart6 TX
                │
                ▼
      TxCpltCallback → g_tx_busy = 0
```

## 7. 协议约束（来自 RM2026 通信协议 V2.0.0）

- 整包最大 127 字节（帧头5 + CMD_ID2 + 数据段最大112 + CRC16 2 + 交互头6 = 127）
- 机器人交互数据内容数据段最大 112 字节
- 带宽限制：每1000ms
  - 英雄/工程/步兵/空中/飞镖：≤ 3720 字节
  - 雷达/哨兵：≤ 5120 字节
- `CMD_ROBOT_INTER` (0x0301) 上行频率上限 30Hz → 最小间隔 33ms

## 8. PlayerUI Task 实现骨架

```c
void PlayerUITask(void *argument)
{
    osDelay(500);
    InterQueue_Init();
    DrawUI_Init();      // 所有图形 OP_ADD

    for (;;) {
        uint32_t tick = osKernelGetTickCount();
        Inter_Schedule_Run(tick);
        osDelay(33);
        // 栈检测: uxTaskGetStackHighWaterMark(NULL)
    }
}
```

## 9. 对外暴露接口（PlayerUI.h）

```c
// Task
void PlayerUITask(void *argument);
void PlayerUI_TxCompleteCallback(void);

// 配置
void PlayerUI_SetSelfID(uint16_t robot_id);
void PlayerUI_SetIsSentryRadar(uint8_t is_sentry_radar);

// 绘图（由DrawUI实现，这里仅转发声明）
void DrawUI_Init(void);
void DrawUI_PowerDisplay(uint16_t power);
void DrawUI_HeatBar(uint16_t heat, uint16_t limit);
void DrawUI_AutoAimRing(float angle);
void DrawUI_BulletCount(uint16_t count);
void DrawUI_ClearLayer(uint8_t layer);

// 机器人交互
uint8_t Robot_Send(uint16_t recv_id, uint8_t *data, uint16_t len);
uint8_t Robot_SendEx(uint16_t sub_cmd_id, uint16_t recv_id, uint8_t *data, uint16_t len);
```