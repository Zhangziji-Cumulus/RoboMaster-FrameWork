#include "UAP_FreeRTOS.h"

extern osMessageQueueId_t HEROChassisHandle;
extern osMessageQueueId_t HEROGimbalHandle;
extern osMessageQueueId_t HEROShootingHandle;
extern osMessageQueueId_t HERODialHandle;

extern DJI_MotorFeedback_t DJI_MFeedback[8];
extern HOTRC_Ctl_t RC_Ctl;

uint8_t g_motor_run_enable  = 0;

uint8_t last_switch = 0;

void UAP_FreeRTOS_Init(void)
{
	//FreeRTOS 任务不同板子之前挂起或停止
	uint8_t board_id = BOARD_ID;
	if(board_id == GIMBAL_BOAD)
	{
		 if(HEROChassisHandle != NULL)
     {
        vTaskDelete(HEROChassisHandle);
        HEROChassisHandle = NULL;
     }
		 if(HERODialHandle != NULL)
     {
        vTaskDelete(HERODialHandle);
        HERODialHandle = NULL; 
     }
	}
	else if(board_id == CHASSIS_BOAD)
	{
		 if(HEROGimbalHandle != NULL)
     {
        vTaskDelete(HEROGimbalHandle);
        HEROGimbalHandle = NULL; 
     }
		 if(HEROShootingHandle != NULL)
     {
        vTaskDelete(HEROShootingHandle);
        HEROShootingHandle = NULL; 
     }
	}
	
	last_switch = RC_Ctl.Switch.S2_L;
}

static UBaseType_t remain_StartRealTime_TASK;
__attribute__((used)) void StartRealTime_TASK(void *argument)
{
	 remain_StartRealTime_TASK = uxTaskGetStackHighWaterMark(NULL);
	 buzzer_t *buzzer = get_buzzer_effect_point();

	 // ==================== 核心安全变量 ====================
	 uint8_t  last_switch        = 0;    // 上一次拨杆值
	 uint8_t  system_allowed     = 0;    // 【关键】是否允许运行
	 uint32_t power_on_lock_time = 0;    // 上电延时等SBUS

	 // 上电强制停机
	 g_motor_run_enable = 0;
	
	 vTaskDelay(100);  // 直接等SBUS稳定，最简单有效

	 // 读取上电时的初始拨杆值
	 last_switch = RC_Ctl.Switch.S2_L;

	 for(;;)
	 {
		 
		uint8_t current_switch = RC_Ctl.Switch.S2_L;

		// ====================== 核心逻辑 ======================
		// 只有 【从非3 → 拨到3】 才允许启动
		// ======================================================
		if(current_switch != last_switch)
		{
			last_switch = current_switch;

			// 拨到 3 → 允许启动
			if(current_switch == HOTRC_SWITCH_DOWN)
			{
				system_allowed = 1;
				g_motor_run_enable = 1;
			}
			// 拨到非3 → 禁止
			else
			{
				system_allowed = 0;
				g_motor_run_enable = 0;
			}
		}
		// ====================== 强制安全 ======================
		// 只要不在3 → 绝对停机
		if(current_switch != HOTRC_SWITCH_DOWN)
		{
			g_motor_run_enable = 0;

		}
				
		// 提示音
		if(RC_Ctl.Knob.KL > 992 - 20 && RC_Ctl.Knob.KL < 992 + 20)
		{
			buzzer->sound_effect = D_B_B_;
		}
		
		vTaskDelay(50);
	 }
}

static UBaseType_t Start_DJI_RecieveData_TASK;
__attribute__((used)) void Start_DJI_RecieveData(void *argument)
{
  for(;;)
  { Start_DJI_RecieveData_TASK = uxTaskGetStackHighWaterMark(NULL);
		
    SBUS_Process();
		
    osDelay(1);
  }
}

