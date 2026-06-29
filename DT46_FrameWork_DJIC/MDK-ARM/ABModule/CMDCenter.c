#include "CMDCenter.h"

static CMD_t CMD;

//** ================================================================================ **//
//** ================================== 对外函数 ==================================== **//
//** ================================================================================ **//

void CMD_Center_Init(void)
{
    
}

#if(BOARD_ID == GIMBAL_BOARD)

// BoardTransmit_Gimbal_TX_t BoardGRX;

//** ------------------------------------------------------------ **//
//** ===================== 获取数据（指针） ====================== **//
//** ------------------------------------------------------------ **//
const CMD_t* CMD_Get_point(void)
{
    return &CMD;
}

//** ================================================================================ **//
//** ============================== 更新控制变量任务 ================================= **//
//** ================================================================================ **//
static UBaseType_t remain_CMDUpdateTask;
__attribute__((used)) void CMDUpdateTask(void *argument)
{

  for(;;)
  {
    const RC_Ctl_t* RC_Ctl = get_RC_Ctl_point();
    const VideoTx_Ctrl_t *VideoTx_Data = get_VideoTx_Ctl_point();

    //判断有没有正确接受到数据、数据是否正常；不正常则设置为“停止模式”
    if(VideoTx_Data->is_valid == 1 || RC_Ctl->is_valid == 1)
    {
      //更新控制模式
      if (VideoTx_Data->is_valid == 1)
      {
        CMD.ctrl = KEYBOARD_MODE;
      }
      else if (RC_Ctl->is_valid == 1)
      {
        CMD.ctrl = REMOTE_MODE;
      }

      //遥控器
      if(CMD.ctrl == REMOTE_MODE)
      {
        //移动模式
        if(RC_Ctl->Switch.S3_R == HOTRC_SWITCH_DOWN)
        {
          CMD.ctrl = STOP_MODE;
        }

        if(RC_Ctl->Switch.S3_R == HOTRC_SWITCH_MID)
        {
          CMD.Move = Normal;
        }
        else if(RC_Ctl->Switch.S3_R == HOTRC_SWITCH_UP)
        {
          if(RC_Ctl->Knob.L_state == HOTRC_KNOB_R)
          {
              CMD.Move = Spin_CW;
          }
          else
          {
              CMD.Move = Spin_CCW;
          }
        }

        //底盘移动指令
        CMD.Chassis.FB   = MyMath_Map_Range_Int16(RC_Ctl->Stick.LY,RC_Ctl->Range.Min,RC_Ctl->Range.Max,-CMD_CTRL_RANGE,CMD_CTRL_RANGE);
        CMD.Chassis.LR   = MyMath_Map_Range_Int16(RC_Ctl->Stick.LX,RC_Ctl->Range.Min,RC_Ctl->Range.Max,-CMD_CTRL_RANGE,CMD_CTRL_RANGE);
        CMD.Chassis.RO   = MyMath_Map_Range_Int16(RC_Ctl->Stick.RX,RC_Ctl->Range.Min,RC_Ctl->Range.Max,-CMD_CTRL_RANGE,CMD_CTRL_RANGE);
        //云台移动指令
        CMD.Gimbal.Yaw   = MyMath_Map_Range_Int16(RC_Ctl->Stick.RX,RC_Ctl->Range.Min,RC_Ctl->Range.Max,-CMD_CTRL_RANGE,CMD_CTRL_RANGE);
        CMD.Gimbal.Pitch = MyMath_Map_Range_Int16(RC_Ctl->Stick.RY,RC_Ctl->Range.Min,RC_Ctl->Range.Max,-CMD_CTRL_RANGE,CMD_CTRL_RANGE);

        //发射机构指令
        
        //发射
        if(RC_Ctl->Switch.S3_L == HOTRC_SWITCH_UP)
        {
          CMD.Shooting.Fire = ON;
        }
        else
        {
          CMD.Shooting.Fire = OFF;
        }
        //上弹
        if(RC_Ctl->Switch.S3_L == HOTRC_SWITCH_DOWN)
        {
          CMD.Shooting.Load = ON;
        }
        else
        {
          CMD.Shooting.Load = OFF;
        }
        //打开摩擦轮
        if(RC_Ctl->Switch.S2_L == HOTRC_SWITCH_DOWN)
        {
          CMD.Shooting.Friction = ON;
        }
        else
        {
          CMD.Shooting.Friction = OFF;
        }

        //自动控制相关
        if(RC_Ctl->Switch.S2_R == HOTRC_SWITCH_DOWN)
        {
          CMD.Auto.Aim = ON;
        }
        else
        {
          CMD.Auto.Aim = OFF;

        }

      }
      //键盘控制
      else if(CMD.ctrl == KEYBOARD_MODE)
      {
        CMD.Chassis.FB = VideoTx_Data->ch2  - VTX_CHANNEL_MID;
        CMD.Chassis.LR = VideoTx_Data->ch3  - VTX_CHANNEL_MID;
        CMD.Chassis.RO = VideoTx_Data->ch0  - VTX_CHANNEL_MID;

        CMD.Gimbal.Yaw = VideoTx_Data->ch0    -  VTX_CHANNEL_MID; 
        CMD.Gimbal.Pitch = VideoTx_Data->ch1  -  VTX_CHANNEL_MID;
      }
    }
    else
    {
      CMD.ctrl = STOP_MODE;
    }

    //=============================检测剩余栈=================================//
	  remain_CMDUpdateTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

#endif

#if(BOARD_ID == CHASSIS_BOARD)

BoardTransmit_Chassis_RX_t  BoardCRX;

//** ------------------------------------------------------------ **//
//** ===================== 获取数据（指针） ====================== **//
//** ------------------------------------------------------------ **//
const CMD_t* CMD_Get_point(void)
{
    return &CMD;
}

//** ================================================================================ **//
//** ============================== 更新控制变量任务 ================================= **//
//** ================================================================================ **//
static UBaseType_t remain_CMDUpdateTask;
__attribute__((used)) void CMDUpdateTask(void *argument)
{
  osDelay(100);
  for(;;)
  {
    CMD = BoardCRX.CMD;
    //=============================检测剩余栈=================================//
	  remain_CMDUpdateTask = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

#endif
