#ifndef ROBOT_HERO_CONFIG_H_
#define ROBOT_HERO_CONFIG_H_

#include "APP_Config.h"

#if(ROBOT_TYPE == ROBOTTYPE_HERO)

//** ------------------------------------------------------------ **//
//** ================== Hero System Config ====================== **//
//** ------------------------------------------------------------ **//



//** ------------------------------------------------------------ **//
//** ===================== Control PID ========================== **//
//** ------------------------------------------------------------ **//

/*

PID_CTRL_MODE_NORMAL        普通PID（无特殊逻辑，单环基础模式）
环路结构模式
PID_CTRL_MODE_SINGLE_LOOP   单环PID（位置/速度单环）
PID_CTRL_MODE_DOUBLE_LOOP   双环PID（位置环+速度环级联）
PID_CTRL_MODE_TRIPLE_LOOP   三环PID（位置+速度+电流三环串级）
带前馈模式（最常用）
PID_CTRL_MODE_SINGLE_FF     单环PID + 前馈
PID_CTRL_MODE_DOUBLE_FF     双环PID + 前馈
PID_CTRL_MODE_TRIPLE_FF     三环PID + 前馈

*/

/* 云台PID相关 */

//YAW轴PID控制模式
#define PID_CTRL_MODE_YAW           PID_CTRL_MODE_SINGLE_FF //PID_CTRL_MODE_SINGLE_FF
#define PID_CTRL_MODE_PITCH         PID_CTRL_MODE_DOUBLE_LOOP

//多环PID控制精度（允许偏差/阈值）
#define GIMBAL_PID_THRESHOLD  0.1f

/* 地盘PID相关 */

//多环PID控制精度（允许偏差/阈值）
#define CHASSIS_PID_THRESHOLD  1.0f

/* 发射机构相关 */

//多环PID控制精度（允许偏差/阈值）
#define FRICTION_PID_THRESHOLD      1.0f

//多环PID控制精度（允许偏差/阈值）
#define DIAL_PID_THRESHOLD      1.0f


//** ------------------------------------------------------------ **//
//** ====================== Auto Config ========================= **//
//** ------------------------------------------------------------ **//

#define AUTO_AIM                AUTO_AIM_OFF         //自动瞄准配置
#define AUTO_SHOOTING           AUTO_SHOOTING_OFF    //自动射击配置
#define AUTO_NAVIGATION         AUTO_NAVIGATION_OFF  //自动导航配置

//** ------------------------------------------------------------ **//
//** ===================== Gimbal Config ======================== **//
//** ------------------------------------------------------------ **//

/* 系统配置 */
#define GIMBAL_TASK_TIME_MS        1               //云台任务循环时间

/* 云台运动、速度、增量相关 */
#define GIMBAL_MAX_SPEED_YAW_M_S       1 
#define GIMBAL_MAX_SPEED_PITCH_M_S     1

#define GIMBAL_MAX_ANGLE_STEP_DEG_YAW           0.30    //云台目标角度增量的最大值，单位：度
#define GIMBAL_MAX_ANGLE_STEP_DEG_PITCH         0.03     //云台目标角度增量的最大值，单位：度

/* 云台陀螺仪 */

//陀螺仪数组序号
#define GIMBAL_IMU_INDEX_YAW      0
#define GIMBAL_IMU_INDEX_PITCH    2

/* 云台物理参数 */
#define GIMBAL_YAW_RATIO    2       //Yaw轴减速比为 2:1

//云台俯仰角范围
#define GIMBAL_PITCH_MAX_ELE    42.0f   //仰角（抬头）
#define GIMBAL_PITCH_MAX_DEP    17.0f   //俯角（低头）

/* 云台电机控制 */
#define GIMBAL_CAN_CTRL     hcan2   //云台CAN总线

#define GIMBAL_YAW_MOTOR_TYPE    MOTOR_DJI_GM6020         //云台电机类型
#define GIMBAL_PITCH_MOTOR_TYPE  MOTOR_DJI_M3508          //云台电机类型

#define GIMBAL_CAN_GROUP_YAW    DJI_CAN_ID_GROUP_1  //Yaw轴电机所在CAN ID组
#define GIMBAL_CAN_GROUP_PITCH  DJI_CAN_ID_GROUP_2  //Pitch轴电机所在CAN ID组

// 云台电机 CAN ID 定义
#define GIMBAL_CAN_ID_YAW      5    // YAW 轴电机 CAN ID
#define GIMBAL_CAN_ID_PITCH    1    // PITCH 轴电机 CAN ID

