#include "Gimbal_Task.h"

//** #################################################################################################### **//
//** ======================================= 声明弱定义函数 ============================================== **//
//** #################################################################################################### **//

__weak void Gimbal_Init(void);               //初始化函数
__weak void Gimbal_Update(void);             //更新状态函数
__weak void Gimbal_HandleError(void);        //异常处理函数
__weak void Gimbal_SetMode(void);            //设置模式
__weak void Gimbal_RefreshTarget(void);      //更新目标量
__weak void Gimbal_CtrlCalc(void);           //计算控制量
__weak void Gimbal_SendCmd(void);            //发送控制指令

//** #################################################################################################### **//
//** ========================================= 任务函数 ================================================= **//
//** #################################################################################################### **//

static UBaseType_t remain_GimbalTask;
__attribute__((used)) void GimbalTask(void *argument)
{
  //初始化地盘
  Gimbal_Init();

  for(;;)
  {	
    
    //更新状态函数
    Gimbal_Update();  
    //异常处理函数
    Gimbal_HandleError();  
    //设置模式
    Gimbal_SetMode();        
    //更新目标量    
    Gimbal_RefreshTarget();     
    //计算控制量 
    Gimbal_CtrlCalc();           
    //发送控制指令
    Gimbal_SendCmd();            

    //=============================== 剩余栈检测 ===============================//
	remain_GimbalTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(CHASSIS_TASK_TIME_MS);
  }
}

//** #################################################################################################### **//
//** ======================================== 弱定义任务函数 ============================================= **//
//** #################################################################################################### **//

//初始化函数
__weak void Gimbal_Init(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}          

//更新状态函数
__weak void Gimbal_Update(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}         

//异常处理函数
__weak void Gimbal_HandleError(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//设置模式
__weak void Gimbal_SetMode(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//更新目标量
__weak void Gimbal_RefreshTarget(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//计算控制量
__weak void Gimbal_CtrlCalc(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//发送控制指令
__weak void Gimbal_SendCmd(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}      
