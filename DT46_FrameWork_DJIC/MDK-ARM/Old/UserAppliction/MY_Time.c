#include "MY_Time.h"





/**
* @brief:自定定时器回调函数
* @param:
* @return:
**/
uint16_t timecount;
void MY_TIM2_Callback(void)
{
	
//	if(timecount > 1000)
//	{
//		Motor_DJI_Angle_SingleContral(0.0f);
//	}
//	else if(timecount > 2000)
//	{
//		timecount = 0;
//		Motor_DJI_Angle_SingleContral(100.0f);
//	}
	
	timecount++;
	
	HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,LED_Flash(100,1));
}
