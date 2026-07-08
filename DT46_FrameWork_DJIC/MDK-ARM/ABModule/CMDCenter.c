#include "CMDCenter.h"
#include "sound_effects_task.h"

static CMD_t CMD;

static KeyHandle_t Key_RCTrigger;

static KeyHandle_t Key_SpinCW;
static KeyHandle_t Key_SpinCCW;

static KeyHandle_t Key_AutoAim;
static KeyHandle_t Key_Fire;
static KeyHandle_t Key_RefreshUI;

#if(BOARD_ID == GIMBAL_BOARD)

//** ================================================================================ **//
//** ================================== 对外函数 ==================================== **//
//** ================================================================================ **//

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

//初始化
void CMD_Center_Init(void)
{
    KeyHandle_Init(&Key_RCTrigger,5,1);

    KeyHandle_Init(&Key_SpinCW,5,1);
    KeyHandle_Init(&Key_SpinCCW,5,1);

    KeyHandle_Init(&Key_AutoAim,5,1);
    KeyHandle_Init(&Key_Fire,5,1);
    KeyHandle_Init(&Key_RefreshUI,5,1);
}

static UBaseType_t remain_CMDUpdateTask;
__attribute__((used)) void CMDUpdateTask(void *argument)
{

  CMD_Center_Init();
  buzzer_t *buzzer = get_buzzer_effect_point();

  for(;;)
  {
    const RC_Ctl_t* RC_Ctl = get_RC_Ctl_point();
    const VT03_Data_t* VT_Ctr = get_VT03_Ctl_point();

    //判断有没有正确接受到数据、数据是否正常；不正常则设置为“停止模式”
    if(VT_Ctr->is_valid == 1 || RC_Ctl->is_valid == 1)
    {
      //更新控制模式
      if (VT_Ctr->is_valid == 1)
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
          CMD.Move = NORMAL;
        }
        else if(RC_Ctl->Switch.S3_R == HOTRC_SWITCH_UP)
        {
          if(RC_Ctl->Knob.L_state == HOTRC_KNOB_R)
          {
              CMD.Move = SPIN_CW;
          }
          else
          {
              CMD.Move = SPIN_CCW;
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
      //图传链路控制
      else if(CMD.ctrl == KEYBOARD_MODE)
      {

        //键盘控制
        {
          //地盘移动
          {
            //前后
            if(VT_Ctr->keyboard.bit.w)
            {
              CMD.Chassis.FB = CMD_CTRL_RANGE;
            }
            else if(VT_Ctr->keyboard.bit.s)
            {
              CMD.Chassis.FB = -CMD_CTRL_RANGE;
            }
            else
            {
              CMD.Chassis.FB = 0;
            }

            //左右
            if(VT_Ctr->keyboard.bit.a)
            {
              CMD.Chassis.LR = CMD_CTRL_RANGE;
            }
            else if(VT_Ctr->keyboard.bit.d)
            {
              CMD.Chassis.LR = -CMD_CTRL_RANGE;
            }
            else
            {
              CMD.Chassis.LR = 0;
            }
          }

          //云台移动，鼠标 x，y
          {
            //鼠标传入的数据太小了放大一点
            int16_t mouse_x = MyMath_Scale_Int16(VT_Ctr->mouse_x,1000.0,VTX_MOUSE_MIN,VTX_MOUSE_MAX);
            int16_t mouse_y = MyMath_Scale_Int16(VT_Ctr->mouse_y,1000.0,VTX_MOUSE_MIN,VTX_MOUSE_MAX);

            CMD.Gimbal.Yaw = MyMath_Map_Range_Int16(mouse_x,
                                                    VTX_MOUSE_MIN,
                                                    VTX_MOUSE_MAX,
                                                   -CMD_CTRL_RANGE,
                                                    CMD_CTRL_RANGE);

            CMD.Gimbal.Pitch = MyMath_Map_Range_Int16(mouse_y,
                                                      VTX_MOUSE_MIN,
                                                      VTX_MOUSE_MAX,
                                                     -CMD_CTRL_RANGE,
                                                      CMD_CTRL_RANGE);
          }


          //小陀螺切换，Q逆时钟，E顺时钟
          {
              // 记录处理前状态，用于判断本次哪个按键新触发
              uint8_t old_cw = Key_SpinCW.toggle_state;
              uint8_t old_ccw = Key_SpinCCW.toggle_state;

              // 执行按键消抖翻转处理

              uint8_t cw_sw  = KeyToggle_Process(&Key_SpinCW, VT_Ctr->keyboard.bit.q);
              uint8_t ccw_sw = KeyToggle_Process(&Key_SpinCCW, VT_Ctr->keyboard.bit.e);
              

              // 两路开关同时打开处理
              if (cw_sw && ccw_sw)
              {
                  // 情况1：仅单个按键是本次新按下，切换旋向
                  if (!old_cw && cw_sw)
                  {
                      // 刚按Q，彻底重置CCW按键所有状态
                      Key_SpinCCW.toggle_state = 0;
                      Key_SpinCCW.last_stable_lv = 0;
                      Key_SpinCCW.debounce_cnt = 0;
                      ccw_sw = 0;
                  }
                  else if (!old_ccw && ccw_sw)
                  {
                      // 刚按E，彻底重置CW按键所有状态
                      Key_SpinCW.toggle_state = 0;
                      Key_SpinCW.last_stable_lv = 0;
                      Key_SpinCW.debounce_cnt = 0;
                      cw_sw = 0;
                  }
                  else
                  {
                      // 双手同步同时按下，强制CW，完整清空CCW
                      Key_SpinCCW.toggle_state = 0;
                      Key_SpinCCW.last_stable_lv = 0;
                      Key_SpinCCW.debounce_cnt = 0;
                      ccw_sw = 0;
                      // 同步更新结构体，保证下一帧稳定为1
                      Key_SpinCW.toggle_state = 1;
                      cw_sw = 1;
                  }
              }

              // 输出运动指令
              if (cw_sw)
              {
                  CMD.Move = SPIN_CW;
              }
              else if (ccw_sw)
              {
                  CMD.Move = SPIN_CCW;
              }
              else
              {
                  CMD.Move = NORMAL;
              }
          }

          //开火
          CMD.Shooting.Fire = KeyDebounce_Process(&Key_Fire,VT_Ctr->mouse_left);
          //自瞄开启
          if(KeyLongPress_Process(&Key_AutoAim,VT_Ctr->mouse_right,1,200) == KEY_LONG_PRESS_HOLD)
          {
              CMD.Auto.Aim = ON;
          }
          else
          {
              CMD.Auto.Aim = OFF;
          }
          //刷新UI
          CMD.other.RefreshUI = KeyDebounce_Process(&Key_RefreshUI,VT_Ctr->keyboard.bit.x);
        }

        //RC遥控器控制
        if(!VT_Ctr->keyboard.value
            && VT_Ctr->mouse_x != 0
            && VT_Ctr->mouse_y != 0
            && VT_Ctr->mouse_z != 0
            && VT_Ctr->custom_left != 0
            && VT_Ctr->custom_right != 0
            && VT_Ctr->mouse_mid != 0
        )
        {
          //地盘移动
          {
            CMD.Chassis.FB = MyMath_Map_Range_Int16((VT_Ctr->ch2 - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MIN - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MAX - VTX_CHANNEL_MID),
                                                    -CMD_CTRL_RANGE,
                                                     CMD_CTRL_RANGE);

            CMD.Chassis.LR = MyMath_Map_Range_Int16((VT_Ctr->ch3 - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MIN - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MAX - VTX_CHANNEL_MID),
                                                    -CMD_CTRL_RANGE,
                                                     CMD_CTRL_RANGE);

            CMD.Chassis.RO = MyMath_Map_Range_Int16((VT_Ctr->ch0 - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MIN - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MAX - VTX_CHANNEL_MID),
                                                    -CMD_CTRL_RANGE,
                                                     CMD_CTRL_RANGE);
          }

          //云台移动
          {

            CMD.Gimbal.Yaw = MyMath_Map_Range_Int16((VT_Ctr->ch0 - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MIN - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MAX - VTX_CHANNEL_MID),
                                                    -CMD_CTRL_RANGE,
                                                     CMD_CTRL_RANGE);

            CMD.Gimbal.Pitch = MyMath_Map_Range_Int16((VT_Ctr->ch1 - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MIN - VTX_CHANNEL_MID),
                                                    (VTX_CHANNEL_MAX - VTX_CHANNEL_MID),
                                                    -CMD_CTRL_RANGE,
                                                     CMD_CTRL_RANGE);
          }

          //发射机构
          CMD.Shooting.Fire = KeyDebounce_Process(&Key_RCTrigger,VT_Ctr->trigger);
        }

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
