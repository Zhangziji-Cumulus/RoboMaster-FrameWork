#ifndef SHOOTING_TASK_H_
#define SHOOTING_TASK_H_

#include "A_MCommon.h"

__attribute__((used)) void ShootingTask(void *argument);

//初始化函数
void Shooting_Init(void);
//更新状态函数
void Shooting_Update(void);
//异常处理函数
void Shooting_HandleError(void);
//设置模式
void Shooting_SetMode(void);
//更新目标量
void Shooting_RefreshTarget(void);
//计算控制量
void Shooting_CtrlCalc(void);
//发送控制指令
void Shooting_SendCmd(void);

#endif // SHOOTING_TASK_H_
