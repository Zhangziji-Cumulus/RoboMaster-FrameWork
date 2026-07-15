#ifndef __REMOTE_CTRL_H__
#define __REMOTE_CTRL_H__

/* system includes */
#include "main.h"
/* user includes */
#include "bsp_SBUS.h"

//** #################################################################################################### **//
//** ========================================= 宏定义 =================================================== **//
//** #################################################################################################### **//

//选择遥控器类型
#define REMOTE_HOTRC            0
#define REMOTE_DJI_DT7          1

#define REMOTE_CTRL_TYPE        REMOTE_HOTRC

//** #################################################################################################### **//
//** ====================================== HOTRC遥控器 ================================================= **//
//** #################################################################################################### **//

#if (REMOTE_CTRL_TYPE == REMOTE_HOTRC)

#define HOTRC_MIN_VEL 192	//接受到的数据的最小值
#define HOTRC_MID_VEL 992   //接受到的数据的正中值
#define HOTRC_MAX_VEL 1792	//接受到的数据的最大值

//#define HOTRC_RANGE 600 //接受到的数据的范围

// ==================== 摇杆数据滤波配置 ====================
#define RC_STICK_DEADZONE      5     // 摇杆死区阈值（原始通道值与中值的差小于此值视为回中）
#define RC_FILTER_THRESHOLD    5     // 滤波判定阈值（计数超过此值才输出有效值）
#define RC_FILTER_CNT_INIT     6     // 滤波计数器初始值（默认 > RC_FILTER_THRESHOLD）

//拨杆的位置
#define HOTRC_SWITCH_UP    1
#define HOTRC_SWITCH_MID   2
#define HOTRC_SWITCH_DOWN  3
//旋钮的位置
#define HOTRC_KNOB_L    1		//旋钮左
#define HOTRC_KNOB_M    2		//旋钮中
#define HOTRC_KNOB_R    3		//旋钮右

typedef struct{
	
	//摇杆
	struct{
		short LX;// 左摇杆 X 轴
		short LY;// 左摇杆 Y 轴
		short RX;// 右摇杆 X 轴
		short RY;// 右摇杆 Y 轴
	}Stick;
	
	//拨杆值为1、2、3
	struct{
		short S2_L;// 左两段拨杆
		short S2_R;// 右两段拨杆
		short S3_L;// 左三段拨杆
		short S3_R;// 右三段拨杆
	}Switch;
	
	//旋钮
	struct{
		short 		L_linear;	//左边旋钮，线性值
		uint8_t 	L_state;	//左边旋钮，离散值

		short 		R_linear;	//右边旋钮，线性值
		uint8_t 	R_state;	//右边旋钮，离散值
	}Knob;

	//标志位
	struct{
		uint8_t ch17;                        // 数字通道1 (0或1)
		uint8_t ch18;                        // 数字通道2 (0或1)
		uint8_t frameLost;                   // 丢帧标志 (1表示丢帧)
		uint8_t failsafe;                    // 故障安全标志 (1表示触发)
	}Flag;

	//数据是否有效
	uint8_t is_valid;					  // 新且正确的数据有效（1为有效，0为无效）

	// 摇杆数据范围
	struct{
		int16_t Max;
		int16_t Min;					  
	}Range;
	
}RC_Ctl_t;

/** 获取遥控器数据指针（只读） */
const RC_Ctl_t* get_RC_Ctl_point(void);
/* 获取遥控器数据 */
RC_Ctl_t get_RC_Ctl(void);

#endif

//** #################################################################################################### **//
//** ====================================== DJI DT7遥控器 =============================================== **//
//** #################################################################################################### **//

#if (REMOTE_CTRL_TYPE == REMOTE_DJI_DT7)



/* 暂时没写 */



#endif

#endif /* __REMOTE_CTRL_H__ */
