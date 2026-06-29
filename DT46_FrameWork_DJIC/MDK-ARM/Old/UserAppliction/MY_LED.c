#include "MY_LED.h"

/**
  * @brief  LED频闪函数
  * @param  FlashTime 每次频闪间隔时间/ms，TimeUint函数每多少ms执行一次 
  * @retval LED_Flag
  */

uint8_t LED_Flash(uint16_t FlashTime,uint16_t TimeUint)
{
		static uint8_t LED_Flag = 0;
		static uint16_t count = 0;
		count++;
		if(count >= (FlashTime / TimeUint))
		{
			count = 0;
			LED_Flag = (LED_Flag ? 0 : 1);
			return LED_Flag;
		}
		return LED_Flag;
}