// 云台电机反馈ID (CAN ID - 1，大疆电机标准规则)
#define GIMBAL_MOTOR_ID_FBK_YAW      (GIMBAL_CAN_ID_YAW   - 1)
#define GIMBAL_MOTOR_ID_FBK_PITCH    (GIMBAL_CAN_ID_PITCH - 1)


//** ------------------------------------------------------------ **//
//** ==================== Shooting Config ======================= **//
//** ------------------------------------------------------------ **//

/* 系统配置 */
#define SHOOTING_TASK_TIME_MS   1                  //射击任务循环时间

/** ===== 摩擦轮配置 ===== **/

/* 摩擦轮速度相关 */
#define FRICTION_MAX_SPEED_M_S  5.0f               //摩擦轮最大线速度(单位：m/s）

/* 摩擦轮的物理参数 */
#define FRICTION_NUM            3                   //摩擦轮数量

#define FRICTION_RADIUS_MM      30.0f                  //摩擦轮半径（单位：mm）
#define FRICTION_RATIO          1                   //摩擦轮减速比

/* 摩擦轮电机控制 */
#define FRICTION_CAN_CTRL       hcan1               //摩擦轮CAN总线

#define FRICTION_CAN_GROUP      DJI_CAN_ID_GROUP_1

#define FRICTION_MOTOR_TYPE     MOTOR_DJI_M3508     //摩擦轮电机类型

//电机实际CAN ID
#define FRICTION_MOTOR_ID_CAN_UL    1                   //上左摩擦轮电机ID
#define FRICTION_MOTOR_ID_CAN_UR    2                   //上右摩擦轮电机ID
#define FRICTION_MOTOR_ID_CAN_DM    3                   //下中摩擦轮电机ID
//电机反馈数据  ID
#define FRICTION_MOTOR_ID_FBK_UL    FRICTION_MOTOR_ID_CAN_UL - 1 
#define FRICTION_MOTOR_ID_FBK_UR    FRICTION_MOTOR_ID_CAN_UR - 1
#define FRICTION_MOTOR_ID_FBK_DM    FRICTION_MOTOR_ID_CAN_DM - 1

/** ===== 拨盘配置 ===== **/

/* 拨盘速度相关 */
#define DIAL_MAX_SPEED_M_S      0.15f                   //拨盘最大线速度

/* 拨盘物理参数 */

#define DIAL_RADIUS_MM          60.0f               //拨盘半径
#define DIAL_RATIO              DJI_M3508_RATIO     //拨盘减速比


/* 拨盘电机控制 */
#define DIAL_CAN_CTRL           hcan1               //拨盘CAN总线

#define DIAL_MOTOR_TYPE         MOTOR_DJI_M3508     //拨盘电机类型

#define DIAL_CAN_ID             5                   //拨盘实际CAN ID

#define DIAL_MOTOR_ID_FBK       DIAL_CAN_ID - 1    //拨盘电机 反馈 ID （CAN ID - 1）

/* ==== 推杆相关配置 ==== */


/* 距离(mm) 转 丝杆旋转总角度(°) */
#define PUSHROD_DIST_TO_ANGLE(dist, lead)    ((dist) * 360.0f / (lead))

/* 丝杆物理参数 */

#define PUSHROD_SCREW_LEAD_MM       5.0f        //丝杆导程

/* 推杆速度相关 */

#define PUSHROD_ACC            10000         //RPM/s
#define PUSHROD_DEC            9000         //RPM/s

#define PUSHROD_CW             0          //方向，顺时针转
#define PUSHROD_CCW            9          //方向，逆时针转

#define PUSHROD_MAX_SPEED_RPM  5000       //要计算成转速rpm/s(最大3000转)

/* 推杆位置、行程 */

/* 位置运动模式定义 (raf 参数) */
#define PUSHROD_POS_MODE_RELATIVE_LAST_TARGET      0  // 相对上一次输入的目标位置
#define PUSHROD_POS_MODE_ABSOLUTE                  1  // 绝对位置（以零点为基准）
#define PUSHROD_POS_MODE_RELATIVE_CURRENT          2  // 相对当前实时位置

#define PUSHROD_POSITION_MAX_TRAVEL_MM      105.0f       //推杆最大行程,单位：mm

#define PUSHROD_POSTION_FRONT_MM            5.0f     //推杆前位置
#define PUSHROD_POSTION_BACK_MM             110.0f    //推杆后位置

#define PUSHROD_POSITION_L_MM            110.0f     //上弹位置
#define PUSHROD_POSITION_A_MM            95.0f     //预发弹位置
#define PUSHROD_POSITION_B_MM            50.0f      //发射第一颗弹位置
#define PUSHROD_POSITION_C_MM            5.0f       //发射第二颗弹位置

