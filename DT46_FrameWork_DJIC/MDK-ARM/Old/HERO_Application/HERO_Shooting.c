#include "HERO_Shooting.h"

//flag
void ShootingVel_Calc(float TSSpeed,ShootingVel_t* pSV,uint8_t flag)
{
	if(flag == 3)
	{
		pSV->FriWhell_Radius = (FW_RADIUS_MM / 1000.0f);
		pSV->T_rpm = calc_wheel_rpm(TSSpeed,pSV->FriWhell_Radius);
	
		pSV->UP_Lrpm   = -pSV->T_rpm;
		pSV->UP_Rrpm   = -pSV->T_rpm;
		pSV->Dowm_Mrpm = -pSV->T_rpm;
	}
	else
	{
		pSV->UP_Lrpm   = 0;
		pSV->UP_Rrpm   = 0;
		pSV->Dowm_Mrpm = 0;
	}
}



