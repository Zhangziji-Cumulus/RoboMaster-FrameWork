#include "Chassis_Task.h"


//** #################################################################################################### **//
//** ======================================= 声明弱定义函数 ============================================== **//
//** #################################################################################################### **//

__weak void Chassis_Init(void);               //初始化函数
__weak void Chassis_Update(void);             //更新状态函数
__weak void Chassis_HandleError(void);        //异常处理函数
__weak void Chassis_SetMode(void);            //设置模式
__weak void Chassis_RefreshTarget(void);      //更新目标量
__weak void Chassis_CtrlCalc(void);           //计算控制量
__weak void Chassis_SendCmd(void);            //发送控制指令

//** #################################################################################################### **//
//** ========================================= 任务函数 ================================================= **//
//** #################################################################################################### **//

static UBaseType_t remain_ChassisTask;
__attribute__((used)) void ChassisTask(void *argument)
{
  //初始化地盘
  Chassis_Init();
	
  osDelay(CHASSIS_INIT_DELAY);
	
  for(;;)
  {	
    
    //更新状态函数
    Chassis_Update();  
    //异常处理函数
    Chassis_HandleError();  
    //设置模式
    Chassis_SetMode();        
    //更新目标量    
    Chassis_RefreshTarget();     
    //计算控制量 
    Chassis_CtrlCalc();           
    //发送控制指令
    Chassis_SendCmd();            

    //=============================== 剩余栈检测 ===============================//
	  remain_ChassisTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(CHASSIS_TASK_TIME_MS);
  }
}

//** #################################################################################################### **//
//** ======================================== 弱定义任务函数 ============================================= **//
//** #################################################################################################### **//

//初始化函数
__weak void Chassis_Init(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}          

//更新状态函数
__weak void Chassis_Update(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}         

//异常处理函数
__weak void Chassis_HandleError(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//设置模式
__weak void Chassis_SetMode(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//更新目标量
__weak void Chassis_RefreshTarget(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//计算控制量
__weak void Chassis_CtrlCalc(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}

//发送控制指令
__weak void Chassis_SendCmd(void)
{
    /* 
     NOTE : 在其他文件中定义具体内容
    */
}      


