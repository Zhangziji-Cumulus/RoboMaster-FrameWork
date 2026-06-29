#ifndef CHASSIS_TASK_H_
#define CHASSIS_TASK_H_

#include "A_MCommon.h"
#include "Robot_Hero_Config.h"

__attribute__((used)) void ChassisTask(void *argument);

//初始化函数
void Chassis_Init(void);
//更新状态函数
void Chassis_Update(void);
//异常处理函数
void Chassis_HandleError(void);
//设置模式
void Chassis_SetMode(void);
//更新目标量
void Chassis_RefreshTarget(void);
//计算控制量
void Chassis_CtrlCalc(void);
//发送控制指令
void Chassis_SendCmd(void);

#endif // CHASSIS_TASK_H_
