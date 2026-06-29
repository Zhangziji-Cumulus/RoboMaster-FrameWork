#include "ZDT_CAN_Bsp.h"

//** #################################################################################################### **//
//** ================================= ZDT CAN 发送函数(通过CAN2进行发送) ================================= **//
//** #################################################################################################### **//

//

__IO CAN_t CAN1_Instance = {0};

/**
	* @brief   CAN发送多个字节
	* @param   无
	* @retval  无
	*/
void can_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
	static uint32_t TxMailbox; __IO uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;

	// 除去ID地址和功能码后的数据长度
	j = len - 2;

	// 发送数据
	while(i < j)
	{
		// 数据个数
		k = j - i;

		// 填充缓存
		CAN1_Instance.CAN_TxMsg.StdId = 0x00;
		CAN1_Instance.CAN_TxMsg.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)packNum;
		CAN1_Instance.txData[0] = cmd[1];
		CAN1_Instance.CAN_TxMsg.IDE = CAN_ID_EXT;
		CAN1_Instance.CAN_TxMsg.RTR = CAN_RTR_DATA;

		// 小于8字节命令
		if(k < 8)
		{
			for(l=0; l < k; l++,i++) { CAN1_Instance.txData[l + 1] = cmd[i + 2]; } CAN1_Instance.CAN_TxMsg.DLC = k + 1;
		}
		// 大于8字节命令，分包发送，每包数据最多发送8个字节
		else
		{
			for(l=0; l < 7; l++,i++) { CAN1_Instance.txData[l + 1] = cmd[i + 2]; } CAN1_Instance.CAN_TxMsg.DLC = 8;
		}

		// 发送数据
		while(HAL_CAN_AddTxMessage((&hcan2), (CAN_TxHeaderTypeDef *)(&CAN1_Instance.CAN_TxMsg), (uint8_t *)(&CAN1_Instance.txData), (&TxMailbox)) != HAL_OK);

		// 记录发送的第几包的数据
		++packNum;
	}
}

//** #################################################################################################### **//
//** ========================================== 接受数据解析函数 ========================================= **//
//** #################################################################################################### **//

ZDT_FeedBack_t  ZDT_FeedBack;



void CAN_ZDT_Motor_FeedBack(ZDT_FeedBack_t* ZDT_FeedBack,uint8_t expectedId,CAN_RxHeaderTypeDef *pHeader, uint8_t* rxdata)
{
	uint32_t ext_id = pHeader->ExtId;		//获取CAN ID帧
	uint8_t Addr = (ext_id >> 8) & 0xFF;	// 提取设备地址
	uint8_t Packet = ext_id & 0xFF;      	// 提取包标识  
	uint8_t Code = rxdata[0];
	uint8_t checkcode;

	if(Addr == expectedId && Packet == 0)
	{
		switch(Code)
		{
			case 0x27: 
				checkcode = rxdata[3];	
				if(checkcode == 0x6B)
				{
					ZDT_FeedBack->current_ma = rxdata[1] | rxdata[2]; 
					break;
				}
		}
	}
}

void CAN_ZDT_Motor_ALLFeedBack(ZDT_FeedBack_t* ZDT_FeedBack,uint8_t expectedId,CAN_RxHeaderTypeDef *pHeader, uint8_t* rxdata)
{
	uint32_t ext_id = pHeader->ExtId;		//获取CAN ID帧
	uint8_t Addr = (ext_id >> 8) & 0xFF;	// 提取设备地址
	uint8_t Packet = ext_id & 0xFF;      	// 提取包标识
	
	static uint8_t rx_Buff[40] = {0};

	
	
	if(Addr == expectedId)
	{
		memcpy(&rx_Buff[Packet * 8],rxdata,8);
		if(Packet == 4)
		{
			if(rx_Buff[39] == 0x6B)
			{
				ZDT_FeedBack->bus_voltage = rx_Buff[3] | rx_Buff[4];
				ZDT_FeedBack->bus_voltage = rx_Buff[5] | rx_Buff[6];
				ZDT_FeedBack->current_ma  = rx_Buff[7] | rx_Buff[8];
			}
		}
	}
}
