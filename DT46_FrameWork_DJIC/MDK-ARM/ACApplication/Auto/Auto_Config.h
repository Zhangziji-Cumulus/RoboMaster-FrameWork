#ifndef AUTO_CONFIG_H_
#define AUTO_CONFIG_H_

/* 系统相关 */

#define AUTO_TASK_TIME_MS       5       //自动任务系统循环时间 

#define AUTO_MATCH_RESET_MS     500      //自动任务系统匹配重置时间
//串口句柄
#define AUTO_USART_HANDLE       huart1    //串口使用到的句柄

#define AUTO_USART_HEADER       0x5A      //串口数据包头
//#define AUTO_USART_TAIL         0x6B      //串口数据包尾



/* 自瞄算法相关 */

//敌人颜色( 不知道能否自动获取值来更改 )
#define AUTOAIM_ENEMY_COLOR_RED       0     //红色
#define AUTOAIM_ENEMY_COLOR_BLUE      1     //蓝色

#define AUTOAIM_ENEMY_COLOR AUTOAIM_ENEMY_COLOR_BLUE 

//开关自瞄宏
#define AUTOAIM_OFF     0       //关闭自瞄
#define AUTOAIM_ON      1       //打开自瞄

//自瞄权重（用于 AutoAim_WeightFusion 函数，保留兼容）
#define AUTOAIM_WEIGHT_AUTO         80
#define AUTOAIM_WEIGHT_MANUAL      (100 - AUTOAIM_WEIGHT_AUTO)

//自瞄修正增益：T_Angle -= Manual + AUTOAIM_GAIN × Auto
//手动全量保留，自瞄作为附加修正，不影响操作手感
#define AUTOAIM_GAIN                0.8f

//自瞄速度前馈增益（将目标角速度 °/s 映射到电机控制量）
#define AUTOAIM_FF_GAIN_YAW         50.0f    // Yaw 速度前馈增益
#define AUTOAIM_FF_GAIN_PITCH       8.0f     // Pitch 速度前馈增益

//自瞄速度低通滤波系数（越小越平滑）
#define AUTOAIM_FF_LPF_ALPHA        0.3f     // 新速度权重，1.0=无滤波

//自瞄速度前馈防飘/防冲参数
#define AUTOAIM_FF_DEADZONE         1.0f     // 速度死区(°/s)：|vel|<此值不给前馈，防静止漂移
#define AUTOAIM_FF_DECAY_K          3.0f     // 误差衰减常数：越大衰减越早，防过冲
#define AUTOAIM_FF_MAX_YAW          800      // Yaw前馈最大控制量，防单帧冲击
#define AUTOAIM_FF_MAX_PITCH        300      // Pitch前馈最大控制量

void AutoAim_UART_IRQHandler(void);//自瞄串口中断处理函数

#endif // AUTO_CONFIG_H_
