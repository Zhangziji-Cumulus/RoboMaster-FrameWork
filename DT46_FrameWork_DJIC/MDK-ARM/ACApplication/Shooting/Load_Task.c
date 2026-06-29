#include "Load_Task.h"

//** #################################################################################################### **//
//** ======================================= 声明弱定义函数 ============================================== **//
//** #################################################################################################### **//

__weak void Load_Init(void);               //初始化函数
__weak void Load_Update(void);             //更新状态函数
__weak void Load_HandleError(void);        //异常处理函数
__weak void Load_SetMode(void);            //设置模式
__weak void Load_RefreshTarget(void);      //更新目标量
__weak void Load_CtrlCalc(void);           //计算控制量
__weak void Load_SendCmd(void);            //发送控制指令

//** #################################################################################################### **//
//** ========================================= 任务函数 ================================================= **//
//** #################################################################################################### **//

static UBaseType_t remain_LoadTask;
__attribute__((used)) void LoadTask(void *argument)
{
  //初始化地盘
  Shooting_Init();

  for(;;)
  {	
    
    //更新状态函数
    Load_Update();  
    //异常处理函数
    Load_HandleError();  
    //设置模式
    Load_SetMode();        
    //更新目标量    
    Load_RefreshTarget();     
    //计算控制量 
    Load_CtrlCalc();           
    //发送控制指令
    Load_SendCmd();            

    //=============================== 剩余栈检测 ===============================//
	remain_LoadTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(SHOOTING_TASK_TIME_MS);
  }
}

//** #################################################################################################### **//
//** ======================================== 弱定义任务函数 ============================================= **//
//** #################################################################################################### **//

//初始化函数
__weak void Load_Init(void)
{

}          

//更新状态函数
__weak void Load_Update(void)
{

}         

//异常处理函数
__weak void Load_HandleError(void)
{

}

//设置模式
__weak void Load_SetMode(void)
{
 
}

//更新目标量
__weak void Load_RefreshTarget(void)
{

}

//计算控制量
__weak void Load_CtrlCalc(void)
{

}

//发送控制指令
__weak void Load_SendCmd(void)
{

}
