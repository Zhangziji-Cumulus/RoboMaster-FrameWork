#include "Remote_Ctrl.h"

//拨杆状态设置函数,根据输入的通道值和预设的最小值、中值、最大值，判断拨杆处于哪个位置，并返回对应的状态码（1、2或3）


/**
 * @brief 根据通道值和预设值判断拨杆状态
 * 
 * @param ChValue 实时通道值
 * @param min_Val 接收到的最小值
 * @param mid_Val 接收到的中间值
 * @param max_Val 接收到的最大值
 * @return uint8_t 返回1、2、3分别表示拨杆处于最小、中值、最大位置，返回0表示未识别、错误输入
 */
static uint8_t Switch_Set(uint16_t ChValue,uint16_t min_Val,uint16_t mid_Val,uint16_t max_Val)
{
		if(ChValue >= min_Val && ChValue <= max_Val)
		{
			if(ChValue > (min_Val - 20) && ChValue < (min_Val + 20))
			{
				return 1;
			}
			else if(ChValue > (mid_Val - 20) && ChValue < (mid_Val + 20))
			{
				return 2;
			}
			else if(ChValue > (max_Val - 20) && ChValue < (max_Val + 20))
			{
				return 3;
			}
		}
		else
		{
			return 0; //未识别、错误输入
		}
}

static uint8_t Knob_Set(uint16_t ChValue,uint16_t min_Val,uint16_t mid_Val,uint16_t max_Val)
{
		if(ChValue >= min_Val && ChValue <= max_Val)
		{
			if(ChValue > (min_Val - 200) && ChValue < (mid_Val - 200))
			{
				return 1;
			}
			else if(ChValue > (mid_Val - 200) && ChValue < (mid_Val + 200))
			{
				return 2;
			}
			else if(ChValue > (mid_Val + 200) && ChValue < (max_Val + 200))
			{
				return 3;
			}
		}
		else
		{
			return 0; //未识别、错误输入
		}
}

//** #################################################################################################### **//
//** ====================================== HOTRC遥控器 ================================================= **//
//** #################################################################################################### **//

#if (REMOTE_CTRL_TYPE == REMOTE_HOTRC)

static RC_Ctl_t RC_Ctl;

//解析对应遥控器数据
static void HORRC_HT10A_GET_Ctl(void)
{
    const SbusData_t *sbusData = get_SBUS_Data_point();

		RC_Ctl.Stick.LX = sbusData->channels[3] - HOTRC_MID_VEL;
		RC_Ctl.Stick.LY = sbusData->channels[2] - HOTRC_MID_VEL;
		RC_Ctl.Stick.RX = sbusData->channels[0] - HOTRC_MID_VEL;
		RC_Ctl.Stick.RY = sbusData->channels[1] - HOTRC_MID_VEL;
	
		RC_Ctl.Switch.S2_L = Switch_Set(sbusData->channels[5],HOTRC_MIN_VEL,HOTRC_MID_VEL,HOTRC_MAX_VEL);
		RC_Ctl.Switch.S2_R = Switch_Set(sbusData->channels[6],HOTRC_MIN_VEL,HOTRC_MID_VEL,HOTRC_MAX_VEL);
		RC_Ctl.Switch.S3_L = Switch_Set(sbusData->channels[4],HOTRC_MIN_VEL,HOTRC_MID_VEL,HOTRC_MAX_VEL);
		RC_Ctl.Switch.S3_R = Switch_Set(sbusData->channels[7],HOTRC_MIN_VEL,HOTRC_MID_VEL,HOTRC_MAX_VEL);
		
		//返回旋钮线性值
		RC_Ctl.Knob.L_linear = sbusData->channels[8];
	  	RC_Ctl.Knob.R_linear = sbusData->channels[9];
		//返回旋钮离散值
		RC_Ctl.Knob.L_state = Knob_Set(sbusData->channels[8],HOTRC_MIN_VEL,HOTRC_MID_VEL,HOTRC_MAX_VEL);
		RC_Ctl.Knob.R_state = Knob_Set(sbusData->channels[9],HOTRC_MIN_VEL,HOTRC_MID_VEL,HOTRC_MAX_VEL);

		RC_Ctl.Flag.ch17 = sbusData->ch17;
		RC_Ctl.Flag.ch18 = sbusData->ch18;
		RC_Ctl.Flag.failsafe = sbusData->failsafe;
		RC_Ctl.Flag.frameLost = sbusData->frameLost;
		RC_Ctl.is_valid = sbusData->is_valid;

		RC_Ctl.Range.Max =  (HOTRC_MAX_VEL - HOTRC_MID_VEL);
		RC_Ctl.Range.Min = -(HOTRC_MAX_VEL - HOTRC_MID_VEL);
}

/** 获取遥控器数据指针（只读） */
const RC_Ctl_t* get_RC_Ctl_point(void)
{
    return &RC_Ctl;
}

/* 获取遥控器数据 */
RC_Ctl_t get_RC_Ctl(void)
{
    return RC_Ctl;
}

//** ================================================================================ **//
//** ============================== 更新控制量任务 =================================== **//
//** ================================================================================ **//
static UBaseType_t remain_RCUpdate;
__attribute__((used)) void RCUpdateTask(void *argument)
{

  for(;;)
  {

		HORRC_HT10A_GET_Ctl();
		//=============================检测剩余栈=================================//
		remain_RCUpdate = uxTaskGetStackHighWaterMark(NULL);
    osDelay(1);
  }
}

#endif

//** #################################################################################################### **//
//** ==================================== DJI DT7遥控器 ================================================= **//
//** #################################################################################################### **//


#if (REMOTE_CTRL_TYPE == REMOTE_DJI_DT7)
/* 暂时没写 */
#endif

