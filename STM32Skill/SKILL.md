---
name: STM32_Skill
description: 'STM32 DMA空闲中断+乒乓双缓冲UART接收方案。适用于高频串口通信、定长/变长帧协议、嵌入式实时系统。'
---

# STM32 DMA 空闲中断 + 乒乓双缓冲 UART 接收

## When to Use
- STM32 平台高频串口接收（≥115200 bps）
- 需要零丢帧、不停止 DMA 的通信场景
- 定长结构体或变长帧协议解析

## Architecture

```
DMA ──┬──▶ Rx_Buf[0]  ◀── DMA当前写入
      └──▶ Rx_Buf[1]  ◀── 上帧完成
               │ 空闲中断 memcpy
               ▼
          Rx_ParseBuf ──▶ 协议状态机解析
```

## Data Structures

- `Rx_Buf[2]` — 乒乓双缓冲，DMA 循环写入
- `Rx_ActiveBuf` — 当前 DMA 使用索引（0/1），`^= 1U` 切换
- `Rx_ParseBuf` — 独立解析缓存，避免 DMA 竞争
- `Rx_ParseLen` — 本次实际接收字节长度

## Procedure

### 1. 初始化
```c
Rx_ActiveBuf = 0;
memset(Rx_Buf, 0, sizeof(Rx_Buf));
HAL_UART_Receive_DMA(&huart, Rx_Buf[0], BUF_SIZE);
__HAL_UART_ENABLE_IT(&huart, UART_IT_IDLE);
```

### 2. 空闲中断处理（核心 — 不停止 DMA）
```c
void UART_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart);
    if (__HAL_UART_GET_FLAG(&huart, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart);

        // 变长帧：BUF_SIZE - __HAL_DMA_GET_COUNTER(huart.hdmarx)
        uint16_t len = BUF_SIZE - __HAL_DMA_GET_COUNTER(huart.hdmarx);
        memcpy(Rx_ParseBuf, Rx_Buf[Rx_ActiveBuf], len);
        Rx_ParseLen = len;

        Rx_ActiveBuf ^= 1U;                     // 乒乓切换
        HAL_UART_Receive_DMA(&huart, Rx_Buf[Rx_ActiveBuf], BUF_SIZE);

        ProcessReceivedData(Rx_ParseBuf, Rx_ParseLen);
    }
}
```

### 3. 解析处理
- **定长帧**：memcpy 到结构体后直接校验使用
- **变长帧**：逐字节喂给协议状态机

## Project Examples
- `ACApplication/Auto/AutoAim.c` — 自瞄定长通信
- `ABModule/Referee/RefereeCenter.c` — 裁判系统变长协议

## Key Benefits
- ✅ DMA 永不停止，零丢帧
- ✅ 双缓冲隔离，无数据竞争
- ✅ 独立解析缓存，安全可靠
- ✅ 支持定长/变长两种帧格式
