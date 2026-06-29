#include "Shooting_Task.h"

//** #################################################################################################### **//
//** ======================================= 声明弱定义函数 ============================================== **//
//** #################################################################################################### **//

__weak void Shooting_Init(void);               //初始化函数
__weak void Shooting_Update(void);             //更新状态函数
__weak void Shooting_HandleError(void);        //异常处理函数
__weak void Shooting_SetMode(void);            //设置模式
__weak void Shooting_RefreshTarget(void);      //更新目标量
__weak void Shooting_CtrlCalc(void);           //计算控制量
__weak void Shooting_SendCmd(void);            //发送控制指令

//** #################################################################################################### **//
//** ========================================= 任务函数 ================================================= **//
//** #################################################################################################### **//

static UBaseType_t remain_ShootingTask;
__attribute__((used)) void ShootingTask(void *argument)
{
  //初始化地盘
  Shooting_Init();

  for(;;)
  {	
    
    //更新状态函数
    Shooting_Update();  
    //异常处理函数
    Shooting_HandleError();  
    //设置模式
    Shooting_SetMode();        
    //更新目标量    
    Shooting_RefreshTarget();     
    //计算控制量 
    Shooting_CtrlCalc();           
    //发送控制指令
    Shooting_SendCmd();            

    //=============================== 剩余栈检测 ===============================//
	remain_ShootingTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(SHOOTING_TASK_TIME_MS);
  }
}

//** #################################################################################################### **//
//** ======================================== 弱定义任务函数 ============================================= **//
//** #################################################################################################### **//

//初始化函数
__weak void Shooting_Init(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}          

//更新状态函数
__weak void Shooting_Update(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}         

//异常处理函数
__weak void Shooting_HandleError(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//设置模式
__weak void Shooting_SetMode(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//更新目标量
__weak void Shooting_RefreshTarget(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//计算控制量
__weak void Shooting_CtrlCalc(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//发送控制指令
__weak void Shooting_SendCmd(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}
