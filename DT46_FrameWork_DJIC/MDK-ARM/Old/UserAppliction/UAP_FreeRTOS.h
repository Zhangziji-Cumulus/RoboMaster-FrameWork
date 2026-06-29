#ifndef __UAP_FREERTOS_H
#define __UAP_FREERTOS_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"


#include "DJI_Motor_CAN.h"
#include "DJI_Motor_Contral.h"
#include "MY_LED.h"
#include "sound_effects_task.h" 
#include "HOTRC_HT10A.h"
#include "Dual_board_Transmit.h"

void UAP_FreeRTOS_Init(void);

#endif
