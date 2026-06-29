#include "Dual_Board_Transmit.h"
#include "bsp_CAN.h"
#include <string.h>

#if(BOARD_MODE == BOARD_MODE_DUAL)

// ==================== 内部缓冲区（用于多帧数据组装）====================

#define MAX_STRUCT_SIZE 256  // 最大支持256字节结构体

static uint8_t g_rxBuffer[MAX_STRUCT_SIZE];  // 接收缓冲区
static uint16_t g_rxOffset = 0;              // 当前写入偏移
static uint16_t g_rxTotalSize = 0;           // 期望的总大小
static uint32_t g_rxExtId = 0;               // 当前接收的扩展帧ID
static uint8_t g_rxFrameCount = 0;           // 已接收帧数

/**
 * @brief  【发送】发送结构体数据（自动分包）
 */
bool DualBoard_SendStruct(
    CAN_HandleTypeDef *hcan,
    uint32_t extId,
    const void *pData,
    uint16_t dataSize
)
{
    if (hcan == NULL || pData == NULL || dataSize == 0) {
        return false;
    }
    
    if (dataSize > MAX_STRUCT_SIZE) {
        return false;
    }
    
    // 计算需要多少帧（每帧8字节）
    uint8_t totalFrames = (dataSize + 7) / 8;
    
    // 逐帧发送
    for (uint8_t i = 0; i < totalFrames; i++) {
        uint16_t offset = i * 8;
        uint8_t frameLen = 8;
        
        // 最后一帧可能不足8字节
        if (offset + frameLen > dataSize) {
            frameLen = dataSize - offset;
        }
        
        // 构建扩展帧ID：高24位为原始ID，低8位为帧序号
        uint32_t txExtId = (extId << 8) | (i + 1);
        
        // 准备发送数据
        uint8_t txData[8] = {0};
        memcpy(txData, (const uint8_t *)pData + offset, frameLen);
        
        // 【关键修复】直接使用HAL库发送扩展帧
        CAN_TxHeaderTypeDef TxHeader;
        uint32_t TxMailbox;
        
        TxHeader.ExtId = txExtId;           // 扩展帧ID
        TxHeader.IDE = CAN_ID_EXT;          // 扩展帧标识
        TxHeader.RTR = CAN_RTR_DATA;        // 数据帧
        TxHeader.DLC = 8;                   // 数据长度固定8字节
        TxHeader.TransmitGlobalTime = DISABLE;
        
        // 等待发送邮箱空闲
        while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
            osDelay(1);
        }
        
        // 添加到发送邮箱并发送
        if (HAL_CAN_AddTxMessage(hcan, &TxHeader, txData, &TxMailbox) != HAL_OK) {
            return false;
        }
        
        // 短暂延时避免总线拥塞
        osDelay(1);
    }
    
    return true;
}

/**
 * @brief  【接收】解析CAN接收到的结构体数据（带ID过滤）
 * @param  pHeader CAN接收帧头
 * @param  RxData 接收到的8字节数据
 * @param  expectedId 期望的消息ID（用于过滤）
 * @param  pOutStruct 输出结构体指针（自动填充）
 * @param  outStructSize 输出结构体大小
 * @retval true=成功解析并填充, false=不是目标数据或解析失败
 */
bool DualBoard_ParseStruct(
    CAN_RxHeaderTypeDef *pHeader,
    const uint8_t *RxData,
    uint32_t expectedId,
    void *pOutStruct,
    uint16_t outStructSize
)
{
    if (pHeader == NULL || RxData == NULL || pOutStruct == NULL) {
        return false;
    }
    
    // 只处理扩展帧
    if (pHeader->IDE != CAN_ID_EXT) {
        return false;
    }
    
    // 提取原始ID和帧序号
    uint32_t extId = pHeader->ExtId;
    uint8_t frameSeq = extId & 0xFF;        // 低8位是帧序号
    uint32_t baseId = extId >> 8;           // 高24位是原始ID
    
    // 【关键】ID过滤：只处理匹配的ID
    if (baseId != expectedId) {
        return false;
    }
    
    // 帧序号从1开始
    if (frameSeq == 0) {
        return false;
    }
    
    // 如果是第一帧，初始化接收状态
    if (frameSeq == 1) {
        g_rxOffset = 0;
        g_rxFrameCount = 0;
        g_rxExtId = baseId;
        memset(g_rxBuffer, 0, sizeof(g_rxBuffer));
    }
    
    // 检查是否是同一包数据（相同的baseId）
    if (baseId != g_rxExtId) {
        return false;
    }
    
    // 检查帧序号是否连续
    if (frameSeq != g_rxFrameCount + 1) {
        // 丢包了，重置
        g_rxOffset = 0;
        g_rxFrameCount = 0;
        return false;
    }
    
    // 拷贝当前帧数据到缓冲区
    uint16_t copyLen = 8;
    if (g_rxOffset + copyLen > MAX_STRUCT_SIZE) {
        copyLen = MAX_STRUCT_SIZE - g_rxOffset;
    }
    memcpy(&g_rxBuffer[g_rxOffset], RxData, copyLen);
    
    g_rxOffset += copyLen;
    g_rxFrameCount++;
    
    // 判断是否是最后一帧（通过检查下一帧是否存在来判断）
    // 简化策略：如果接收的数据已经达到结构体大小，认为接收完成
    if (g_rxOffset >= outStructSize) {
        // 拷贝到输出结构体
        memcpy(pOutStruct, g_rxBuffer, outStructSize);
        
        // 重置状态
        g_rxOffset = 0;
        g_rxFrameCount = 0;
        
        return true;
    }
    
    // 还未接收完整，等待下一帧
    return false;
}

#endif /* BOARD_MODE == BOARD_MODE_DUAL */

