#ifndef __DUAL_BOARD_TRANSMIT_H
#define __DUAL_BOARD_TRANSMIT_H

//** ############################################# **//
//** ================= 引用头文件 ================= **//
//** ############################################# **//

#include "main.h"
#include "can.h"
#include "struct_typedef.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "INS_task.h"
#include "CAN_PART.h"
#include "HOTRC_HT10A.h"



//** ########################################## **//
//** ================= 宏定义 ================= **//
//** ########################################## **//

//** ================= 板子的ID ================= **//

#define BOARD_ID 1//主发送板1（云台板），主接受板2（地盘板）
#define TX_BASE_ID ((BOARD_ID == 1) ? 0x100 : 0x200)  /* 发送基ID */
#define RX_BASE_ID ((BOARD_ID == 1) ? 0x200 : 0x100)  /* 接收基ID */

#define GIMBAL_BOAD		1 //云台板
#define CHASSIS_BOAD	2 //地盘板


//** ############################################## **//
//** ================= 定义结构体 ================= **//
//** ############################################## **//

//** ================= 双板通信数据结构体 ================= **//
typedef struct{
	struct{
		HOTRC_Ctl_t RC_Ctl;
	} B1;
	
	struct{
		float IMU[3];
		HOTRC_Ctl_t RC_Ctl;
	} B2;
	
}Dual_Board_Transmit_t;

//** ################################################ **//
//** ================= 声明对外函数 ================= **//
//** ############################################### **//

void Dual_Board_ReceiveCallBack(void);

#endif
