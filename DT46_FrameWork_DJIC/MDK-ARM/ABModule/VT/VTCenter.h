#ifndef VTCENTER_H_
#define VTCENTER_H_

#include "VTCenter_Instance.h"
#include "VT03Protocol.h"
#include "Dual_board_Transmit.h"

#if(BOARD_ID == GIMBAL_BOARD)

const VT03_Data_t* get_VT03_Ctl_point(void);

#endif

#endif
