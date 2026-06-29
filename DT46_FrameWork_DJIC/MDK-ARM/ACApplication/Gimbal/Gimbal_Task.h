#ifndef GIMBAL_TASK_H_
#define GIMBAL_TASK_H_

#include "A_MCommon.h"

__attribute__((used)) void GimbalTask(void *argument);

//初始化函数
void Gimbal_Init(void);
//更新状态函数
void Gimbal_Update(void);
//异常处理函数
void Gimbal_HandleError(void);
//设置模式
void Gimbal_SetMode(void);
//更新目标量
void Gimbal_RefreshTarget(void);
//计算控制量
void Gimbal_CtrlCalc(void);
//发送控制指令
void Gimbal_SendCmd(void);


#endif // GIMBAL_TASK_H_