//推杆后位置
#define PUSHROD_POSTION_FRONT_DEG      PUSHROD_DIST_TO_ANGLE(PUSHROD_POSTION_FRONT_MM,PUSHROD_SCREW_LEAD_MM) 
//推杆前位置PUSHROD_DIST_TO_ANGLE     
#define PUSHROD_POSTION_BACK_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSTION_BACK_MM,PUSHROD_SCREW_LEAD_MM)       

//点L位置
#define PUSHROD_POSITION_L_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_L_MM,PUSHROD_SCREW_LEAD_MM) 
//点A位置
#define PUSHROD_POSITION_A_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_A_MM,PUSHROD_SCREW_LEAD_MM) 
//点B位置
#define PUSHROD_POSITION_B_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_B_MM,PUSHROD_SCREW_LEAD_MM) 
//点C位置
#define PUSHROD_POSITION_C_DEG       PUSHROD_DIST_TO_ANGLE(PUSHROD_POSITION_C_MM,PUSHROD_SCREW_LEAD_MM)   

/* 电流检测相关 */

#define PUSHROD_CURRENT_MAX       2000                  //阈值最大电流(mA)

/* 推杆电机控制 */

#define PUSHROD_CAN_CTRL           hcan2                //推杆CAN总线

#define PUSHROD_MOTOR_TYPE         MOTOR_ZTD_42STEP     //推杆电机类型

#define PUSHROD_CAN_ID             1                    //推杆实际CAN ID / Address

//#define PUSHROD_MOTOR_ID_FBK       PUSHROD_CAN_ID - 1      //推杆电机 反馈 ID （CAN ID - 1）

/* ==== 自动上弹 ==== */
#define AUTOLOAD_THRESH_CUR_DIAL 7500 //自动上弹拨盘电流阈值

#define AUTOLOAD_THRESH_CUR_FRICTION 1000 //自动上弹发射电流阈值

//** ------------------------------------------------------------ **//
//** ==================== Chassis Config ======================== **//
//** ------------------------------------------------------------ **//

/* 系统配置 */
#define CHASSIS_TASK_TIME_MS        1               //地盘任务循环时间

#define CHASSIS_INIT_DELAY          100             //地盘任务初始化延迟

/* 地盘类型 */
#define CHASSIS_TYPE            CHASSIS_MECANUM            //底盘类型

/* 地盘速度相关 */
#define CHASSIS_MAX_SPEED            1.0f           //底盘最大移动速度（单位：m/s）
#define CHASSIS_MAX_SPEED_FOLLOWING  1.5f           //底盘最大跟随速度（单位：m/s）
#define CHASSIS_MAX_SPIN_SPEED       1.5f           //底盘最大旋转速度（单位：rad/s）

/* 地盘的物理参数 */
#define CHASSIS_WHEEL_RADIUS_MM     76.0f           //轮子半径（单位：mm）
#define CHASSIS_WHEEL_WB_MM         0.0f            //地盘轴距（单位：mm），前后两轮中心直线距离
#define CHASSIS_WHEEL_TW_MM         0.0f            //地盘轮距（单位：mm），左右车轮中心距离

/* 地盘电机控制 */
#define CHASSIS_CAN_CTRL        hcan1               //底盘CAN控制总线 
#define CHASSIS_CAN_GROUP       DJI_CAN_ID_GROUP_1  //CAN总线控制组，组1

#define CHASSIS_CAN_YAW         hcan2               //获取地盘云台相关角度电机所在总线

#define CHASSIS_MOTOR_TYPE      MOTOR_DJI_M3508     //底盘电机类型

// 底盘电机 实际 CAN ID
#define CHASSIS_CAN_ID_FL      4    // 前左底盘电机 CAN ID
#define CHASSIS_CAN_ID_FR      3    // 前右底盘电机 CAN ID
#define CHASSIS_CAN_ID_BL      1    // 后左底盘电机 CAN ID
#define CHASSIS_CAN_ID_BR      2    // 后右底盘电机 CAN ID

// 底盘电机 反馈 ID (CAN ID - 1)
#define CHASSIS_MOTOR_ID_FBK_FL    (CHASSIS_CAN_ID_FL - 1)
#define CHASSIS_MOTOR_ID_FBK_FR    (CHASSIS_CAN_ID_FR - 1)
#define CHASSIS_MOTOR_ID_FBK_BL    (CHASSIS_CAN_ID_BL - 1)
#define CHASSIS_MOTOR_ID_FBK_BR    (CHASSIS_CAN_ID_BR - 1)

/* 地盘云台角 */
#define YAW_ZERO_ANGLE 271.5f   //云台朝前零点角度重置

#endif

#endif // ROBOT_HERO_CONFIG_H_
