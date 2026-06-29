#ifndef ZDT_CAN_BSP_H_
#define ZDT_CAN_BSP_H_

#include "can.h"
#include "stdbool.h"

//发送数据结构体
typedef struct {
	__IO CAN_RxHeaderTypeDef CAN_RxMsg;
	__IO uint8_t rxData[32];
	
	__IO CAN_TxHeaderTypeDef CAN_TxMsg;
	__IO uint8_t txData[32];

	__IO bool rxFrameFlag;
}CAN_t;
//接受数据结构体
typedef struct {
	
	uint16_t current_ma;//电机实际运行电流

	uint8_t     addr;                  // 字节1: 地址
    uint8_t     func_code;             // 字节2: 功能码
    uint8_t     data_len;              // 字节3: 字节数

    uint8_t     param_count;           // 字节4: 参数个数 (0x0C)

    uint16_t    bus_voltage;           // 字节5-6: 总线电压 (0000-FFFF)
    uint16_t    bus_current;            // 字节7-8: 总线电流 (0000-FFFF)
    uint16_t    phase_current;          // 字节9-10: 电机相电流 (0000-FFFF)

    uint16_t    encoder_raw;           // 字节11-12: 编码器原始值 (0000-FFFF)
    uint16_t    encoder_linear;       // 字节13-14: 线性化编码器值 (0000-FFFF)
    int32_t     target_position;       // 字节17-20: 电机目标位置 (00000000-FFFFFFFF)

    uint16_t    realtime_speed;        // 字节23-24: 电机实时转速 (0000-7530)
    int32_t     realtime_position;     // 字节27-30: 电机实时位置 (00000000-FFFFFFFF)

    int32_t     position_error;        // 字节33-36: 电机位置误差 (00000000-FFFFFFFF)

    uint8_t     temperature;            // 字节39: 电机实时温度 (00-FF)

    uint8_t     checksum;              // 字节42: 校验码

	struct{
		
		struct
		{
			uint8_t Ocp_TF;//掉电标志
			uint8_t Otp_TF;//右限位开关的状态
			uint8_t Org_CF;//左限位开关的状态
			uint8_t Cgp_TF;//堵转保护标志
			uint8_t Org_SF;//堵转标志
			uint8_t Cal_Rdy;//位置到达标志
			uint8_t Enc_Rdy;//使能状态标志
		}zero;

		struct
		{
			uint8_t Oac_TF;//掉电标志
			uint8_t Esi_RF;//右限位开关的状态
			uint8_t Esi_LF;//左限位开关的状态
			uint8_t Cgp_TF;//堵转保护标志
			uint8_t Cgi_TF;//堵转标志
			uint8_t Prf_TF;//位置到达标志
			uint8_t Ens_TF;//使能状态标志
		}motor;

	}flag;


}ZDT_FeedBack_t;

extern ZDT_FeedBack_t  ZDT_FeedBack;

void can_SendCmd(__IO uint8_t *cmd, uint8_t len);
void CAN_ZDT_Motor_FeedBack(ZDT_FeedBack_t* ZDT_FeedBack,uint8_t expectedId,CAN_RxHeaderTypeDef *pHeader, uint8_t* rxdata);
void CAN_ZDT_Motor_ALLFeedBack(ZDT_FeedBack_t* ZDT_FeedBack,uint8_t expectedId,CAN_RxHeaderTypeDef *pHeader, uint8_t* rxdata);

#endif // ZDT_CAN_BSP_H_
