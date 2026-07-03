#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>

extern const uint8_t  CRC8_INIT;
extern const uint8_t  CRC8_TAB[256];
extern const uint16_t CRC16_INIT;
extern const uint16_t CRC16_TAB[256];

uint8_t  Get_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint8_t ucCRC8);
uint32_t Verify_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void     Append_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);

uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void     Append_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);

#endif
