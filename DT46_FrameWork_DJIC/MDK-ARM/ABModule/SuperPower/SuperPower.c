#include "SuperPower.h"

SuperPower_t SuperPower;

static void SuperPower_CalcVoltPercent(void);

const SuperPower_t* get_superpower_piont(void)
{
    return &SuperPower;
}

//解析超电返回数据
void SuperPower_Process(uint32_t std_id, uint8_t* data)
{
    if (std_id == 0x311) 
    {
        SuperPower.cap_v = (float)*data;

        SuperPower_CalcVoltPercent();
    }
}

//将接受到的电压转化返回成百分比值
static void SuperPower_CalcVoltPercent(void)
{
    SuperPower.Prencent = SuperPower.cap_v / (SUPERPOWER_MAX_VOLT - SUPERPOWER_MIN_VOLT);
}


