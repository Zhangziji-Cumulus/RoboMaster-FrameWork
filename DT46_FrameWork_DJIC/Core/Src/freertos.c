/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "INS_task.h"
#include "sound_effects_task.h" 
#include "bsp_SBUS.h" 

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for imuTask */
osThreadId_t imuTaskHandle;
const osThreadAttr_t imuTask_attributes = {
  .name = "imuTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for buzr */
osThreadId_t buzrHandle;
const osThreadAttr_t buzr_attributes = {
  .name = "buzr",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DualBoard */
osThreadId_t DualBoardHandle;
uint32_t Dual_Board_TranBuffer[ 256 ];
osStaticThreadDef_t Dual_Board_TranControlBlock;
const osThreadAttr_t DualBoard_attributes = {
  .name = "DualBoard",
  .cb_mem = &Dual_Board_TranControlBlock,
  .cb_size = sizeof(Dual_Board_TranControlBlock),
  .stack_mem = &Dual_Board_TranBuffer[0],
  .stack_size = sizeof(Dual_Board_TranBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for Chassis */
osThreadId_t ChassisHandle;
uint32_t ChassisBuffer[ 128 ];
osStaticThreadDef_t ChassisControlBlock;
const osThreadAttr_t Chassis_attributes = {
  .name = "Chassis",
  .cb_mem = &ChassisControlBlock,
  .cb_size = sizeof(ChassisControlBlock),
  .stack_mem = &ChassisBuffer[0],
  .stack_size = sizeof(ChassisBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for Gimbal */
osThreadId_t GimbalHandle;
uint32_t GimbalBuffer[ 128 ];
osStaticThreadDef_t GimbalControlBlock;
const osThreadAttr_t Gimbal_attributes = {
  .name = "Gimbal",
  .cb_mem = &GimbalControlBlock,
  .cb_size = sizeof(GimbalControlBlock),
  .stack_mem = &GimbalBuffer[0],
  .stack_size = sizeof(GimbalBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for Shooting */
osThreadId_t ShootingHandle;
uint32_t ShootingBuffer[ 128 ];
osStaticThreadDef_t ShootingControlBlock;
const osThreadAttr_t Shooting_attributes = {
  .name = "Shooting",
  .cb_mem = &ShootingControlBlock,
  .cb_size = sizeof(ShootingControlBlock),
  .stack_mem = &ShootingBuffer[0],
  .stack_size = sizeof(ShootingBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for CMDUpdate */
osThreadId_t CMDUpdateHandle;
uint32_t CMDUpdateBuffer[ 128 ];
osStaticThreadDef_t CMDUpdateControlBlock;
const osThreadAttr_t CMDUpdate_attributes = {
  .name = "CMDUpdate",
  .cb_mem = &CMDUpdateControlBlock,
  .cb_size = sizeof(CMDUpdateControlBlock),
  .stack_mem = &CMDUpdateBuffer[0],
  .stack_size = sizeof(CMDUpdateBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for SBUS */
osThreadId_t SBUSHandle;
uint32_t SBUSBuffer[ 128 ];
osStaticThreadDef_t SBUSControlBlock;
const osThreadAttr_t SBUS_attributes = {
  .name = "SBUS",
  .cb_mem = &SBUSControlBlock,
  .cb_size = sizeof(SBUSControlBlock),
  .stack_mem = &SBUSBuffer[0],
  .stack_size = sizeof(SBUSBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for RCUpdate */
osThreadId_t RCUpdateHandle;
uint32_t RCUpdateBuffer[ 128 ];
osStaticThreadDef_t RCUpdateControlBlock;
const osThreadAttr_t RCUpdate_attributes = {
  .name = "RCUpdate",
  .cb_mem = &RCUpdateControlBlock,
  .cb_size = sizeof(RCUpdateControlBlock),
  .stack_mem = &RCUpdateBuffer[0],
  .stack_size = sizeof(RCUpdateBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for DJIMotorCheck */
osThreadId_t DJIMotorCheckHandle;
uint32_t DJIMotorCheckBuffer[ 128 ];
osStaticThreadDef_t DJIMotorCheckControlBlock;
const osThreadAttr_t DJIMotorCheck_attributes = {
  .name = "DJIMotorCheck",
  .cb_mem = &DJIMotorCheckControlBlock,
  .cb_size = sizeof(DJIMotorCheckControlBlock),
  .stack_mem = &DJIMotorCheckBuffer[0],
  .stack_size = sizeof(DJIMotorCheckBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ShootingPushRod */
osThreadId_t ShootingPushRodHandle;
uint32_t PushRodBuffer[ 128 ];
osStaticThreadDef_t PushRodControlBlock;
const osThreadAttr_t ShootingPushRod_attributes = {
  .name = "ShootingPushRod",
  .cb_mem = &PushRodControlBlock,
  .cb_size = sizeof(PushRodControlBlock),
  .stack_mem = &PushRodBuffer[0],
  .stack_size = sizeof(PushRodBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for ShootingLoad */
osThreadId_t ShootingLoadHandle;
uint32_t ShootingLoadBuffer[ 128 ];
osStaticThreadDef_t ShootingLoadControlBlock;
const osThreadAttr_t ShootingLoad_attributes = {
  .name = "ShootingLoad",
  .cb_mem = &ShootingLoadControlBlock,
  .cb_size = sizeof(ShootingLoadControlBlock),
  .stack_mem = &ShootingLoadBuffer[0],
  .stack_size = sizeof(ShootingLoadBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for AutoAim */
osThreadId_t AutoAimHandle;
uint32_t AutoBuffer[ 128 ];
osStaticThreadDef_t AutoControlBlock;
const osThreadAttr_t AutoAim_attributes = {
  .name = "AutoAim",
  .cb_mem = &AutoControlBlock,
  .cb_size = sizeof(AutoControlBlock),
  .stack_mem = &AutoBuffer[0],
  .stack_size = sizeof(AutoBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for Referee */
osThreadId_t RefereeHandle;
uint32_t RefereeBuffer[ 128 ];
osStaticThreadDef_t RefereeControlBlock;
const osThreadAttr_t Referee_attributes = {
  .name = "Referee",
  .cb_mem = &RefereeControlBlock,
  .cb_size = sizeof(RefereeControlBlock),
  .stack_mem = &RefereeBuffer[0],
  .stack_size = sizeof(RefereeBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void DualBoardTask(void *argument);
void ChassisTask(void *argument);
void GimbalTask(void *argument);
void ShootingTask(void *argument);
void CMDUpdateTask(void *argument);
void SBUSTask(void *argument);
void RCUpdateTask(void *argument);
void DJIMotorCheckTask(void *argument);
void ShootingPushRodTask(void *argument);
void ShootingLoadTask(void *argument);
void AutoAimTask(void *argument);
void RefereeTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of imuTask */
  imuTaskHandle = osThreadNew(INS_task, NULL, &imuTask_attributes);

  /* creation of buzr */
  buzrHandle = osThreadNew(buzzer_effects_task, NULL, &buzr_attributes);

  /* creation of DualBoard */
  DualBoardHandle = osThreadNew(DualBoardTask, NULL, &DualBoard_attributes);

  /* creation of Chassis */
  ChassisHandle = osThreadNew(ChassisTask, NULL, &Chassis_attributes);

  /* creation of Gimbal */
  GimbalHandle = osThreadNew(GimbalTask, NULL, &Gimbal_attributes);

  /* creation of Shooting */
  ShootingHandle = osThreadNew(ShootingTask, NULL, &Shooting_attributes);

  /* creation of CMDUpdate */
  CMDUpdateHandle = osThreadNew(CMDUpdateTask, NULL, &CMDUpdate_attributes);

  /* creation of SBUS */
  SBUSHandle = osThreadNew(SBUSTask, NULL, &SBUS_attributes);

  /* creation of RCUpdate */
  RCUpdateHandle = osThreadNew(RCUpdateTask, NULL, &RCUpdate_attributes);

  /* creation of DJIMotorCheck */
  DJIMotorCheckHandle = osThreadNew(DJIMotorCheckTask, NULL, &DJIMotorCheck_attributes);

  /* creation of ShootingPushRod */
  ShootingPushRodHandle = osThreadNew(ShootingPushRodTask, NULL, &ShootingPushRod_attributes);

  /* creation of ShootingLoad */
  ShootingLoadHandle = osThreadNew(ShootingLoadTask, NULL, &ShootingLoad_attributes);

  /* creation of AutoAim */
  AutoAimHandle = osThreadNew(AutoAimTask, NULL, &AutoAim_attributes);

  /* creation of Referee */
  RefereeHandle = osThreadNew(RefereeTask, NULL, &Referee_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
	//UAP_FreeRTOS_Init();
	
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	
//	static uint16_t timecounttest = 0;
//	buzzer_t *buzzer = get_buzzer_effect_point();
//	buzzer->work = FALSE;
//	buzzer->work = TRUE;
	
  for(;;)
  {
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_DualBoardTask */
/**
* @brief Function implementing the DualBoard thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DualBoardTask */
__weak void DualBoardTask(void *argument)
{
  /* USER CODE BEGIN DualBoardTask */
  
  /* Infinite loop */
  for(;;)
  {
		osDelay(1);
  }
  /* USER CODE END DualBoardTask */
}

/* USER CODE BEGIN Header_ChassisTask */
/**
* @brief Function implementing the Chassis thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ChassisTask */
__weak void ChassisTask(void *argument)
{
  /* USER CODE BEGIN ChassisTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ChassisTask */
}

/* USER CODE BEGIN Header_GimbalTask */
/**
* @brief Function implementing the Gimbal thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GimbalTask */
__weak void GimbalTask(void *argument)
{
  /* USER CODE BEGIN GimbalTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END GimbalTask */
}

/* USER CODE BEGIN Header_ShootingTask */
/**
* @brief Function implementing the Shooting thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ShootingTask */
__weak void ShootingTask(void *argument)
{
  /* USER CODE BEGIN ShootingTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ShootingTask */
}

/* USER CODE BEGIN Header_CMDUpdateTask */
/**
* @brief Function implementing the CMDUpdate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CMDUpdateTask */
__weak void CMDUpdateTask(void *argument)
{
  /* USER CODE BEGIN CMDUpdateTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CMDUpdateTask */
}

/* USER CODE BEGIN Header_SBUSTask */
/**
* @brief Function implementing the SBUS thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SBUSTask */
__weak void SBUSTask(void *argument)
{
  /* USER CODE BEGIN SBUSTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END SBUSTask */
}

/* USER CODE BEGIN Header_RCUpdateTask */
/**
* @brief Function implementing the RCUpdate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RCUpdateTask */
__weak void RCUpdateTask(void *argument)
{
  /* USER CODE BEGIN RCUpdateTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END RCUpdateTask */
}

/* USER CODE BEGIN Header_DJIMotorCheckTask */
/**
* @brief Function implementing the DJIMotorCheck thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DJIMotorCheckTask */
__weak void DJIMotorCheckTask(void *argument)
{
  /* USER CODE BEGIN DJIMotorCheckTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DJIMotorCheckTask */
}

/* USER CODE BEGIN Header_ShootingPushRodTask */
/**
* @brief Function implementing the ShootingPushRod thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ShootingPushRodTask */
__weak void ShootingPushRodTask(void *argument)
{
  /* USER CODE BEGIN ShootingPushRodTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ShootingPushRodTask */
}

/* USER CODE BEGIN Header_ShootingLoadTask */
/**
* @brief Function implementing the ShootingLoad thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ShootingLoadTask */
__weak void ShootingLoadTask(void *argument)
{
  /* USER CODE BEGIN ShootingLoadTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ShootingLoadTask */
}

/* USER CODE BEGIN Header_AutoAimTask */
/**
* @brief Function implementing the AutoAim thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AutoAimTask */
__weak void AutoAimTask(void *argument)
{
  /* USER CODE BEGIN AutoAimTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AutoAimTask */
}

/* USER CODE BEGIN Header_RefereeTask */
/**
* @brief Function implementing the Referee thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RefereeTask */
__weak void RefereeTask(void *argument)
{
  /* USER CODE BEGIN RefereeTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END RefereeTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

