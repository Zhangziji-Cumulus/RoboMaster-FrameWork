#include "UAP_Init.h"
#include "DJI_Motor_Contral.h"
#include "HOTRC_HT10A.h"
#include "usart.h"

#include "CAN_PART.h"
#include "UAP_FreeRTOS.h"

void UAP_Init(void)
{
	can_filter_init();
	Motor_Init();
	SBUS_Init(&huart3);
}
