#ifndef DRAWUI_EXTEND_H_
#define DRAWUI_EXTEND_H_


/*
    这个文件用来存放一些UI生成器无法实现绘制的一些绘制辅助函数
*/
#include <math.h>
#include <stdint.h>

/* 宏定义 */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 角度(°)转弧度(rad)转换宏
 * @param deg 输入角度值，单位：度
 * @retval 转换后的弧度值，double精度
 * @note C语言标准cos/sin函数仅接收弧度作为输入
 */
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)


/* 对外函数 */

//线条旋转基础函数
void RotateLineByStart(float x0, float y0, float len, double angle, float base_angle_deg, uint8_t reverse_dir, float *out_x, float *out_y);
//线条旋转封装部分功能函数
void RotateLineUI(float x0, float y0, float len, double angle, float *out_x, float *out_y);
//角度插值：按百分比计算角度范围内的终点角度
float LerpAngle(float start_deg, float range_deg, float percent, int normalize);

#endif // DRAWUI_EXTEND_H_
