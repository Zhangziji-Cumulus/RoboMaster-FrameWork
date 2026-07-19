#ifndef SUPERPOWER_H_
#define SUPERPOWER_H_

#include "A_CommonSystem.h"

#define SUPERPOWER_MAX_VOLT     24.0f
#define SUPERPOWER_MIN_VOLT     8.0f

typedef struct{

    float cap_v;    //超级电容电压

    float Prencent;

}SuperPower_t;

//解析超电返回数据
void SuperPower_Process(uint32_t std_id, uint8_t* data);
//获取超电结构体只读指针
const SuperPower_t* get_superpower_piont(void)

#endif // SUPERPOWER_H_