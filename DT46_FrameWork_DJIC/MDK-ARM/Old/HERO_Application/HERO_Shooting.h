#ifndef __HERO_SHOOTING_H
#define __HERO_SHOOTING_H

#include "main.h"
#include "My_Math.h"
#include "HERO_Math.h"

#define FW_RADIUS_MM 30 //摩擦轮半径，单位：mm


typedef struct
{
	float FriWhell_Radius;//单位是m
	uint16_t T_rpm;
	
	int16_t UP_Lrpm;
	int16_t UP_Rrpm;
	int16_t Dowm_Mrpm;
	
}ShootingVel_t;

void ShootingVel_Calc(float TSSpeed,ShootingVel_t* SV,uint8_t flag);

#endif
