#include "DrawUI_Extend.h"

//PI
#define M_PI 3.14159265358979323846
// 角度转弧度
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)

/**
 * @brief 线段绕起点旋转，计算线段末端坐标
 * @param x0,y0 线段起始点
 * @param len   线段长度
 * @param angle 旋转角度(°)，正数逆时针，负数顺时针
 * @param out_x 输出末端x
 * @param out_y 输出末端y
 */
void RotateLineByStart(float x0, float y0, float len, float angle, float *out_x, float *out_y)
{
    // 转弧度
    double rad = DEG_TO_RAD(angle);
    double dx = len * cos(rad);
    double dy = len * sin(rad);

    *out_x = x0 + (float)dx;
    *out_y = y0 + (float)dy;
}

